#pragma once

#include "utils/utils.hh"
#include "core/temperature/hwmon/profiler.hh"
#include "core/temperature/gpu/profiler.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_TEMPERATURE_MACROS_ENABLED

#define OPTKIT_HWMON_TEMPERATURE(block_name)                                                                   \
    optkit::temperature::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__)                                      \
    {                                                                                                          \
        {                                                                                                      \
            block_name, "hwmon_temperature", true, optkit::Query::create_folder, !optkit::Query::create_folder \
        }                                                                                                      \
    }

#define OPTKIT_GPU_TEMPERATURE(block_name)                                                                   \
    optkit::temperature::gpu::Profiler EXPAND_AND_CONCAT(var, __LINE__)                                      \
    {                                                                                                        \
        {                                                                                                    \
            block_name, "gpu_temperature", true, optkit::Query::create_folder, !optkit::Query::create_folder \
        }                                                                                                    \
    }
#else
#define OPTKIT_HWMON_TEMPERATURE(block_name)
#define OPTKIT_GPU_TEMPERATURE(block_name)
#endif