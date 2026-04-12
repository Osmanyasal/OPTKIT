#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "utils/metric_builder.hh"
namespace optkit::metrics::performance::gpu
{
    /**
     * @class Metrics
    * @brief Interface for accessing GPU performance metrics. Each method returns a MetricBuilder<double> instance that defines a metric and its associated events.
     *
     * The actual implementation should reside in GPU vendor-specific modules, not in this interface. These methods are not purely abstract because a metric might be defined generically but not supported on a particular GPU. In such cases, the method may return an empty list.
     *
     * The metric formulas described in this documentation are pseudocode representations. The actual event names used in implementations may differ from those shown here.
     *
    * @note It is recommended to use the MetricBuilder<double> class to construct metrics, as it offers a flexible and architecture-agnostic way to define and compute metrics.
    * @note For performance reasons, implementations return references to `static const` MetricBuilder<double> instances, usually defined through static lambdas.
     * An example is shown below: This way, the metric is only built once and can be reused without reinitialization.
     *
    * static const MetricBuilder<double>& MyMetric()
     * {
    *     static const MetricBuilder<double> metric = [] {
    *         return MetricBuilder<double>{}
     *             .add("event_name", event_id)
     *             .build("MyMetric",
    *                    [](const std::unordered_map<std::string, double>& counts) -> double {
     *                        // Compute the metric value from event counts
     *                        return result;
     *                    });
     *     }();
     *     return metric;
     * }
     */

    template <typename T>
    class CoreMetrics
    {
    public:
        static MetricBuilder<double> graphics_util() { return {}; }      
        static MetricBuilder<double> sm_util() { return {}; }      
        static MetricBuilder<double> sm_occupancy() { return {}; }      
        static MetricBuilder<double> integer_util() { return {}; } 
        static MetricBuilder<double> tensor_util() { return {}; } 
        static MetricBuilder<double> dfma_util() { return {}; } // Double Fused Multiply-Add
        static MetricBuilder<double> hmma_util() { return {}; } // Half-precision Matrix Multiply-Accumulate
        static MetricBuilder<double> imma_util() { return {}; } // Integer Matrix Multiply-Accumulate
        static MetricBuilder<double> dram_bw_util() { return {}; } // DRAM Bandwidth Utilization
        static MetricBuilder<double> fp64_util() { return {}; } // Double-precision Floating Point Utilization
        static MetricBuilder<double> fp32_util() { return {}; } // Single-precision Floating Point Utilization
        static MetricBuilder<double> fp16_util() { return {}; } // Half-precision Floating Point Utilization

        static MetricBuilder<double> all_metrics() { return {}; }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::metrics::performance