#pragma once

#include "utils/logging/logger.hh"
#include <cupti.h>

#ifndef CUPTI_API_CALL
#define CUPTI_API_CALL(apiFunctionCall)                                           \
    do                                                                            \
    {                                                                             \
        CUptiResult _status = apiFunctionCall;                                    \
        if (_status != CUPTI_SUCCESS)                                             \
        {                                                                         \
            const char *pErrorString;                                             \
            cuptiGetResultString(_status, &pErrorString);                         \
                                                                                  \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",   \
                         #apiFunctionCall, " failed with error(", _status, "): ", \
                         pErrorString, ".\n\n");                                  \
                                                                                  \
            exit(EXIT_FAILURE);                                                   \
        }                                                                         \
    } while (0)
#endif

#ifndef CUPTI_API_CALL_VERBOSE
#define CUPTI_API_CALL_VERBOSE(apiFunctionCall)                                   \
    do                                                                            \
    {                                                                             \
        std::cout << "Calling CUPTI API: " << #apiFunctionCall << "\n";           \
                                                                                  \
        CUptiResult _status = apiFunctionCall;                                    \
        if (_status != CUPTI_SUCCESS)                                             \
        {                                                                         \
            const char *pErrorString;                                             \
            cuptiGetResultString(_status, &pErrorString);                         \
                                                                                  \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",   \
                         #apiFunctionCall, " failed with error(", _status, "): ", \
                         pErrorString, ".\n\n");                                  \
                                                                                  \
            exit(EXIT_FAILURE);                                                   \
        }                                                                         \
    } while (0)
#endif

#ifndef CUPTI_UTIL_CALL
#define CUPTI_UTIL_CALL(apiFunctionCall)                                             \
    do                                                                               \
    {                                                                                \
        CUptiUtilResult _status = apiFunctionCall;                                   \
        if (_status != CUPTI_UTIL_SUCCESS)                                           \
        {                                                                            \
            OPTKIT_ERROR("\n\nError: ", __FILE__, ":", __LINE__, ": Function ",      \
                         #apiFunctionCall, " failed with error: ", _status, "\n\n"); \
                                                                                     \
            exit(EXIT_FAILURE);                                                      \
        }                                                                            \
    } while (0)
#endif