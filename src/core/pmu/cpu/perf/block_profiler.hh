#pragma once

#include <iostream>
#include <vector>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/pmu/cpu/perf/pmu_event_manager.hh"
#include "core/pmu/cpu/perf/pmu_utils.hh"

namespace optkit::core::pmu::cpu::perf
{
    /**
     * @brief The BlockProfiler class utilizes the RAII technique to initiate and conclude profiling for a specific raw event.
     *        Profiling commences upon instantiation and persists until the current scope is exited.
     *
     *        Note that block profiler does NOT group raw_events!
     *
     *        Each raw_event creates seperate file_description (fd) to read data and each raw_event is treated seperately.<br>
     *        In cases where CPU performs multiplexing and since each event treated seperately, you cannot gurantee that <br>
     *        events E1 and E2 will record the same instructions.<br>
     *        for more information about grouping @see https://man7.org/linux/man-pages/man2/perf_event_open.2.html
     */
    class BlockProfiler : public BaseProfiler<std::vector<uint64_t>>
    {
    public:
        BlockProfiler(const char *block_name, const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, bool verbose = true, const PerfProfilerConfig &config = PerfProfilerConfig{true, true, false, 0, -1});
        virtual ~BlockProfiler();
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
         * @brief converts buffer to json
         *
         */
        virtual std::string convert_buffer_to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::vector<uint64_t> read() override;

    public:
        /**
         * @brief fd_list holds pmu events being monitor by this BlockProfiler Object.
         * when created the same file description must be registered global fd_stack
         *
         */
        std::vector<int32_t> fd_list;
        PerfProfilerConfig profiler_config;

    private:
        std::vector<std::pair<uint64_t, std::string>> raw_events;
    };

} // namespace optkit::core::pmu::cpu::perf
