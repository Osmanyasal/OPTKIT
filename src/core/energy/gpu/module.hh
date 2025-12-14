#pragma once

#include "utils/utils.hh"
#include "core/energy/gpu/nvidia/profiler.hh"
#include "core/energy/gpu/amd/profiler.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_ENERGY_MACROS_ENABLED

#define OPTKIT_GPU_ENERGY(block_name)                                                                                                                                                                                                         \
    optkit::energy::gpu::nvidia::Profiler EXPAND_AND_CONCAT(nvidia_var, __LINE__){{block_name, "nvidia_gpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::gpu_metrics::all_metrics()}; \
    optkit::energy::gpu::amd::Profiler EXPAND_AND_CONCAT(amd_var, __LINE__) { {block_name, "amd_gpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::gpu_metrics::all_metrics() }

#define OPTKIT_GPU_ENERGY_WITH_METRICS(block_name, metric_builder)                                                                                                                                       \
    optkit::energy::gpu::nvidia::Profiler EXPAND_AND_CONCAT(nvidia_var, __LINE__){{block_name, "nvidia_gpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    optkit::energy::gpu::amd::Profiler EXPAND_AND_CONCAT(amd_var, __LINE__) { {block_name, "amd_gpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder }

#else
#include "core/energy/gpu/clear.hh"
#endif
