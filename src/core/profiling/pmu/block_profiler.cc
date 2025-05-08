#include "core/profiling/pmu/block_profiler.hh"

namespace optkit::core::pmu
{

    BlockProfiler::BlockProfiler(const char *block_name, const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, bool verbose, const ProfilerConfig &config) : BaseProfiler{block_name, event_name,verbose}, profiler_config{config}, raw_events{raw_events}
    {
        PMUEventManager::disable_all_events();

        int32_t fd = -1;
        for (auto &raw_event : raw_events)
        {
            struct perf_event_attr attr = this->profiler_config.perf_event_config;  // copy default config.
            attr.config = raw_event.first;  // set an event

            fd = syscall(__NR_perf_event_open, &attr, this->profiler_config.pid, this->profiler_config.cpu, -1, 0); // <-- first becomes -1 and later we use the group_leader's fd.
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
        // enable clock now!
        start = std::chrono::high_resolution_clock::now();

        PMUEventManager::enable_all_events();
    }
    BlockProfiler ::~BlockProfiler()
    {
        PMUEventManager::disable_all_events();

        this->read();

        for (int32_t fd : fd_list)
        {
            PMUEventManager::unregister_event(fd); // unregister this event
        }

        if (OPT_LIKELY(profiler_config.dump_results_to_file))
            this->save();
        else if (OPT_LIKELY(this->verbose))
        {
            auto ctr = 0u;
            for (auto iter = this->read_buffer.rbegin(); ctr < raw_events.size() && iter != this->read_buffer.rend(); iter++, ctr++)
            {
                std::cout << "\033[1;35m"
                          << "Block: " << this->block_name << "\033[0m"
                          << " [" << iter->first << "ms] Measured";
                for (auto &&i : iter->second)
                {
                    std::cout << " "<< i << std::endl;
                }
            }
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

    std::string BlockProfiler::convert_buffer_to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        // based on the insertion order.
        ss << core::pmu::to_json(this->event_name, this->raw_events, this->read_buffer);
        ss << "]\n";
        return ss.str();
    }

    std::vector<uint64_t> BlockProfiler::read_val()
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
} // namespace optkit::core::pmu