
#include "core/callstack/profiler.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT

namespace optkit::callstack
{
    namespace
    {
        constexpr double READ_BUFFER_FLUSH_PERIOD_MS = 5000.0;
    }

    namespace
    {
        class CallstackSession
        {
        public:
            static CallstackSession &instance()
            {
                static CallstackSession s;
                return s;
            }

            void acquire(uint32_t sample_freq, int32_t target_pid)
            {
                this->sample_freq_hz = sample_freq;
                postman.set_sample_freq(this->sample_freq_hz);
                postman.set_target_pid(target_pid);
                postman.start();
            }

            void release()
            {
                postman.stop();
            }

            void reset_counts()
            {
                postman.reset_counts();
            }

            std::unordered_map<std::string, uint64_t> consume_counts(bool reset_after)
            {
                return postman.consume_counts(reset_after);
            }

            std::string symbolize(uint64_t addr)
            {
                return postman.symbolize(addr);
            }

            CallstackSession(const CallstackSession &) = delete;
            CallstackSession(CallstackSession &&) = delete;

            void operator=(const CallstackSession &) = delete;
            void operator=(CallstackSession &&) = delete;

        private:
            CallstackSession() = default;
            ~CallstackSession()
            {
                postman.stop();
            }

        private:
            uint32_t sample_freq_hz{100};
            Postman postman;
        };

        static std::string symbolize_stack_trace(const std::string &address_chain)
        {
            std::string symbolized_stack;
            symbolized_stack.reserve(address_chain.size());

            size_t pos = 0;
            bool first = true;
            while (pos < address_chain.length())
            {
                size_t semicolon_pos = address_chain.find(';', pos);
                if (semicolon_pos == std::string::npos)
                    semicolon_pos = address_chain.length();

                std::string addr_str = address_chain.substr(pos, semicolon_pos - pos);

                if (!first)
                    symbolized_stack.push_back(';');

                char *end = nullptr;
                const uint64_t addr = std::strtoull(addr_str.c_str(), &end, 0);
                if (end == addr_str.c_str())
                    symbolized_stack += "?";
                else
                    symbolized_stack += CallstackSession::instance().symbolize(addr);

                first = false;
                pos = semicolon_pos + 1;
            }

            return symbolized_stack;
        }

    } // namespace

    // If the user doesn't provide anything, keep it conservative.
    static constexpr uint32_t DEFAULT_SAMPLE_FREQ_HZ = 100;

    Profiler::Profiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config)
        : BaseProfiler{static_cast<const optkit::ProfilerConfig &>(config)},
          profiler_config{config},
          sample_freq_hz{DEFAULT_SAMPLE_FREQ_HZ}
    {
        // Use PerfProfilerConfig::pid as the target process ID. getpid() is set by default.
        CallstackSession::instance().acquire(this->sample_freq_hz, this->profiler_config.pid);
        start = std::chrono::high_resolution_clock::now();
    }

    Profiler::~Profiler()
    {
        this->read_and_store();
        (void)this->aggregate();

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;35m"
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            for (auto &&event : this->event_results)
                std::cout << "\t" << event.first << ": " << event.second << std::endl;
        }

        CallstackSession::instance().release();
    }

    void Profiler::disable()
    {
        this->is_enabled = false;
        CallstackSession::instance().release();
    }

    void Profiler::enable()
    {
        this->is_enabled = true;
        CallstackSession::instance().acquire(this->sample_freq_hz, this->profiler_config.pid);
    }

    void Profiler::reset()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return;
        CallstackSession::instance().reset_counts();
    }

    std::string Profiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        nlohmann::json out;
        nlohmann::json reading;
        reading["duration"] = this->total_duration_ms;
        reading["duration_unit"] = "ms";
        reading["measurement_type"] = this->config.measurement_type;
        reading["block_name"] = this->config.block_name;

        for (const auto &kv : this->event_results)
        {
            nlohmann::json entry;
            entry["stack"] = kv.first;
            entry["count"] = kv.second;
            reading["samples"].push_back(entry);
        }

        out["readings"].push_back(reading);
        return out.dump(2) + "\n";
    }

    std::unordered_map<std::string, uint64_t> Profiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        // Make sure this thread is included in the global registry.
        // CallstackSession::instance().touch_current_thread();
        return CallstackSession::instance().consume_counts(this->config.is_reset_after_read);
    }

    void Profiler::on_sample_stored(const std::pair<double, std::unordered_map<std::string, uint64_t>> &sample)
    {
        this->buffered_duration_ms += sample.first;
        if (this->buffered_duration_ms < READ_BUFFER_FLUSH_PERIOD_MS)
            return;

        flush_compacted_samples();
    }

    void Profiler::flush_compacted_samples()
    {
        if (this->read_buffer.empty())
            return;

        for (size_t index = 0; index < this->read_buffer.size(); ++index)
        {
            const std::unordered_map<std::string, uint64_t> &sample_counts = this->read_buffer[index].second;
            for (std::unordered_map<std::string, uint64_t>::const_iterator it = sample_counts.begin(); it != sample_counts.end(); ++it)
                this->compacted_event_counts[symbolize_stack_trace(it->first)] += it->second;
        }

        this->compacted_duration_ms += this->buffered_duration_ms;
        this->buffered_duration_ms = 0.0;
        this->read_buffer.clear();
    }

    std::unordered_map<std::string, uint64_t> Profiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        double total_duration = this->compacted_duration_ms;
        std::unordered_map<std::string, uint64_t> aggregated = this->compacted_event_counts;

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            for (const auto &kv : entry.second)
            {
                aggregated[symbolize_stack_trace(kv.first)] += kv.second;
            }
        }

        std::vector<std::pair<std::string, uint64_t>> event_value(aggregated.begin(), aggregated.end());
        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        return aggregated;
    }

} // namespace optkit::callstack

#endif