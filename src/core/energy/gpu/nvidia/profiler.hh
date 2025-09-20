#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "core/gpu_query.hh"

namespace optkit::energy::gpu
{
    class Profiler : public BaseProfiler<std::vector<std::unordered_map<double, double>>, std::unordered_map<double, double>>
    {

    public:
        Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<std::unordered_map<double, double>> &mb = {});
        virtual ~Profiler();

        virtual void enable() override {}  // Already handled by constructor
        virtual void disable() override {} // No-op
        virtual void reset() override {}   // No-op

        // This read saves deltas, meaning, the change from the previous read. (current_val - prev_val)
        // if you need current values, use the read_temperature_sensors() method directly.
        virtual std::vector<std::unordered_map<double, double>> read() override;
        virtual std::string to_json() override;
        virtual std::unordered_map<std::string, std::unordered_map<double, double>> aggregate() override;

    private:
        optkit::metrics::MetricBuilder<std::unordered_map<double, double>> metric_builder;
        std::unordered_map<uint32_t, std::unordered_map<double, double>> last_snapshot; // device-index -> last energy & power
        std::vector<std::pair<std::string, double>> metric_results;
    };
} // namespace optkit::energy::gpu