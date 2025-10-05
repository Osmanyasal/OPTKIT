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

namespace optkit::metrics::energy
{
    /**
     * @brief Core energy-related performance metrics.
     *
     * @tparam T
     */
    template <typename T>
    class CoreMetrics
    {
    public:
        static optkit::metrics::MetricBuilder<T> k_edp() { return {}; }
        static optkit::metrics::MetricBuilder<T> watt_hour() { return {}; }
        static optkit::metrics::MetricBuilder<T> all_metrics() { return {}; }

    private:
        CoreMetrics()
        {
        }
        ~CoreMetrics() {}
    };
}; // namespace optkit::metrics::energy