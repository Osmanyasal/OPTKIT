#pragma once

#include "core/metrics/energy/cpu/core_metrics.hh"

namespace optkit::metrics::energy
{
    using core_metrics = CoreMetrics<void>;
    using core_events = CoreEvents;
}
using optkit::metrics::energy::operator<<; // make available to global namespace