#pragma once

#include "core/metrics/cpu/core_metrics.hh"
#include "utils/deployment/deployment_config.hh"

#if OPTKIT_ENV_CPU_INTEL
#include "core/metrics/cpu/intel/core_metrics.hh"
#include "core/metrics/cpu/intel/event_mapper.hh"
namespace optkit::metrics::cpu
{
    using core_metrics = CoreMetrics<IntelMetricsImpl>;
    using event_mapper = intel::EventMapper;
    using core_events = CoreEvents;
    using native_events = intel::NativeEvents;
}
#elif OPTKIT_ENV_CPU_AMD
#include "core/metrics/cpu/amd/core_metrics.hh"
#include "core/metrics/cpu/amd/event_mapper.hh"
namespace optkit::metrics::cpu
{
    using core_metrics = CoreMetrics<AMDMetricsImpl>;
    using event_mapper = amd::EventMapper;
    using core_events = CoreEvents;
    using native_events = amd::NativeEvents;
}
#elif OPTKIT_ENV_CPU_ARM
#include "core/metrics/cpu/arm/core_metrics.hh"
#include "core/metrics/cpu/arm/event_mapper.hh"
namespace optkit::metrics::cpu
{
    using core_metrics = CoreMetrics<ARMMetricsImpl>;
    using event_mapper = arm::EventMapper;
    using core_events = CoreEvents;
    using native_events = arm::NativeEvents;
}
#elif OPTKIT_ENV_CPU_RISCV
#elif OPTKIT_ENV_CPU_MIPS
#elif OPTKIT_ENV_CPU_POWERPC
#else
#endif

using optkit::metrics::cpu::operator<<; // make available to global namespace