#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include "utils/metric_builder.hh"
#include "core/metrics/energy/core_metrics.hh"
#include "core/metrics/energy/gpu/core_events.hh"

namespace optkit::metrics::energy
{
    /**
     * @brief Dummy class to indicate CPU energy metrics
     *
     */
    class GPUImpl
    {
    };

    /**
     * @brief energy-related performance metrics.
     *
     * Focus on actionable, interpretable metrics that provide insights into:
     *
     * Template is there for the convention (@see pmu_metrics.hh), but not used.
     */
    template <>
    class CoreMetrics<GPUImpl>
    {
    public:
        static const std::vector<std::string> &get_all_metrics()
        {
            static const std::vector<std::string> names = {
                "k_edp",
                "watt_hour",
            };
            return names;
        }
        // Fetch a metric by its method name (e.g., "l2_mpki").
        // Returns a const reference to a static MetricBuilder.
        static const MetricBuilder<double> &get_metric(const std::string &metric_name)
        {
            if (metric_name == "k_edp")
                return k_edp();
            if (metric_name == "watt_hour")
                return watt_hour();
            static const MetricBuilder<double> empty{};
            return empty;
        }
        static const optkit::metrics::MetricBuilder<double> &all_domains()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(gpu::CoreEvents::GPU), {0x0})
                    .add(to_string(gpu::CoreEvents::MEMORY), {0x0});
            }();
            return mb;
        }
        static const optkit::metrics::MetricBuilder<double> &k_edp()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(gpu::CoreEvents::GPU), {0x0})
                    .build("kilo_edp_total", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double gpu = get_event_count(results, to_string(gpu::CoreEvents::GPU));
                               double memory = get_event_count(results, to_string(gpu::CoreEvents::MEMORY));
                               return (duration_sec * (gpu + memory)) / 1000; // K-EDP calculation
                           })
                    .build("kilo_edp_gpu", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double gpu = get_event_count(results, to_string(gpu::CoreEvents::GPU));
                               return (duration_sec * gpu) / 1000; // K-EDP calculation
                           })
                    .build("kilo_edp_memory", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double memory = get_event_count(results, to_string(gpu::CoreEvents::MEMORY));
                               return (duration_sec * memory) / 1000; // K-EDP calculation
                           });
            }();
            return mb;
        }

        static const optkit::metrics::MetricBuilder<double> &watt_hour()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(gpu::CoreEvents::GPU), {0x0})
                    .build("watt_hour", [](const std::unordered_map<std::string, double> &results)
                           {
                               double gpu = get_event_count(results, to_string(gpu::CoreEvents::GPU));
                               double memory = get_event_count(results, to_string(gpu::CoreEvents::MEMORY));
                               return (gpu + memory) / 3600; // Watt-hour calculation = Joules / 3600
                           });
            }();
            return mb;
        }

        static const MetricBuilder<double> &all_metrics()
        {
            static const MetricBuilder<double> mb = []
            {
                MetricBuilder<double> mb{};
                mb.add(k_edp());
                mb.add(watt_hour());
                return mb;
            }();
            return mb;
        }

    private:
        CoreMetrics()
        {
        }
        ~CoreMetrics() {}
    };
}; // namespace optkit::metrics::energy::cpu