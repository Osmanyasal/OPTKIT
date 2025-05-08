#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/profiling/rapl/rapl.hh"
#include "core/profiling/rapl/rapl_perf_reader.hh"
#include "core/profiling/rapl/rapl_profiler.hh"
#include "core/profiling/rapl/rapl_utils.hh"
#include "core/profiling/rapl/query_rapl.hh"

/*
    Static instance is defined because monitoring recursive methods would cause an issue
*/

#if defined(CONF__RAPL__MACROS__ENABLED) && CONF__RAPL__MACROS__ENABLED == 1

#define OPTKIT_RAPL(var_name, block_name)        \
    OPTKIT_CORE_GANTT_PROFILE_SCOPE(block_name); \
    optkit::core::rapl::RaplProfiler var_name{block_name};

#define OPTKIT_RAPL_REPEAT(var_name, block_name, count)    \
    OPTKIT_CORE_GANTT_PROFILE_SCOPE(block_name);           \
    optkit::core::rapl::RaplProfiler var_name{block_name}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_RAPL_REPEAT_READ(var_name, block_name, count) \
    OPTKIT_CORE_GANTT_PROFILE_SCOPE(block_name);             \
    optkit::core::rapl::RaplProfiler var_name{block_name};   \
    for (int32_t i = 0; i < count; i++, var_name.read())

#else
    #define OPTKIT_RAPL(var_name, block_name)
    #define OPTKIT_RAPL_REPEAT(var_name, block_name, count)
    #define OPTKIT_RAPL_REPEAT_READ(var_name, block_name, count)
#endif


using optkit::core::rapl::operator<<; // make available to global namespace