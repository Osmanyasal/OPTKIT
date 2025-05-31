#pragma once

#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"


#define OPTKIT_PERFORMANCE_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::perf::BlockProfiler variable_name { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_PERFORMANCE_BLOCK_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::perf::BlockGroupProfiler variable_name { block_name, event_name, __VA_ARGS__ }
