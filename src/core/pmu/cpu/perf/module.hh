#pragma once

#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"

#define OPTKIT_CPU_EVENTS(block_name, metric_builder, ...) \
    optkit::core::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, metric_builder, __VA_ARGS__ }

#define OPTKIT_CPU_EVENTS_REPEAT(block_name, metric_builder, count, ...)                                                   \
    optkit::core::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__){block_name, metric_builder, __VA_ARGS__}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_CPU_GROUP_EVENTS(block_name, metric_builder, ...) \
    optkit::core::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, metric_builder, __VA_ARGS__ }

#define OPTKIT_CPU_GROUP_EVENTS_REPEAT(block_name, metric_builder, count, ...)                                                   \
    optkit::core::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__){block_name, metric_builder, __VA_ARGS__}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())
