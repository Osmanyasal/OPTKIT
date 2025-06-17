#pragma once

#include <iostream>
#include <vector>
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/pmu/cpu/pmu_event_manager.hh"
#include "core/pmu/cpu/pmu_utils.hh"
#include "core/metrics/cpu/core_metrics.hh"

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
        // BlockProfiler(const char *block_name, const std::vector<std::pair<std::string, uint64_t>> &raw_events, bool verbose = true, const PerfProfilerConfig &config = PerfProfilerConfig{true, true, false, 0, -1});
        BlockProfiler(const char *block_name, const core::metrics::cpu::MetricBuilder &mb, bool verbose = true, const PerfProfilerConfig &config = PerfProfilerConfig{true, true, false, 0, -1});
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
         * @brief Reset this block profiler and associated events
         *
         */
        virtual void reset() override;

        /**
         * @brief converts buffer to json
         *
         */
        virtual std::string to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::vector<uint64_t> read() override;

        /**
         * @brief Aggregates all collected data from the read buffer.
         *
         * If multiple `read_and_save()` calls are made while monitoring events (e.g., x, y, z),
         * the `read_buffer` may contain several entries, each holding a duration and a list of event values.
         *
         * This method processes the entire buffer, summing up the total duration and combining
         * all recorded event-value pairs into a single list.
         *
         * You should call MetricBuffer.calculate(...) to see the results
         *
         * @return A pair consisting of:
         *         - total duration (in seconds),
         *         - a vector of <event name, value> pairs.
         */
        virtual std::pair<double, std::vector<std::pair<std::string, uint64_t>>> aggregate() override;

    public:
        /**
         * @brief fd_list holds pmu events being monitor by this BlockProfiler Object.
         * when created the same file description must be registered global fd_stack
         *
         */
        std::vector<int32_t> fd_list;
        PerfProfilerConfig profiler_config;

    private:
        std::vector<std::pair<std::string, uint64_t>> results;
        std::vector<std::pair<std::string, double>> metric_results;
        core::metrics::cpu::MetricBuilder metric_builder;
    };

} // namespace optkit::core::pmu::cpu::perf

#endif