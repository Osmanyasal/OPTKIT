#pragma once

#include "core/metrics/energy/cpu/core_metrics.hh"
#include "core/metrics/energy/gpu/core_metrics.hh"

namespace optkit::metrics::energy
{
    using cpu_metrics = CoreMetrics<CPUImpl>;
    using cpu_core_events = cpu::CoreEvents;

    using gpu_metrics = CoreMetrics<GPUImpl>;
    using gpu_core_events = gpu::CoreEvents;
}