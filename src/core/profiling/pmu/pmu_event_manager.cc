
#include "core/profiling/pmu/pmu_event_manager.hh"

namespace optkit::core::pmu
{
    std::map<int32_t, int32_t> PMUEventManager::fd__event_count_map;
    int32_t PMUEventManager::event_count_being_monitor = 0;

    bool PMUEventManager::register_event(int32_t fd, int32_t num_events)
    {

        // before adding fd, check if fd exists
        if (OPT_UNLIKELY(PMUEventManager::fd__event_count_map.find(fd) != PMUEventManager::fd__event_count_map.end()))
        {
            OPTKIT_CORE_WARN("fd {} already exists", fd);
            return false;
        }

        // increment event count
        PMUEventManager::event_count_being_monitor += num_events;

        // before adding fd, check if total event coun exceeds the systems' resources. then print a warning!
        int32_t num_cntrs = QueryPMU::default_pmu_info().num_cntrs;
        if (OPT_LIKELY(PMUEventManager::event_count_being_monitor > num_cntrs))
        {
            OPTKIT_CORE_WARN("Total # of events exceed system resources!! {}/{}(max) Multiplexing will take place(BlockGroup is not created by the system when this occures).", PMUEventManager::event_count_being_monitor, num_cntrs);
        }

        // add event
        PMUEventManager::fd__event_count_map.insert({fd, num_events});
        return true;
    }

    int32_t PMUEventManager::unregister_event(int32_t fd)
    {
        close(fd);
        int32_t tmp = PMUEventManager::fd__event_count_map[fd];
        PMUEventManager::event_count_being_monitor -= tmp;
        PMUEventManager::fd__event_count_map.erase(fd);
        return tmp;
    }

    void PMUEventManager::disable_all_events()
    {
        // disable all other counters in insertion order
        for (const auto &pair : PMUEventManager::fd__event_count_map)
            ioctl(pair.first, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
    }
    void PMUEventManager::enable_all_events()
    {
        // enable all other counters in reverse order
        auto rit = PMUEventManager::fd__event_count_map.rbegin();
        while (rit != PMUEventManager::fd__event_count_map.rend())
        {
            ioctl(rit->first, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
            ++rit;
        }
    }

    std::vector<int32_t> PMUEventManager::all_fds()
    {
        std::vector<int32_t> result;
        for (const auto &pair : PMUEventManager::fd__event_count_map)
            result.push_back(pair.first);

        return result;
    }

    int32_t PMUEventManager::number_of_events_being_monitored()
    {
        return PMUEventManager::event_count_being_monitor;
    }

    int32_t PMUEventManager::pmu_event_size()
    {
        return QueryPMU::default_pmu_info().num_cntrs;
    }
};