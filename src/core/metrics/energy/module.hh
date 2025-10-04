#pragma once

#include "core/metrics/energy/cpu/core_metrics.hh"

namespace optkit::metrics::energy
{
    using cpu_metrics = CoreMetrics<CPUImpl>;
    using cpu_events = CoreEvents;
}
using optkit::metrics::energy::operator<<; // make available to global namespace