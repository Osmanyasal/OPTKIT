#pragma once

#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT

#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"

#if OPTKIT_CONF_PMU_MACROS_ENABLED

#define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::perf::BlockProfiler variable_name { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::perf::BlockGroupProfiler variable_name { block_name, event_name, __VA_ARGS__ }
#else

#define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...)
#define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...)

#endif

using optkit::core::pmu::cpu::operator<<; // make available to global namespace

#else // not OPTKIT_CONF_PMU_MACROS_ENABLED

#define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...)
#define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...)

#endif