#pragma once
#if OPTKIT_ENV_LIB_ROCM_SMI
#include <string>
#include "utils/optimizations/cpu_opt.hh"

OPT_FORCE_INLINE std::string _rocm_smi_status_to_string(rsmi_status_t status)
{
    const char *status_str = nullptr;
    if (rsmi_status_string(status, &status_str) == RSMI_STATUS_SUCCESS && status_str)
    {
        return std::string(status_str);
    }
    return "Unknown error";
}

#endif
