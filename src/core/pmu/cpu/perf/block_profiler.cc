
#include "core/pmu/cpu/perf/block_profiler.hh"

#include <unordered_set>

#if OPTKIT_ENV_CPU_RISCV
#include "core/pmu/cpu/perf/riscv/event_resolver.hh"
#endif

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::pmu::cpu::perf
{

    OPT_FORCE_INLINE std::vector<std::string> unique_event_names_from(const std::vector<std::string> &event_names)
    {
        std::unordered_set<std::string> seen;
        std::vector<std::string> unique;
        unique.reserve(event_names.size());
        for (const auto &name : event_names)
        {
            if (seen.insert(name).second)
                unique.push_back(name);
        }
        return unique;
    }

    OPT_FORCE_INLINE std::vector<std::pair<std::string, uint64_t>> event_values_from_counts(
        const std::vector<std::string> &unique_event_names,
        const std::unordered_map<std::string, uint64_t> &counts)
    {
        std::vector<std::pair<std::string, uint64_t>> event_values;
        event_values.reserve(unique_event_names.size());
        for (const auto &name : unique_event_names)
        {
            auto it = counts.find(name);
            event_values.emplace_back(name, (it == counts.end()) ? 0 : it->second);
        }
        return event_values;
    }

    OPT_FORCE_INLINE std::unordered_map<std::string, uint64_t> aggregate_counts_from_read_buffer(
        const std::vector<std::string> &event_names,
        const std::vector<std::pair<double, std::vector<uint64_t>>> &read_buffer,
        double total_duration_ms)
    {
        std::unordered_map<std::string, uint64_t> aggregated_counts;
        for (const auto &sample : read_buffer)
        {
            const std::vector<uint64_t> &values = sample.second;
            for (size_t j = 0; j < values.size(); ++j)
                aggregated_counts[event_names[j % event_names.size()]] += values[j];
        }
        aggregated_counts["duration_microsec"] = static_cast<uint64_t>(total_duration_ms * 1000.0);
        return aggregated_counts;
    }

    OPT_FORCE_INLINE void append_reading_json(
        nlohmann::json &out,
        double duration_ms,
        const char *measurement_type,
        const std::vector<std::pair<std::string, uint64_t>> &event_values,
        const std::vector<std::pair<std::string, double>> &metric_values)
    {
        nlohmann::json single = utils::to_json<uint64_t>(duration_ms, measurement_type, event_values, metric_values);
        if (single.contains("readings") && single["readings"].is_array() && !single["readings"].empty())
            out["readings"].push_back(single["readings"][0]);
    }

    OPT_FORCE_INLINE std::unordered_map<std::string, uint64_t> event_counts_from_sample(
        const std::vector<std::string> &event_names,
        const std::vector<uint64_t> &values,
        double duration_ms)
    {
        std::unordered_map<std::string, uint64_t> counts;
        if (event_names.empty() || values.empty())
        {
            counts["duration_microsec"] = static_cast<uint64_t>(duration_ms * 1000.0);
            return counts;
        }

        for (size_t j = 0; j < values.size(); ++j)
        {
            counts[event_names[j % event_names.size()]] += values[j];
        }
        counts["duration_microsec"] = static_cast<uint64_t>(duration_ms * 1000.0);
        return counts;
    }

    // this is the sampling function that runs in a separate thread
    // it calls read_and_store every sampling_frequency_sec seconds
    OPT_FORCE_INLINE void sampling_function(BlockProfiler &profiler)
    {
        profiler.read_and_store();
    }

    BlockProfiler::BlockProfiler(const PerfProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
    {
        PMUEventManager::disable_all_events();

        int32_t fd = -1;
        const int32_t core_begin = (this->profiler_config.cpu == -1) ? -1 : 0;
        const int32_t core_end_exclusive = (this->profiler_config.cpu == -1) ? 0 : ((this->profiler_config.cpu > 0) ? this->profiler_config.cpu : 1);

        for (int32_t core = core_begin; core < core_end_exclusive; ++core)
        {
            for (const auto &raw_event : this->metric_builder.metric_events)
            {
                struct perf_event_attr attr = this->profiler_config.perf_event_config;
                attr.config = raw_event.second;

#if OPTKIT_ENV_CPU_RISCV
                riscv::apply_event_attr(attr, raw_event.first, raw_event.second);
#endif

                fd = syscall(__NR_perf_event_open, &attr, this->profiler_config.pid, core, -1, 0);
                if (fd < 0)
                {
                    OPTKIT_CORE_ERROR("perf_event_open error");
                    this->is_enabled = false;
                    return;
                }
                else
                {
                    // std::cout << "Registered for core " << core << " event " << raw_event.first << " with code 0x" << std::hex << raw_event.second << std::dec << " and fd " << fd << "\n";
                    PMUEventManager::register_event(fd, 1);
                    fd_list.push_back(fd);
                }
                ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            }
        }
        if (OPT_UNLIKELY(this->profiler_config.is_sampling))
        {
            this->is_sampling = true;
            this->sampling_thread = std::thread([this]()
                                                {
            while (this->is_sampling)
            { 
                sampling_function(*this);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } });
        }

        start = std::chrono::high_resolution_clock::now();
        PMUEventManager::enable_all_events();
    }

    BlockProfiler ::~BlockProfiler()
    {
        PMUEventManager::disable_all_events();

        if (this->profiler_config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

        this->read_and_store();

        for (int32_t fd : fd_list)
            PMUEventManager::unregister_event(fd); // unregister this event

        if (OPT_UNLIKELY(this->config.is_screenshot))
        {
            double total_duration = 0.0;
            for (const auto &entry : this->read_buffer)
                total_duration += entry.first;
            this->total_duration_ms = total_duration;

            this->event_results.clear();
            this->metric_results.clear();
        }
        else
        {
            this->metric_results = this->metric_builder.calculate(aggregate());
        }

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            if (OPT_UNLIKELY(this->config.is_screenshot))
            {
                std::cout << "\033[1;35m"
                          << "Block: " << this->config.block_name << "\033[0m"
                          << " [" << this->total_duration_ms << "ms] Screenshot samples (" << this->read_buffer.size() << ")\n";

                const std::vector<std::string> &event_names = this->metric_builder.event_names();
                const auto unique_event_names = unique_event_names_from(event_names);

                if (!this->read_buffer.empty())
                {
                    const std::unordered_map<std::string, uint64_t> aggregated_counts =
                        aggregate_counts_from_read_buffer(event_names, this->read_buffer, this->total_duration_ms);
                    const auto aggregated_metrics = this->metric_builder.calculate(aggregated_counts);

                    std::cout << std::fixed << "\t[Aggregated] duration_ms: " << this->total_duration_ms << std::endl;
                    if (OPT_UNLIKELY(this->metric_builder.print_events))
                    {
                        for (const auto &name : unique_event_names)
                        {
                            auto it = aggregated_counts.find(name);
                            std::cout << std::fixed << "\t\t" << name << ": " << ((it == aggregated_counts.end()) ? 0 : it->second) << std::endl;
                        }
                    }
                    for (const auto &metric : aggregated_metrics)
                        std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
                }

                for (size_t sample_idx = 0; sample_idx < this->read_buffer.size(); ++sample_idx)
                {
                    const auto &sample = this->read_buffer[sample_idx];
                    const double duration_ms = sample.first;
                    const std::unordered_map<std::string, uint64_t> sample_counts =
                        event_counts_from_sample(event_names, sample.second, duration_ms);
                    const auto sample_metrics = this->metric_builder.calculate(sample_counts);

                    std::cout << std::fixed << "\t[Sample " << sample_idx << "] duration_ms: " << duration_ms << std::endl;

                    if (OPT_UNLIKELY(this->metric_builder.print_events))
                    {
                        for (const auto &name : unique_event_names)
                        {
                            auto it = sample_counts.find(name);
                            std::cout << std::fixed << "\t\t" << name << ": " << ((it == sample_counts.end()) ? 0 : it->second) << std::endl;
                        }
                    }

                    for (const auto &metric : sample_metrics)
                        std::cout << std::fixed << "\t\t" << metric.first << ": " << metric.second << std::endl;
                }
            }
            else
            {
                std::cout << "\033[1;35m"
                          << "Block: " << this->config.block_name << "\033[0m"
                          << " [" << this->total_duration_ms << "ms] Measured\n";

                if (OPT_UNLIKELY(this->metric_builder.print_events))
                    for (auto &&event : this->event_results)
                        std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;

                for (auto &&metric : this->metric_results)
                    std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
            }
        }

        PMUEventManager::enable_all_events();
    }

    void BlockProfiler::disable()
    {
        this->is_enabled = false;
        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        }
    }
    void BlockProfiler::enable()
    {
        this->is_enabled = true;
        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        }
    }

    void BlockProfiler::reset()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return;

        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        }
    }

    std::string BlockProfiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        if (OPT_UNLIKELY(this->config.is_screenshot))
        {
            nlohmann::json out;

            const std::vector<std::string> &event_names = this->metric_builder.event_names();
            const auto unique_event_names = unique_event_names_from(event_names);

            if (!this->read_buffer.empty())
            {
                const std::unordered_map<std::string, uint64_t> aggregated_counts =
                    aggregate_counts_from_read_buffer(event_names, this->read_buffer, this->total_duration_ms);
                const auto aggregated_event_values = event_values_from_counts(unique_event_names, aggregated_counts);
                const auto aggregated_metric_values = this->metric_builder.calculate(aggregated_counts);

                append_reading_json(out,
                                   this->total_duration_ms,
                                   this->config.measurement_type,
                                   aggregated_event_values,
                                   aggregated_metric_values);
            }

            for (const auto &sample : this->read_buffer)
            {
                const double duration_ms = sample.first;
                const std::unordered_map<std::string, uint64_t> sample_counts =
                    event_counts_from_sample(event_names, sample.second, duration_ms);

                const auto event_values = event_values_from_counts(unique_event_names, sample_counts);
                const std::vector<std::pair<std::string, double>> metric_values =
                    this->metric_builder.calculate(sample_counts);

                append_reading_json(out,
                                   duration_ms,
                                   this->config.measurement_type,
                                   event_values,
                                   metric_values);
            }

            std::stringstream ss;
            ss << "[\n";
            ss << out.dump(2);
            ss << "\n]\n";
            return ss.str();
        }

        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::vector<uint64_t> BlockProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        PMUEventManager::disable_all_events();

        std::vector<uint64_t> result;
        uint64_t count;
        for (int32_t fd : fd_list)
        {
            ::read(fd, &count, sizeof(count));
            result.push_back(count);
            // std::cout << "Read value " << count << " from fd " << fd << "\n";
            if (OPT_LIKELY(this->config.is_reset_after_read))
                ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        }

        PMUEventManager::enable_all_events();

        return result;
    }
    std::unordered_map<std::string, uint64_t> BlockProfiler::aggregate()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        double total_duration = 0.0;
        std::unordered_map<std::string, uint64_t> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        if (event_names.empty())
        {
            for (const auto &entry : read_buffer)
                total_duration += entry.first;

            this->event_results.clear();
            this->total_duration_ms = total_duration;
            aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0;
            return aggregated_events;
        }

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<uint64_t> &values = entry.second;

            for (size_t j = 0; j < values.size(); ++j)
            {
                aggregated_events[event_names[j % event_names.size()]] += values[j];
            }
        }
        std::vector<std::pair<std::string, uint64_t>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }

} // namespace optkit::pmu::cpu::perf

#endif