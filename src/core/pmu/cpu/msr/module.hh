#pragma once

#include "core/pmu/cpu/msr/block_group_profiler.hh"
#include "core/pmu/cpu/msr/block_profiler.hh"
#include "core/pmu/cpu/msr/profiler_config.hh"

#define OPTKIT_CPU_EVENTS(block_name, event_name, ...) \
    optkit::pmu::cpu::msr::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_CPU_BLOCK_EVENTS(block_name, event_name, ...) \
    optkit::pmu::cpu::msr::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_CPU_GROUP_EVENTS(block_name, metric_builder, ...) \
    optkit::pmu::cpu::msr::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, metric_builder, __VA_ARGS__ }

#define OPTKIT_CPU_GROUP_EVENTS_REPEAT(block_name, metric_builder, count, ...)                                           \
    optkit::pmu::cpu::msr::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__){block_name, metric_builder, __VA_ARGS__}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())
