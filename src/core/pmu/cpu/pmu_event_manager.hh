#pragma once

#include <vector>
#include <map>
#include <sys/ioctl.h>

#include "utils/utils.hh"
#include "core/pmu/cpu/query_pmu.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
#include <linux/perf_event.h>
#endif

namespace optkit::core::pmu::cpu
{
    class PMUEventManager
    {

    public:
        /**
         * @brief Register a file descriptor
         *
         * @param fd file descriptor itself @see BlockProfiler or BlockGroupProfiler
         * @param num_events number of events being registered for this fd.
         * @return bool wether fd is saved successfully
         *
         */
        static bool register_event(int32_t fd, int32_t num_events);

        /**
         * @brief Unregister a file descriptor
         *
         * @param fd file descriptor itself @see BlockProfiler or BlockGroupProfiler
         * @return uint32_t num of events were being monitored with this fd
         *
         */
        static int32_t unregister_event(int32_t fd);

        static void disable_all_events();
        static void enable_all_events();

        static std::vector<int32_t> all_fds();
        static int32_t number_of_events_being_monitored();

        // returns number of counters
        static int32_t pmu_num_cntrs();

#ifdef OPTKIT_TESTING   // adds this for testing build
        static void reset()
        {
            PMUEventManager::fd__event_count_map.clear();
            PMUEventManager::event_count_being_monitor = 0;
        }
#endif

    private:
        static std::map<int32_t, int32_t> fd__event_count_map; // insertion order is important for enable/disable ordering
        static int32_t event_count_being_monitor;

    private:
        PMUEventManager();
        ~PMUEventManager();
    };
}