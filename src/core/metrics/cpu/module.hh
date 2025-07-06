#pragma once

#include "core/metrics/cpu/core_metrics.hh"
#include "utils/deployment/deployment_config.hh"

#if OPTKIT_CONF_PMU_MACROS_ENABLED

#if OPTKIT_ENV_CPU_INTEL
#include "core/metrics/cpu/intel/core_metrics.hh"
#include "core/metrics/cpu/intel/event_mapper.hh"
namespace optkit::core::metrics::cpu
{
    using core_metrics = CoreMetrics<IntelMetricsImpl>;
    using event_mapper = intel::EventMapper;
    using core_events = CoreEvents;
    using native_events = intel::NativeEvents;
}
#elif OPTKIT_ENV_CPU_AMD
#include "core/metrics/cpu/amd/core_metrics.hh"
#include "core/metrics/cpu/amd/event_mapper.hh"
namespace optkit::core::metrics::cpu
{
    using core_metrics = CoreMetrics<AMDMetricsImpl>;
    using event_mapper = amd::EventMapper;
    using core_events = CoreEvents;
    using native_events = amd::NativeEvents;
}
#elif OPTKIT_ENV_CPU_ARM
#include "core/metrics/cpu/arm/core_metrics.hh"
#include "core/metrics/cpu/arm/event_mapper.hh"
namespace optkit::core::metrics::cpu
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

#define OPTKIT_TMA_ANALYSIS(block_name, variable_name, TMA_RECEPIE)                                  \
    std::string variable_name##_event_name = optkit::core::metrics::to_string(TMA_RECEPIE);          \
    optkit::core::metrics::TMAnalysis variable_name(block_name, variable_name##_event_name.c_str()); \
    variable_name.begin_monitoring(TMA_RECEPIE)

#else

#define OPTKIT_TMA_ANALYSIS(...)

#endif

using optkit::core::metrics::cpu::operator<<; // make available to global namespace