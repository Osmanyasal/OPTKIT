#pragma once

#include "core/pmu/cpu/libpfm4_wrapper.hh"
#include "core/pmu/cpu/pmu_event_manager.hh"
#include "core/pmu/cpu/query_pmu.hh"
#include "utils/deployment/deployment_config.hh"


// Select which way (perf or msr) to macro
#if OPTKIT_CONF_PMU_USE_PERF
    #include "core/pmu/cpu/perf/module.hh"
#elif OPTKIT_CONF_PMU_USE_MSR
    #include "core/pmu/cpu/msr/module.hh"
#else
    #define OPTKIT_CPU_EVENTS(block_name, event_name, variable_name, ...)
    #define OPTKIT_CPU_BLOCK_EVENTS(block_name, event_name, variable_name, ...)
#endif

using optkit::core::pmu::cpu::operator<<; // make available to global namespace