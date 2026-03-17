
#include "core/pmu/cpu/perf/block_group_profiler.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::pmu::cpu::perf
{

    // this is the sampling function that runs in a separate thread
    // it calls read_and_store every sampling_frequency_sec seconds
    OPT_FORCE_INLINE void sampling_function(BlockGroupProfiler &profiler)
    {
        // std::cout << "Sampling function for pmu group\n";
        profiler.read_and_store();
    }

    BlockGroupProfiler::BlockGroupProfiler(const PerfProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
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

        const int32_t core_begin = (this->profiler_config.cpu == -1) ? -1 : 0;
        const int32_t core_end_exclusive = (this->profiler_config.cpu == -1) ? 0 : ((this->profiler_config.cpu > 0) ? this->profiler_config.cpu : 1);

        for (int32_t core = core_begin; core < core_end_exclusive; ++core)
        {
            int32_t group_leader = -1;
            for (const auto &raw_event : mb.metric_events)
            {
                struct perf_event_attr attr = this->profiler_config.perf_event_config; // copy default config
                attr.config = raw_event.second;                                        // set an event

                int32_t fd = syscall(__NR_perf_event_open, &attr, this->profiler_config.pid, core, group_leader, 0);
                if (fd < 0)
                {
                    OPTKIT_CORE_ERROR("perf_event_open error");
                    this->is_enabled = false;
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
            group_leaders.push_back(group_leader);
        }

        if (OPT_UNLIKELY(this->profiler_config.is_sampling))
            this->sampling_thread = std::thread([this]()
                                                {
            this->is_sampling = true;
            while (this->is_sampling)
            { 
                sampling_function(*this);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } });

        this->reset();
        start = std::chrono::high_resolution_clock::now();
        PMUEventManager::enable_all_events();
    }

    BlockGroupProfiler ::~BlockGroupProfiler()
    {
        PMUEventManager::disable_all_events();

        if (!is_configured || !is_enabled)
            return;

        if (this->profiler_config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

        this->read_and_store(); // read the last one.

        for (int32_t group_leader : group_leaders)
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
        this->is_enabled = false;
        for (int32_t group_leader : group_leaders)
            ioctl(group_leader, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
    }

    void BlockGroupProfiler::enable()
    {
        this->is_enabled = true;
        for (int32_t group_leader : group_leaders)
            ioctl(group_leader, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
    }

    void BlockGroupProfiler::reset()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return;
        for (int32_t group_leader : group_leaders)
            ioctl(group_leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
    }

    std::vector<uint64_t> BlockGroupProfiler::read()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};
        PMUEventManager::disable_all_events();

        std::vector<uint64_t> result;

        if (!is_configured)
            return {0};

        char buf[4096];
        struct read_format *rf = (struct read_format *)buf;
        for (int32_t group_leader : group_leaders)
        {
            ::read(group_leader, buf, sizeof(buf));
            for (uint64_t i = 0; i < rf->nr; i++)
            {
                result.push_back(rf->values[i].value);
            }
            if (OPT_LIKELY(this->profiler_config.is_reset_after_read))
                ioctl(group_leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
        }

        PMUEventManager::enable_all_events();

        return result;
    }

    std::string BlockGroupProfiler::to_json()
    {
        if (OPT_UNLIKELY(!is_enabled))
            return {};

        std::stringstream ss;
        ss << "[\n";
        ss << utils::to_json<uint64_t>(this->total_duration_ms, this->config.measurement_type, this->event_results, this->metric_results);
        ss << "]\n";
        return ss.str();
    }

    std::unordered_map<std::string, uint64_t> BlockGroupProfiler::aggregate()
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

        this->event_results = event_value;
        this->total_duration_ms = total_duration;

        aggregated_events["duration_microsec"] = this->total_duration_ms * 1000.0; // convert to microseconds
        return aggregated_events;
    }
} // namespace optkit::pmu::cpu::perf

#endif