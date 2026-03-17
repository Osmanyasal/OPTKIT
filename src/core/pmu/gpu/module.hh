#pragma once

#include "utils/environment_config.hh"

#if OPTKIT_ENV_LIB_NVML && defined(__has_include) && __has_include(<cuda.h>) && __has_include(<cuda_runtime_api.h>) && __has_include(<cupti.h>)
#include "core/pmu/gpu/nvidia/module.hh"
#elif (OPTKIT_ENV_LIB_AMDSMI || OPTKIT_ENV_LIB_ROCM_SMI) && defined(__has_include) && __has_include("core/pmu/gpu/amd/module.hh")
#include "core/pmu/gpu/amd/module.hh"
#else
#include "core/pmu/gpu/clear.hh"
#endif
