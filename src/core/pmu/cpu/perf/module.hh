#pragma once

#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"


#define OPTKIT_CPU_EVENTS(block_name, event_name, ...) \
    optkit::core::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var,__LINE__) { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_CPU_BLOCK_EVENTS(block_name, event_name, ...) \
    optkit::core::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var,__LINE__) { block_name, event_name, __VA_ARGS__ }