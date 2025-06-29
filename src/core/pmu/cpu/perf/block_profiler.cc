
#include "core/pmu/cpu/perf/block_profiler.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::core::pmu::cpu::perf
{

    BlockProfiler::BlockProfiler(const char *block_name, const core::metrics::MetricBuilder &mb, bool verbose, const PerfProfilerConfig &config) : BaseProfiler{block_name, "cpu_pmu", verbose}, profiler_config{config}, metric_builder{mb}
    {
        PMUEventManager::disable_all_events();

        int32_t fd = -1;
        for (const auto &raw_event : this->metric_builder.metric_events)
        {
            struct perf_event_attr attr = this->profiler_config.perf_event_config;
            attr.config = raw_event.second;

            fd = syscall(__NR_perf_event_open, &attr, this->profiler_config.pid, this->profiler_config.cpu, -1, 0);
            if (fd < 0)
            {
                OPTKIT_CORE_ERROR("perf_event_open error");
                return;
            }
            else
            {
                PMUEventManager::register_event(fd, 1);
                fd_list.push_back(fd);
            }
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        }

        start = std::chrono::high_resolution_clock::now();
        PMUEventManager::enable_all_events();
    }

    BlockProfiler ::~BlockProfiler()
    {
        PMUEventManager::disable_all_events();

        this->read_and_store();

        for (int32_t fd : fd_list)
            PMUEventManager::unregister_event(fd); // unregister this event

        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(profiler_config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->verbose))
        {
            std::cout << "\033[1;35m"
                      << "Block: " << this->block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;

            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }

        PMUEventManager::enable_all_events();
    }

    void BlockProfiler::disable()
    {
        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        }
    }
    void BlockProfiler::enable()
    {
        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        }
    }

    void BlockProfiler::reset()
    {
        for (int32_t fd : fd_list)
        {
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        }
    }

    std::string BlockProfiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << utils::to_json(this->total_duration_ms, this->measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::vector<uint64_t> BlockProfiler::read()
    {
        PMUEventManager::disable_all_events();

        std::vector<uint64_t> result;
        uint64_t count;
        for (int32_t fd : fd_list)
        {
            ::read(fd, &count, sizeof(count));
            result.push_back(count);
            if (OPT_LIKELY(this->profiler_config.is_reset_after_read))
                ioctl(fd, PERF_EVENT_IOC_RESET, 0);
        }

        PMUEventManager::enable_all_events();

        return result;
    }
    std::unordered_map<std::string, uint64_t> BlockProfiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, uint64_t> aggregated_events;
        const std::vector<std::string> &event_names = this->metric_builder.event_names();

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<uint64_t> &values = entry.second;

            for (size_t j = 0; j < values.size(); ++j)
            {
                aggregated_events[event_names[j]] += values[j];
            }
        }
        std::vector<std::pair<std::string, uint64_t>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        return aggregated_events;
    }

} // namespace optkit::core::pmu::cpu::perf

#endif