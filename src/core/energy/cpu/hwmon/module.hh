#pragma once

#include "utils/utils.hh"
#include "utils/deployment/deployment_config.hh"
#include "core/energy/cpu/hwmon/profiler.hh"
#include "core/metrics/energy/module.hh"
#include "core/query.hh"

#if OPTKIT_CONF_HWMON_MACROS_ENABLED == 1

// NON SAMPLING MACROS
#define OPTKIT_HWMON_ENERGY(block_name) \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()};

#define OPTKIT_HWMON_ENERGY_REPEAT(block_name, count)                                                                                                                                                                             \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE(block_name, count)                                                                                                                                                              \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// Macros with metric_builder parameter
#define OPTKIT_HWMON_ENERGY_WITH_METRICS(block_name, metric_builder) \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_HWMON_ENERGY_REPEAT_WITH_METRICS(block_name, count, metric_builder)                                                                                                           \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS(block_name, count, metric_builder)                                                                                            \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// SAMPLING MACROS
#define OPTKIT_HWMON_ENERGY_SAMPLING(block_name) \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()};

#define OPTKIT_HWMON_ENERGY_REPEAT_SAMPLING(block_name, count)                                                                                                                                                                   \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_SAMPLING(block_name, count)                                                                                                                                                    \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::metrics::energy::cpu_metrics::all_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

// Macros with metric_builder parameter
#define OPTKIT_HWMON_ENERGY_WITH_METRICS_SAMPLING(block_name, metric_builder) \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_HWMON_ENERGY_REPEAT_WITH_METRICS_SAMPLING(block_name, count, metric_builder)                                                                                                  \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS_SAMPLING(block_name, count, metric_builder)                                                                                   \
    optkit::energy::hwmon::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "hwmon_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#else

#define OPTKIT_HWMON_ENERGY(block_name)
#define OPTKIT_HWMON_ENERGY_REPEAT(block_name, count)
#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE(block_name, count)
#define OPTKIT_HWMON_ENERGY_WITH_METRICS(block_name, metric_builder)
#define OPTKIT_HWMON_ENERGY_REPEAT_WITH_METRICS(block_name, count, metric_builder)
#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS(block_name, count, metric_builder)

#define OPTKIT_HWMON_ENERGY_SAMPLING(block_name)
#define OPTKIT_HWMON_ENERGY_REPEAT_SAMPLING(block_name, count)
#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_SAMPLING(block_name, count)
#define OPTKIT_HWMON_ENERGY_WITH_METRICS_SAMPLING(block_name, metric_builder)
#define OPTKIT_HWMON_ENERGY_REPEAT_WITH_METRICS_SAMPLING(block_name, count, metric_builder)
#define OPTKIT_HWMON_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS_SAMPLING(block_name, count, metric_builder)

#endif
