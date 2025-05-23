#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/libpfm4_wrapper.hh"
#include "core/pmu/cpu/perf/pmu_event_manager.hh"
#include "core/pmu/cpu/perf/query_pmu.hh"

#if defined(CONF__PMU__MACROS__ENABLED) && CONF__PMU__MACROS__ENABLED == 1
    #define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...) \
        optkit::core::pmu::cpu::perf::BlockProfiler variable_name { block_name, event_name, __VA_ARGS__ }

    #define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...) \
        optkit::core::pmu::cpu::perf::BlockGroupProfiler variable_name { block_name, event_name, __VA_ARGS__ }
#else

    #define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...)
    #define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...)

#endif

using optkit::core::pmu::cpu::perf::operator<<; // make available to global namespace