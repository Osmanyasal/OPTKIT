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
#include "core/metrics/energy/cpu/core_events.hh"

namespace optkit::metrics::energy
{
    /**
     * @brief Dummy class to indicate CPU energy metrics
     *
     */
    class CPUImpl
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
    class CoreMetrics<CPUImpl>
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
        static optkit::metrics::MetricBuilder<double> all_domains()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(cpu::CoreEvents::PP0), {0x0})
                    .add(to_string(cpu::CoreEvents::PP1), {0x0})
                    .add(to_string(cpu::CoreEvents::PACKAGE), {0x0})
                    .add(to_string(cpu::CoreEvents::PSYS), {0x0})
                    .add(to_string(cpu::CoreEvents::DRAM), {0x0});
            }();
            return mb;
        }
        static optkit::metrics::MetricBuilder<double> k_edp()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(cpu::CoreEvents::PACKAGE), {0x0})
                    .add(to_string(cpu::CoreEvents::DRAM), {0x0})
                    .build("kilo_edp_edp", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double pkg = get_event_count(results, to_string(cpu::CoreEvents::PACKAGE));
                               double dram = get_event_count(results, to_string(cpu::CoreEvents::DRAM));
                               return (duration_sec * (pkg + dram)) / 1000; // K-EDP calculation
                           })
                    .build("kilo_edp_pkg", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double pkg = get_event_count(results, to_string(cpu::CoreEvents::PACKAGE));
                               return (duration_sec * pkg) / 1000; // K-EDP calculation
                           })
                    .build("kilo_edp_dram", [](const std::unordered_map<std::string, double> &results)
                           {
                               double duration_sec = get_event_count(results, "duration_microsec") / 1.0e6;
                               double dram = get_event_count(results, to_string(cpu::CoreEvents::DRAM));
                               return (duration_sec * dram) / 1000; // K-EDP calculation
                           });
            }();
            return mb;
        }

        static optkit::metrics::MetricBuilder<double> watt_hour()
        {
            static const optkit::metrics::MetricBuilder<double> mb = []
            {
                return MetricBuilder<double>{}
                    .add(to_string(cpu::CoreEvents::PACKAGE), {0x0})
                    .add(to_string(cpu::CoreEvents::DRAM), {0x0})
                    .build("watt_hour", [](const std::unordered_map<std::string, double> &results)
                           {
                               double pkg = get_event_count(results, to_string(cpu::CoreEvents::PACKAGE));
                               double dram = get_event_count(results, to_string(cpu::CoreEvents::DRAM));
                               return (pkg + dram) / 3600; // Watt-hour calculation = Joules / 3600
                           });
            }();
            return mb;
        }

        static optkit::metrics::MetricBuilder<double> all_metrics()
        {
            static const MetricBuilder<double> mb = []
            {
                MetricBuilder<double> mb{};
                mb.add(all_domains());
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