#pragma once

#include <iostream>
#include <vector>
#include <initializer_list>

#include "core/pmu/cpu/perf/profiler_config.hh"
#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/pmu/cpu/pmu_event_manager.hh"
#include "core/metrics/performance/cpu/core_metrics.hh"
namespace optkit::pmu::cpu::perf
{
    /**
     * @brief The BlockGroupProfiler class utilizes the RAII technique to initiate and conclude profiling for a specific raw event.
     *        Profiling commences upon instantiation and persists until the current scope is exited.
     *
     *        Note that block profiler DOES group raw_events!
     *
     *        for more information about grouping @see https://man7.org/linux/man-pages/man2/perf_event_open.2.html
     *
     * @note In what order the event names and codes are added is IMPORTANT! it is read as it is added.
     *       If you add event1, event2, event3 then the read buffer will contain the values in the same order.
     *       Given the reason, we used vectors and pairs to store the data in metric Builder.
     */
    class BlockGroupProfiler : public BaseProfiler<std::vector<uint64_t>, uint64_t>
    {

    public:
        BlockGroupProfiler(const PerfProfilerConfig &config, const optkit::metrics::MetricBuilder<uint64_t> &mb);
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
         * @brief Reset this block profiler and associated events
         *
         */
        virtual void reset() override;

        /**
         * @brief  converts buffer to json
         *
         */
        virtual std::string to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::vector<uint64_t> read() override;

        int32_t get_group_leader()
        {
            return this->group_leaders.empty() ? -1 : this->group_leaders[0];
        }

        virtual std::unordered_map<std::string, uint64_t> aggregate() override;

#if !OPTKIT_TESTING // if not testing (in prod) then make those private, in testin make those public
    private:
#endif

        bool is_configured;
        std::vector<int32_t> group_leaders;

        const PerfProfilerConfig profiler_config;
        const optkit::metrics::MetricBuilder<uint64_t> metric_builder;
        std::vector<std::pair<std::string, double>> metric_results;
        std::thread sampling_thread;
        std::atomic<bool> is_sampling{false};

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

} // namespace optkit::pmu::cpu::perf

#endif