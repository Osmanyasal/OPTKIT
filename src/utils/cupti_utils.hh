#pragma once

#include "utils/logging/logger.hh"
#if OPTKIT_ENV_LIB_NVML
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cupti.h>

#ifndef CUPTI_API_CALL
#define CUPTI_API_CALL(apiFunctionCall)                                                             \
    do                                                                                              \
    {                                                                                               \
        CUptiResult _status = apiFunctionCall;                                                      \
        if (_status != CUPTI_SUCCESS)                                                               \
        {                                                                                           \
            const char *pErrorString;                                                               \
            cuptiGetResultString(_status, &pErrorString);                                           \
                                                                                                    \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",                     \
                         #apiFunctionCall, " failed with error(", static_cast<int>(_status), "): ", \
                         pErrorString, ".\n\n");                                                    \
                                                                                                    \
            exit(EXIT_FAILURE);                                                                     \
        }                                                                                           \
    } while (0)
#endif

#ifndef CUPTI_API_CALL_VERBOSE
#define CUPTI_API_CALL_VERBOSE(apiFunctionCall)                                                     \
    do                                                                                              \
    {                                                                                               \
        std::cout << "Calling CUPTI API: " << #apiFunctionCall << "\n";                             \
                                                                                                    \
        CUptiResult _status = apiFunctionCall;                                                      \
        if (_status != CUPTI_SUCCESS)                                                               \
        {                                                                                           \
            const char *pErrorString;                                                               \
            cuptiGetResultString(_status, &pErrorString);                                           \
                                                                                                    \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",                     \
                         #apiFunctionCall, " failed with error(", static_cast<int>(_status), "): ", \
                         pErrorString, ".\n\n");                                                    \
                                                                                                    \
            exit(EXIT_FAILURE);                                                                     \
        }                                                                                           \
    } while (0)
#endif

#ifndef CUPTI_UTIL_CALL
#define CUPTI_UTIL_CALL(apiFunctionCall)                                                               \
    do                                                                                                 \
    {                                                                                                  \
        CUptiUtilResult _status = apiFunctionCall;                                                     \
        if (_status != CUPTI_UTIL_SUCCESS)                                                             \
        {                                                                                              \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",                        \
                         #apiFunctionCall, " failed with error: ", static_cast<int>(_status), "\n\n"); \
                                                                                                       \
            exit(EXIT_FAILURE);                                                                        \
        }                                                                                              \
    } while (0)
#endif

// Helper to print memory kind strings (Host, Device, Array, etc.)
inline std::string getMemKindString(CUpti_ActivityMemoryKind kind)
{
    switch (kind)
    {
    case CUPTI_ACTIVITY_MEMORY_KIND_UNKNOWN:
        return "Unknown";
    case CUPTI_ACTIVITY_MEMORY_KIND_PAGEABLE:
        return "Pageable";
    case CUPTI_ACTIVITY_MEMORY_KIND_PINNED:
        return "Pinned";
    case CUPTI_ACTIVITY_MEMORY_KIND_DEVICE:
        return "Device";
    case CUPTI_ACTIVITY_MEMORY_KIND_ARRAY:
        return "Array";
    case CUPTI_ACTIVITY_MEMORY_KIND_MANAGED:
        return "Managed";
    case CUPTI_ACTIVITY_MEMORY_KIND_DEVICE_STATIC:
        return "Device Static";
    case CUPTI_ACTIVITY_MEMORY_KIND_MANAGED_STATIC:
        return "Managed Static";
    default:
        return "Other";
    }
}
#endif