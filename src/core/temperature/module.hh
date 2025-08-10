#pragma once

#include "utils/utils.hh"
#include "core/temperature/temperature_profiler.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_TEMPERATURE_MACROS_ENABLED

#define OPTKIT_CPU_TEMPERATURE_EVENTS(block_name, metric_builder, ...) \
    optkit::core::temperature::TemperatureProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, metric_builder, __VA_ARGS__ }

#else
#define OPTKIT_CPU_TEMPERATURE_EVENTS(block_name, metric_builder, ...)
#endif