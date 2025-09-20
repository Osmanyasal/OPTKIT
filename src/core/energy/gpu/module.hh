#pragma once

#include "utils/utils.hh"
#include "core/temperature/hwmon/profiler.hh"
#include "core/temperature/gpu/profiler.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_ENERGY_MACROS_ENABLED

#define OPTKIT_GPU_ENERGY_EVENTS(block_name, metric_builder, ...) \
    optkit::energy::gpu::Profiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, "gpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder, __VA_ARGS__ }

#else
#define OPTKIT_GPU_ENERGY_EVENTS(block_name, metric_builder, ...)
#endif