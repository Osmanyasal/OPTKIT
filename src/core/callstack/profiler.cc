
#include "core/callstack/profiler.hh"

#include "core/callstack/perf_mailbox.hh"

#include <chrono>

#if OPTKIT_ENV_LIB_PERF_EVENT

namespace optkit::callstack
{
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

            void acquire(uint32_t sample_freq)
            {
                std::lock_guard<std::mutex> l(lock);
                sample_freq_hz = sample_freq;
                ref_count++;
                sweeper.set_sample_freq(sample_freq);
                sweeper.start();
            }

            void release()
            {
                std::lock_guard<std::mutex> l(lock);
                if (ref_count == 0)
                    return;
                ref_count--;
                if (ref_count == 0)
                    sweeper.stop();
            }

            void touch_current_thread()
            {
                touch_thread(sample_freq_hz);
            }

            void reset_counts()
            {
                sweeper.reset_counts();
            }

            std::unordered_map<std::string, uint64_t> consume_counts(bool reset_after)
            {
                return sweeper.consume_counts(reset_after);
            }

        private:
            std::mutex lock;
            uint32_t sample_freq_hz{100};
            uint32_t ref_count{0};
            Sweeper sweeper;
        };

    } // namespace

    // If the user doesn't provide anything, keep it conservative.
    static constexpr uint32_t DEFAULT_SAMPLE_FREQ_HZ = 100;

    Profiler::Profiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config)
        : BaseProfiler{static_cast<const optkit::ProfilerConfig &>(config)},
          profiler_config{config},
          sample_freq_hz{DEFAULT_SAMPLE_FREQ_HZ}
    {
        // Ensure the current thread is registered (this is the library version of "touch_profiler()" in the sample).
        CallstackSession::instance().acquire(sample_freq_hz);
        CallstackSession::instance().touch_current_thread();

        if (OPT_UNLIKELY(this->profiler_config.is_sampling))
        {
            this->sampling_thread = std::thread([this]()
                                                {
                this->is_sampling = true;
                while (this->is_sampling)
                {
                    this->read_and_store();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                } });
        }

        start = std::chrono::high_resolution_clock::now();
    }

    Profiler::~Profiler()
    {
        if (this->profiler_config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

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
    }

    void Profiler::enable()
    {
        this->is_enabled = true;
        CallstackSession::instance().touch_current_thread();
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
        CallstackSession::instance().touch_current_thread();
        return CallstackSession::instance().consume_counts(this->config.is_reset_after_read);
    }

    std::unordered_map<std::string, uint64_t> Profiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        double total_duration = 0.0;
        std::unordered_map<std::string, uint64_t> aggregated;

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            for (const auto &kv : entry.second)
                aggregated[kv.first] += kv.second;
        }

        std::vector<std::pair<std::string, uint64_t>> event_value(aggregated.begin(), aggregated.end());
        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        return aggregated;
    }

} // namespace optkit::callstack

#endif