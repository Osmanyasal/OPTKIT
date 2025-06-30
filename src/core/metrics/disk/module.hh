#pragma once

#include "core/metrics/disk/core_events.hh"
#include "core/metrics/disk/core_metrics.hh"

namespace optkit::core::metrics::disk
{
    using core_metrics = CoreMetrics<void>;
    using core_events = CoreEvents;
}
using optkit::core::metrics::disk::operator<<; // make available to global namespace