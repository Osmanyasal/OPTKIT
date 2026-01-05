#pragma once

#include <iostream>
#include <vector>
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "utils/cupti_utils.hh"

namespace optkit::pmu::gpu::amd
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
     *
     * @note In what order the event names and codes are added is IMPORTANT! it is read as it is added.
     *       If you add event1, event2, event3 then the read buffer will contain the values in the same order.
     *       Given the reason, we used vectors and pairs to store the data in metric Builder.
     *
     * @note Data type is chosen as uint64_t to match the syscall returns. Architectures usually have 48bits or 64 bits wide pmu counters.
     *       uint64_t is used to comprehend all of them. https://www.man7.org/linux/man-pages/man2/perf_event_open.2.html#EXAMPLES
     */
    class BlockProfiler : public BaseProfiler<std::vector<uint64_t>, uint64_t>
    {
    public:
        BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<uint64_t> &mb);
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

        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

#if !OPTKIT_TESTING // if not testing (in prod) then make those private, in testin make those public
    private:
#endif
        /**
         * @brief fd_list holds pmu events being monitor by this BlockProfiler Object.
         * when created the same file description must be registered global fd_stack
         */
        std::vector<int32_t> fd_list;
        const ProfilerConfig profiler_config;
        const optkit::metrics::MetricBuilder<uint64_t> metric_builder;
        std::vector<std::pair<std::string, double>> metric_results;
    };

} // namespace optkit::pmu::gpu::amd

#endif