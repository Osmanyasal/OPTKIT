#pragma once

#include "core/metrics/disk/core_events.hh"
#include "core/metrics/disk/core_metrics.hh"

namespace optkit::metrics::disk
{
    using core_metrics = CoreMetrics<void>;
    using core_events = CoreEvents;
}
using optkit::metrics::disk::operator<<; // make available to global namespace