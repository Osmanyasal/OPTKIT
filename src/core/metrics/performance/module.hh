#pragma once

#include "core/metrics/performance/cpu/core_metrics.hh"
#include "utils/deployment/deployment_config.hh"

#if OPTKIT_ENV_CPU_INTEL
#include "core/metrics/performance/cpu/intel/core_metrics.hh"
#include "core/metrics/performance/cpu/intel/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = CoreMetrics<IntelMetricsImpl>;
    using cpu_mapper = intel::EventMapper;
    using cpu_events = CoreEvents;
    using cpu_native_events = intel::NativeEvents;
}
#elif OPTKIT_ENV_CPU_AMD
#include "core/metrics/performance/cpu/amd/core_metrics.hh"
#include "core/metrics/performance/cpu/amd/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = CoreMetrics<AMDMetricsImpl>;
    using cpu_mapper = amd::EventMapper;
    using cpu_events = CoreEvents;
    using cpu_native_events = amd::NativeEvents;
}
#elif OPTKIT_ENV_CPU_ARM
#include "core/metrics/performance/cpu/arm/core_metrics.hh"
#include "core/metrics/performance/cpu/arm/event_mapper.hh"
namespace optkit::metrics::performance
{
    using cpu_metrics = CoreMetrics<ARMMetricsImpl>;
    using cpu_mapper = arm::EventMapper;
    using cpu_events = CoreEvents;
    using cpu_native_events = arm::NativeEvents;
}
#elif OPTKIT_ENV_CPU_RISCV
#elif OPTKIT_ENV_CPU_MIPS
#elif OPTKIT_ENV_CPU_POWERPC
#else
#endif

using optkit::metrics::performance::operator<<; // make available to global namespace