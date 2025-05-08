#include "core/profiling/pmu/block_group_profiler.hh"

namespace optkit::core::pmu
{

    BlockGroupProfiler::BlockGroupProfiler(const char *block_name, const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, bool verbose, const ProfilerConfig &config) : BaseProfiler{block_name, event_name, verbose}, profiler_config{config}, group_leader{-1}, is_active{true}, raw_events{raw_events}
    {

        PMUEventManager::disable_all_events();

        if ((int32_t)raw_events.size() > PMUEventManager::pmu_event_size())
        {
            this->is_active = false;
            OPTKIT_CORE_ERROR("Cannot create a blockgroup for block {} by monitoring more than pmu hardware event size {}|{}(max).", this->block_name, raw_events.size(), PMUEventManager::pmu_event_size());
            OPTKIT_CORE_WARN("Consider dividing the BlockGroupProfiler for block {} into multiple sub-groups!", this->block_name);
            return;
        }

        for (auto &raw_event : raw_events)
        {
            struct perf_event_attr attr = this->profiler_config.perf_event_config; // copy default config
            attr.config = raw_event.first;                                         // set an event

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
                    PMUEventManager::register_event(group_leader, raw_events.size());
                }
            }
        }

        ioctl(group_leader, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
        // enable clock now!
        start = std::chrono::high_resolution_clock::now();

        PMUEventManager::enable_all_events();
    }
    BlockGroupProfiler ::~BlockGroupProfiler()
    {

        PMUEventManager::disable_all_events();

        if (OPT_UNLIKELY(!is_active))
        {
            OPTKIT_CORE_WARN("BlockGroupProfiler for block {} is not active!", this->block_name);
            return;
        }
        this->read();

        PMUEventManager::unregister_event(group_leader);

        if (OPT_LIKELY(profiler_config.dump_results_to_file))
            this->save();

        else if(OPT_LIKELY(this->verbose))
        {
            uint64_t ctr = 0u;
            for (auto iter = this->read_buffer.rbegin(); ctr < raw_events.size() && iter != this->read_buffer.rend(); iter++, ctr++)
            {
                std::cout << "\033[1;35m"
                          << "Block: " << this->block_name << "\033[0m"
                          << " [" << iter->first << "ms] Measured";
                for (auto &&i : iter->second)
                {
                    std::cout << " " << i << std::endl;
                }
            }
        }

        PMUEventManager::enable_all_events();
    }

    void BlockGroupProfiler::disable()
    {
        if (OPT_UNLIKELY(!is_active))
        {
            OPTKIT_CORE_WARN("BlockGroupProfiler for block {} is not active!", this->block_name);
        }

        ioctl(group_leader, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
    }
    void BlockGroupProfiler::enable()
    {
        if (OPT_UNLIKELY(!is_active))
        {
            OPTKIT_CORE_WARN("BlockGroupProfiler for block {} is not active!", this->block_name);
        }

        ioctl(group_leader, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
    }

    std::vector<uint64_t> BlockGroupProfiler::read_val()
    {

        PMUEventManager::disable_all_events();

        std::vector<uint64_t> result;
        if (OPT_UNLIKELY(!is_active))
        {
            OPTKIT_CORE_WARN("BlockGroupProfiler for block {} is not active!", this->block_name);
            return result;
        }

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

    std::string BlockGroupProfiler::convert_buffer_to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << core::pmu::to_json(this->event_name, this->raw_events, this->read_buffer);
        ss << "]\n";
        return ss.str();
    }
} // namespace optkit::core::pmu