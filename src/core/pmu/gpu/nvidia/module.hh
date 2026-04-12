#pragma once

#include "core/pmu/gpu/nvidia/block_profiler.hh"

#define OPTKIT_GPU_EVENTS(block_name)                                        \
    optkit::pmu::gpu::nvidia::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) \
    {                                                                        \
        optkit::ProfilerConfig(                                              \
            block_name,                                                      \
            "nvidia_gpu_pmu",                                                \
            true,                                                            \
            false,                                                           \
            optkit::Query::create_folder,                                    \
            !optkit::Query::create_folder),                                  \
            optkit::metrics::performance::gpu_metrics::all_metrics()         \
    }

#define OPTKIT_GPU_EVENTS(block_name, metrics)                               \
    optkit::pmu::gpu::nvidia::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) \
    {                                                                        \
        optkit::ProfilerConfig(                                              \
            block_name,                                                      \
            "nvidia_gpu_pmu",                                                \
            true,                                                            \
            false,                                                           \
            optkit::Query::create_folder,                                    \
            !optkit::Query::create_folder),                                  \
            metrics                                                          \
    }