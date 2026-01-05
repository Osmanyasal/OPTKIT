#pragma once

#include "utils/environment_config.hh"

#ifdef OPTKIT_ENV_LIB_NVML
#include "core/pmu/gpu/nvidia/module.hh"
#elif OPTKIT_ENV_LIB_AMDSMI || OPTKIT_ENV_LIB_ROCM_SMI
#include "core/pmu/gpu/amd/module.hh"
#else
#include "core/pmu/gpu/clear" // default to nvidia
#endif
