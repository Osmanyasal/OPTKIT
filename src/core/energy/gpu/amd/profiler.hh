#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <thread>
#include <unordered_map>
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "core/gpu_query.hh"

namespace optkit::energy::gpu::amd
{

    /**
     * @brief AMD GPU Energy Profiler
     *
     * This profiler measures the energy consumption of AMD GPUs using the AMD ROCm SMI Library.
     * It periodically samples the power usage of each GPU and aggregates the data to provide insights into energy efficiency and usage patterns.
     * The profiler runs a separate sampling thread that collects power readings at a user-defined frequency.
     * It averages the power consumption over the sampling period and can output the results in JSON format.
     *
     * @note User should call get_read_buffer to get the raw data of power samples taken, which gives eirther instantaneous power or 1 seconds average power consumption. Consult ROCm SMI documentation for more details.
     *
     */
    class Profiler : public BaseProfiler<std::unordered_map<uint32_t, double>, std::unordered_map<uint32_t, double>> // storing device-id, energy in Joules
    {

    public:
        Profiler(const ProfilerConfig &profiler_config, const uint32_t sampling_frequency_sec = 1, const optkit::metrics::MetricBuilder<std::unordered_map<uint32_t, double>> &mb = {});
        virtual ~Profiler();

        virtual void enable() override {}                             // Already handled by constructor
        virtual void disable() override { this->is_enabled = false; } // No-op
        virtual void reset() override {}                              // No-op

        // returns device_id - energy (power * delta_time) values in a vector.
        virtual std::unordered_map<uint32_t, double> read() override;
        virtual std::string to_json() override;
        virtual std::unordered_map<std::string, std::unordered_map<uint32_t, double>> aggregate() override;

    private:
        optkit::metrics::MetricBuilder<std::unordered_map<uint32_t, double>> metric_builder;
        std::unordered_map<uint32_t, double> snapshot; // device-index -> current power drawn in Watts
        std::vector<std::pair<std::string, double>> metric_results;

        uint32_t sampling_frequency_sec;
        uint32_t sampling_counter;
        std::thread sampling_thread;
        std::atomic<bool> is_sampling;
        bool is_enabled;
    };
} // namespace optkit::energy::gpu::amd
