
#include "core/pmu/cpu/perf/block_group_profiler.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::core::pmu::cpu::perf
{

    BlockGroupProfiler::BlockGroupProfiler(const PerfProfilerConfig &profiler_config, const core::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, group_leader{-1}, profiler_config{profiler_config}, metric_builder{mb}
    {
        PMUEventManager::disable_all_events();

        is_configured = true;
        if ((int32_t)mb.metric_events.size() >= PMUEventManager::pmu_num_cntrs())
        {
            is_configured = false;
            OPTKIT_CORE_ERROR("Cannot create a blockgroup for block {} by monitoring more than pmu hardware event size {}|{}(max).", this->profiler_config.block_name, mb.metric_events.size(), PMUEventManager::pmu_num_cntrs());
            OPTKIT_CORE_WARN("Consider dividing the BlockGroupProfiler for block {} into multiple sub-groups!", this->profiler_config.block_name);
            return;
        }

        for (const auto &raw_event : mb.metric_events)
        {
            struct perf_event_attr attr = this->profiler_config.perf_event_config; // copy default config
            attr.config = raw_event.second;                                        // set an event

            int32_t fd = syscall(__NR_perf_event_open, &attr, this->profiler_config.pid, this->profiler_config.cpu, group_leader, 0); // <-- first becomes -1 and later we use the group_leader's fd.
            if (fd < 0)
            {
                OPTKIT_CORE_ERROR("perf_event_open error");
                return;
            }
            else
            {
                if (group_leader == -1)
                {
                    group_leader = fd;
                    PMUEventManager::register_event(group_leader, mb.metric_events.size());
                }
            }
        }

        this->reset();
        start = std::chrono::high_resolution_clock::now();
        PMUEventManager::enable_all_events();
    }

    BlockGroupProfiler ::~BlockGroupProfiler()
    {
        PMUEventManager::disable_all_events();

        if (is_configured == false)
            return;

        this->read_and_store(); // read the last one.

        PMUEventManager::unregister_event(group_leader);

        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;35m"
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << std::fixed << "\t" << event.first << ": " << event.second << std::endl;
            for (auto &&metric : this->metric_results)
                std::cout << std::fixed << "\t" << metric.first << ": " << metric.second << std::endl;
        }

        PMUEventManager::enable_all_events();
    }

    void BlockGroupProfiler::disable()
    {
        ioctl(group_leader, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
    }

    void BlockGroupProfiler::enable()
    {
        ioctl(group_leader, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
    }

    void BlockGroupProfiler::reset()
    {
        ioctl(group_leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    }

    std::vector<uint64_t> BlockGroupProfiler::read()
    {
        PMUEventManager::disable_all_events();

        std::vector<uint64_t> result;

        if (!is_configured)
            return {0};

        char buf[4096];
        struct read_format *rf = (struct read_format *)buf;
        ::read(group_leader, buf, sizeof(buf));
        for (uint64_t i = 0; i < rf->nr; i++)
        {
            result.push_back(rf->values[i].value);
        }
        if (OPT_LIKELY(this->profiler_config.is_reset_after_read))
            ioctl(group_leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);

        PMUEventManager::enable_all_events();

        return result;
    }

    std::string BlockGroupProfiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::unordered_map<std::string, uint64_t> BlockGroupProfiler::aggregate()
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

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }
} // namespace optkit::core::pmu::cpu::perf

#endif