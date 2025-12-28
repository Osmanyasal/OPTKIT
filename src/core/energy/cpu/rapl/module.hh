#pragma once

#include "utils/utils.hh"
#include "utils/deployment/deployment_config.hh"
#include "core/energy/cpu/rapl/profiler.hh"
#include "core/metrics/energy/module.hh"
#include "core/query.hh"

#if OPTKIT_CONF_RAPL_MACROS_ENABLED == 1

// NON SAMPLING MACROS
#define OPTKIT_CPU_ENERGY(block_name) \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()};

#define OPTKIT_CPU_ENERGY_REPEAT(block_name, count)                                                                                                                                                                             \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE(block_name, count)                                                                                                                                                              \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// Macros with metric_builder parameter
#define OPTKIT_CPU_ENERGY_WITH_METRICS(block_name, metric_builder) \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_CPU_ENERGY_REPEAT_WITH_METRICS(block_name, count, metric_builder)                                                                                                           \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS(block_name, count, metric_builder)                                                                                            \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// SAMPLING MACROS
#define OPTKIT_CPU_ENERGY_SAMPLING(block_name) \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()};

#define OPTKIT_CPU_ENERGY_REPEAT_SAMPLING(block_name, count)                                                                                                                                                                   \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE_SAMPLING(block_name, count)                                                                                                                                                    \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// Macros with metric_builder parameter
#define OPTKIT_CPU_ENERGY_WITH_METRICS_SAMPLING(block_name, metric_builder) \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_CPU_ENERGY_REPEAT_WITH_METRICS_SAMPLING(block_name, count, metric_builder)                                                                                                 \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_CPU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS_SAMPLING(block_name, count, metric_builder)                                                                                  \
    optkit::energy::rapl::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#else
#include "core/energy/cpu/rapl/clear.hh"
#endif
