#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include <iostream>
#include <vector>
#include "core/pmu/cpu/perf/profiler_config.hh"

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "utils/cupti_utils.hh"

namespace optkit::pmu::gpu::nvidia
{
    /**
     * @brief The BlockProfiler class utilizes the RAII technique to initiate and conclude profiling for specific metrics.
     *        Profiling commences upon instantiation and persists until the current scope is exited.
     */
    class BlockProfiler : public BaseProfiler<std::vector<std::string>, std::string>
    {
    public:
        BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<std::string> &mb = {});
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
        virtual std::vector<std::string> read() override;

        virtual std::unordered_map<std::string, std::string> aggregate() override;

#if !OPTKIT_TESTING // if not testing (in prod) then make those private, in testin make those public
    private:
#endif
        const ProfilerConfig profiler_config;
        const optkit::metrics::MetricBuilder<std::string> metric_builder;
        std::vector<std::pair<std::string, double>> metric_results;
    };

} // namespace optkit::pmu::gpu::nvidia

#endif