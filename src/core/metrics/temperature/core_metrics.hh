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
#include "core/metrics/temperature/core_events.hh"

namespace optkit::metrics::temperature
{
    /**
     * @brief Temperature-related performance metrics.
     *
     * Focus on actionable, interpretable metrics that provide insights into:
     */
    template <typename T>
    class CoreMetrics
    {
    public:
        static optkit::metrics::MetricBuilder<double> AllMetrics()
        {
            return optkit::metrics::MetricBuilder<double>{}
                .add(to_string(CoreEvents::CPU), {0x0})
                .add(to_string(CoreEvents::STORAGE), {0x0})
                .add(to_string(CoreEvents::CPUGPU), {0x0})
                .add(to_string(CoreEvents::GPU), {0x0})
                .add(to_string(CoreEvents::NETWORK), {0x0})
                .add(to_string(CoreEvents::MOTHERBOARD), {0x0})
                .add(to_string(CoreEvents::MEMORY), {0x0})
                .add(to_string(CoreEvents::USB), {0x0})
                .add(to_string(CoreEvents::AIO_COOLANT), {0x0})
                .add(to_string(CoreEvents::PSU), {0x0})
                .add(to_string(CoreEvents::BMC), {0x0})
                .add(to_string(CoreEvents::FAN), {0x0})
                .add(to_string(CoreEvents::BATTERY), {0x0})
                .add(to_string(CoreEvents::ACPI_THERMAL), {0x0})
                .add(to_string(CoreEvents::GENERIC_I2C), {0x0})
                .add(to_string(CoreEvents::SOC_PLATFORM), {0x0})
                .add(to_string(CoreEvents::ACCELERATOR), {0x0});
        }

    private:
        CoreMetrics() {}
        ~CoreMetrics() {}
    };
} // namespace optkit::metrics::disk