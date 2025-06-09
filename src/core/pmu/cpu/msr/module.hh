#pragma once

#include "core/pmu/cpu/msr/block_group_profiler.hh"
#include "core/pmu/cpu/msr/block_profiler.hh"
#include "core/pmu/cpu/msr/profiler_config.hh"

#define OPTKIT_CPU_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::msr::BlockProfiler variable_name { block_name, event_name, __VA_ARGS__ }

#define OPTKIT_CPU_BLOCK_EVENTS(block_name, event_name, variable_name, ...) \
    optkit::core::pmu::cpu::msr::BlockGroupProfiler variable_name { block_name, event_name, __VA_ARGS__ }