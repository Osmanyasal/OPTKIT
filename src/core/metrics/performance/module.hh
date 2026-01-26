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
    inline const cpu_native_events &cpu_get_native_events()
    {
        static const cpu_native_events events = intel::get_native_events();
        return events;
    }
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
    inline const cpu_native_events &cpu_get_native_events()
    {
        static const cpu_native_events events = intel::get_native_events();
        return events;
    }
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
    inline const cpu_native_events &cpu_get_native_events()
    {
        static const cpu_native_events events = intel::get_native_events();
        return events;
    }
}
#elif OPTKIT_ENV_CPU_RISCV
#elif OPTKIT_ENV_CPU_MIPS
#elif OPTKIT_ENV_CPU_POWERPC
#else
#endif

using optkit::metrics::performance::operator<<; // make available to global namespace