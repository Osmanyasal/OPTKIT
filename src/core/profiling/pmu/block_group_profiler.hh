#pragma once

#include <iostream>
#include <initializer_list>
#include <vector>
#include "utils/utils.hh"
#include "core/profiling/pmu/pmu_event_manager.hh"
#include "core/profiling/base_profiler.hh"
#include "core/profiling/pmu/pmu_utils.hh"

namespace optkit::core::pmu
{
    /**
     * @brief The BlockGroupProfiler class utilizes the RAII technique to initiate and conclude profiling for a specific raw event.
     *        Profiling commences upon instantiation and persists until the current scope is exited.
     *
     *        Note that block profiler DOES group raw_events!
     *
     *        for more information about grouping @see https://man7.org/linux/man-pages/man2/perf_event_open.2.html
     */
    class BlockGroupProfiler : public BaseProfiler<std::vector<uint64_t>>
    {

    public:
        BlockGroupProfiler(const char *block_name, const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, bool verbose = true, const ProfilerConfig &config = ProfilerConfig{true, true, true,0,-1});
        virtual ~BlockGroupProfiler();
        /**
         * @brief Disables this block profiler and associated events
         *
         */
        virtual void disable() override;

        /**
         * @brief Enables this block profiler and associated events
         *
         */
        virtual void enable() override;

        /**
         * @brief  converts buffer to json
         *
         */
        virtual std::string convert_buffer_to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::vector<uint64_t> read_val() override;

        int32_t get_group_leader(){
            return this->group_leader;
        }

    private:
        ProfilerConfig profiler_config;
        int32_t group_leader;
        bool is_active;
        std::vector<std::pair<uint64_t, std::string>> raw_events;

        struct read_format
        {
            uint64_t nr;
            struct
            {
                uint64_t value;
                uint64_t id;
            } values[];
        };
    };

} // namespace optkit::core::pmu
