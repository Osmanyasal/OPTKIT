#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/energy/cpu/rapl/rapl.hh"
#include "core/energy/cpu/rapl/rapl_perf_reader.hh"
#include "core/energy/cpu/rapl/rapl_profiler.hh"
#include "core/energy/cpu/rapl/rapl_utils.hh"
#include "core/energy/cpu/rapl/query_rapl.hh"

/*
    Static instance is defined because monitoring recursive methods would cause an issue
*/

#if defined(CONF__RAPL__MACROS__ENABLED) && CONF__RAPL__MACROS__ENABLED == 1

#define OPTKIT_RAPL(var_name, block_name)        \
    optkit::core::energy::rapl::RaplProfiler var_name{block_name};

#define OPTKIT_RAPL_REPEAT(var_name, block_name, count)    \
    optkit::core::energy::rapl::RaplProfiler var_name{block_name}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_RAPL_REPEAT_READ_AND_STORE(var_name, block_name, count) \
    optkit::core::energy::rapl::RaplProfiler var_name{block_name};   \
    for (int32_t i = 0; i < count; i++, var_name.read_and_store())

#else
    #define OPTKIT_RAPL(var_name, block_name)
    #define OPTKIT_RAPL_REPEAT(var_name, block_name, count)
    #define OPTKIT_RAPL_REPEAT_READ_AND_STORE(var_name, block_name, count)
#endif


using optkit::core::energy::rapl::operator<<; // make available to global namespace