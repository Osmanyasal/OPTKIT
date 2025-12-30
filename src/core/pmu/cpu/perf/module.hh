#pragma once

#include "core/pmu/cpu/perf/block_group_profiler.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"

// non-sampling macros
#define OPTKIT_CPU_EVENTS(block_name, metric_builder) \
    optkit::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name}, metric_builder }

#define OPTKIT_CPU_EVENTS_REPEAT(block_name, metric_builder, count)                                       \
    optkit::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__){{block_name}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_CPU_GROUP_EVENTS(block_name, metric_builder) \
    optkit::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, false, true}, metric_builder }

#define OPTKIT_CPU_GROUP_EVENTS_REPEAT(block_name, metric_builder, count)                                                   \
    optkit::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, false, true}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// sampling macros
#define OPTKIT_CPU_EVENTS_SAMPLING(block_name, metric_builder) \
    optkit::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, true}, metric_builder }

#define OPTKIT_CPU_EVENTS_REPEAT_SAMPLING(block_name, metric_builder, count)                                    \
    optkit::pmu::cpu::perf::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, true}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_CPU_GROUP_EVENTS_SAMPLING(block_name, metric_builder) \
    optkit::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, true, true}, metric_builder }

#define OPTKIT_CPU_GROUP_EVENTS_REPEAT_SAMPLING(block_name, metric_builder, count)                                         \
    optkit::pmu::cpu::perf::BlockGroupProfiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, true, true}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())
