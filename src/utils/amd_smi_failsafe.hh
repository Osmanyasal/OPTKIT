#pragma once
#if OPTKIT_ENV_LIB_AMDSMI
#include <string>
#include "utils/optimizations/cpu_opt.hh"

OPT_FORCE_INLINE std::string _amdsmi_status_to_string(amdsmi_status_t status)
{
    const char *status_str = nullptr;
    if (amdsmi_status_code_to_string(status, &status_str) == AMDSMI_STATUS_SUCCESS && status_str)
    {
        return std::string(status_str);
    }
    return "Unknown error";
}

#endif