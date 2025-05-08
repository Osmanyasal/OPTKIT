#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>

#include "utils/utils.hh"
#include "core/profiling/rapl/rapl.hh"
#include "core/profiling/pmu/libpfm4_wrapper.hh"
#include <unistd.h>
namespace optkit::core::pmu
{ 

    /**
     * @brief  ASK PMU related queries here<br>
     * This Query class uses <b>libpfm4</b> to retrieve information
     *
     * Don't forget to call init() before using it and destroy() when you're done with it.<br>
     * These 2 method calls can be done at the beginning and end of the application.
     */
    class QueryPMU final
    {
    public:
        static const int64_t num_cores;

    public:
        /**
         * @brief Gets PMU detail information based on the pmu_id value.
         * @return pfm_pmu_info_t
         *
         * @param pmu_id The PMU ID to retrieve information for. @see pfm_pmu_t enum
         * @see libpfm.h pfm_pmu_t for pmu_ids, list_avail_events()
         */
        static pfm_pmu_info_t pmu_info(int32_t pmu_id);

        /**
         * @brief Provides comprehensive information about the Default PMU Architecture.
         *
         * @return pfm_pmu_info_t
         */
        static pfm_pmu_info_t default_pmu_info();

        /**
         * @brief Lists all available events of the pmu_id.
         *
         * @param pmu_id The PMU ID to list events for.
         * @see libpfm.h pfm_pmu_t for pmu_ids
         */
        static void list_avail_events(int32_t pmu_id);

        /**
         * @brief Get the event detail object.
         *
         * @param pmu_id The PMU ID to get event details for.
         * @param event_code The code of the event.
         * @return pfm_event_info_t
         * @see list_avail_events()
         */
        static pfm_event_info_t event_detail(int32_t pmu_id, uint32_t event_code);

        /**
         * @brief List available PMUs on the system with respective details.
         *
         * @see get_avail_pmus()
         */
        static void list_avail_pmus();

        /**
         * @brief Get detected PMU IDs on the system.
         *
         * @return std::vector<int32_t> containing PMU IDs
         * @see  libpfm.h pfm_pmu_t, pmu_info(), list_avail_events()
         */
        static std::vector<int32_t> avail_pmu_ids();

        /**
         * @brief Destroy libpfm4 library.
         *
         * @see init()
         */
        static void destroy();

    private:
        QueryPMU() = delete;
        ~QueryPMU() = delete;

        /**
         * @brief Initializes libpfm4 library.
         *
         * @see destroy()
         */
        static void init();

    private:
        static bool is_active;
        static pfm_pmu_info_t default_architectural_pmu;
    };


    std::ostream &operator<<(std::ostream &out, const pfm_pmu_info_t &pmu_info);
    std::ostream &operator<<(std::ostream &out, const pfm_event_info_t &event_info);
} // namespace optkit::core
 