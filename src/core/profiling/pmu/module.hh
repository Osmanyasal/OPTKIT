#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/profiling/pmu/block_group_profiler.hh"
#include "core/profiling/pmu/block_profiler.hh"
#include "core/profiling/pmu/libpfm4_wrapper.hh"
#include "core/profiling/pmu/pmu_event_manager.hh"
#include "utils/profiling/gantt_instrumentor.hh"
#include "core/profiling/pmu/query_pmu.hh"

#if defined(CONF__PMU__MACROS__ENABLED) && CONF__PMU__MACROS__ENABLED == 1
    #define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...) \
        OPTKIT_CORE_GANTT_PROFILE_SCOPE(block_name);                              \
        optkit::core::pmu::BlockProfiler variable_name { block_name, event_name, __VA_ARGS__ }

    #define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...) \
        OPTKIT_CORE_GANTT_PROFILE_SCOPE(block_name);                                    \
        optkit::core::pmu::BlockGroupProfiler variable_name { block_name, event_name, __VA_ARGS__ }
#else

    #define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...)
    #define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...)

#endif

using optkit::core::pmu::operator<<; // make available to global namespace