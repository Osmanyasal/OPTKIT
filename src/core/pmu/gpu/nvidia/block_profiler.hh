#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include <iostream>
#include <vector>
#include <memory>
#include "core/pmu/cpu/perf/profiler_config.hh"

#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "utils/cupti_utils.hh"
#include "core/pmu/gpu/nvidia/gpm_sampler.hh"

namespace optkit::pmu::gpu::nvidia
{
    /**
     * @brief The BlockProfiler class utilizes the RAII technique to initiate and conclude profiling for specific metrics.
     *        Profiling commences upon instantiation and persists until the current scope is exited.
     *        Supports both CUPTI Activity API profiling and NVML GPM metric sampling via a background thread.
     */
    class BlockProfiler : public BaseProfiler<std::vector<double>, double>
    {
    public:
        BlockProfiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb = {},
                       uint32_t gpm_sample_period_us = 1000000);    // 1 sec
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
         * @return std::vector<double> contains each raw_events' pmu data.
         */
        virtual std::vector<double> read() override;

        virtual std::unordered_map<std::string, double> aggregate() override;

#if !OPTKIT_TESTING // if not testing (in prod) then make those private, in testin make those public
    private:
#endif
        virtual void on_sample_stored(const std::pair<double, std::vector<double>> &sample) override;
        void flush_compacted_samples();

        const ProfilerConfig profiler_config;
        const optkit::metrics::MetricBuilder<double> metric_builder;
        std::vector<std::pair<std::string, double>> metric_results;
        std::vector<std::pair<std::string, std::string>> detail_event_results;
        std::unordered_map<std::string, double> compacted_event_counts;
        double compacted_duration_ms{0.0};
        double buffered_duration_ms{0.0};

        // GPM sampling (self-contained in GpmSampler)
        std::unique_ptr<GpmSampler> gpm_sampler_;
        std::vector<std::string> gpm_metric_names_;
    };

} // namespace optkit::pmu::gpu::nvidia

#endif