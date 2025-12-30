
#include "core/pmu/cpu/perf/block_profiler.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::pmu::cpu::perf
{

    // this is the sampling function that runs in a separate thread
    // it calls read_and_store every sampling_frequency_sec seconds
    OPT_FORCE_INLINE void sampling_function(BlockProfiler &profiler)
    {
        // std::cout << "Sampling function for pmu\n";
        profiler.read_and_store();
    }

    BlockProfiler::BlockProfiler(const PerfProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb)
        : BaseProfiler{static_cast<const ProfilerConfig &>(profiler_config)}, profiler_config{profiler_config}, metric_builder{mb}
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
                this->is_enabled = false;
                return;
            }
            else
            {
                PMUEventManager::register_event(fd, 1);
                fd_list.push_back(fd);
            }
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
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

        this->metric_results = this->metric_builder.calculate(aggregate());

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
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

        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;

            const std::vector<uint64_t> &values = entry.second;

            // std::cout << "read buffer:";
            for (size_t j = 0; j < values.size(); ++j)
            {
                // std::cout << event_names[j] << ":" << values[j] << "\n";
                aggregated_events[event_names[j]] += values[j];
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