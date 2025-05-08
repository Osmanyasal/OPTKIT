#pragma once

#include <vector>
#include <map>
#include <sys/ioctl.h>
#include <linux/perf_event.h>

#include "utils/utils.hh"
#include "core/profiling/pmu/query_pmu.hh"
namespace optkit::core::pmu
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

        static int32_t pmu_event_size();

    private:
        static std::map<int32_t, int32_t> fd__event_count_map; // insertion order is important for enable/disable ordering
        static int32_t event_count_being_monitor;

    private:
        PMUEventManager();
        ~PMUEventManager();
    };
}