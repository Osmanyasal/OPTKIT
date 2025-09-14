#pragma once

#include "utils/deployment/deployment_config.hh"
#include "core/energy/cpu/rapl/rapl_profiler.hh"
#include "core/query.hh"

/*
    Static instance is defined because monitoring recursive methods would cause an issue
*/

#if OPTKIT_CONF_RAPL_MACROS_ENABLED == 1

#define OPTKIT_CPU_ENERGY(var_name, block_name) \
    optkit::energy::rapl::RaplProfiler var_name{{block_name, "cpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}};

#define OPTKIT_CPU_ENERGY_REPEAT(var_name, block_name, count)                                                                                   \
    optkit::energy::rapl::RaplProfiler var_name{{block_name, "cpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE(var_name, block_name, count)                                                                    \
    optkit::energy::rapl::RaplProfiler var_name{{block_name, "cpu_energy", true, optkit::Query::create_folder, !optkit::Query::create_folder}}; \
    for (int32_t i = 0; i < count; i++, var_name.read_and_store())

#else
#define OPTKIT_CPU_ENERGY(var_name, block_name)
#define OPTKIT_CPU_ENERGY_REPEAT(var_name, block_name, count)
#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE(var_name, block_name, count)
#endif
