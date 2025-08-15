#pragma once

#include "core/metrics/temperature/core_events.hh"
#include "core/metrics/temperature/core_metrics.hh"

namespace optkit::core::metrics::temperature
{
    using core_metrics = CoreMetrics<void>;
    using core_events = CoreEvents;
}
using optkit::core::metrics::temperature::operator<<; // make available to global namespace