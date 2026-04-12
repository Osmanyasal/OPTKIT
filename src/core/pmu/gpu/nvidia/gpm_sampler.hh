#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_NVML

#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstdint>
#include <nvml.h>
#include "core/metrics/performance/gpu/core_events.hh"

namespace optkit::pmu::gpu::nvidia
{
    /**
     * @brief Self-contained GPM sampling class.
     *
     *        Manages a background thread that periodically takes two nvmlGpmSample snapshots
     *        and computes metrics between them.  Call start() to begin sampling and stop() to
     *        join the thread and retrieve the results.
     */
    class GpmSampler
    {
    public:
        /**
         * @brief Construct a GpmSampler for the given metrics.
         *
         * @param device      Already-initialised NVML device handle (from gpu::Query::get_nvml_device).
         * @param metric_ids   NVML GPM metric IDs to sample.
         * @param metric_names Human-readable names matching each metric_id (same order).
         * @param sample_period_us  Microseconds between two successive nvmlGpmSampleGet calls, 1 sec by default.
         */
        GpmSampler(nvmlDevice_t device,
                   const std::vector<nvmlGpmMetricId_t> &metric_ids,
                   const std::vector<std::string> &metric_names,
                   uint32_t sample_period_us = 1000000);

        ~GpmSampler();

        /** Start the background sampling thread.  Returns true on success. */
        bool start();

        /** Signal the thread to stop and join it. Safe to call multiple times. */
        void stop();

        /** Whether the sampler is currently running. */
        bool is_running() const { return running_.load(); }

        /** Whether GPM was successfully initialised on this device. */
        bool is_enabled() const { return enabled_; }

        /** Number of collected samples so far (thread-safe). */
        size_t sample_count() const;

        /**
         * @brief Compute per-metric averages across all collected samples.
         *
         * @return Map of metric_name -> average value.  Empty if no samples collected.
         */
        std::unordered_map<std::string, double> average_results() const;

        std::vector<std::unordered_map<std::string, double>> all_samples() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return samples_;
        }

    private:
        void sample_loop_();

        std::vector<nvmlGpmMetricId_t> metric_ids_;
        std::vector<std::string> metric_names_;
        uint32_t sample_period_us_;

        nvmlDevice_t device_{};
        bool enabled_{false};

        std::thread thread_;
        std::atomic<bool> running_{false};
        mutable std::mutex mutex_;
        std::vector<std::unordered_map<std::string, double>> samples_;
    };

} // namespace optkit::pmu::gpu::nvidia

#endif
