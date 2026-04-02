#pragma once

#include "utils/utils.hh"
#include "utils/deployment/deployment_config.hh"
#include "core/energy/cpu/pdu/profiler.hh"
#include "core/query.hh"

#if OPTKIT_CONF_PDU_MACROS_ENABLED == 1 && OPTKIT_ENV_LIB_NET_SNMP

#define OPTKIT_PDU_ENERGY(block_name) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()};

#define OPTKIT_PDU_ENERGY_REPEAT(block_name, count) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE(block_name, count) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_PDU_ENERGY_WITH_METRICS(block_name, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_PDU_ENERGY_REPEAT_WITH_METRICS(block_name, count, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS(block_name, count, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_PDU_ENERGY_SAMPLING(block_name) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()};

#define OPTKIT_PDU_ENERGY_REPEAT_SAMPLING(block_name, count) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_SAMPLING(block_name, count) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, optkit::energy::pdu::default_metrics()}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#define OPTKIT_PDU_ENERGY_WITH_METRICS_SAMPLING(block_name, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder};

#define OPTKIT_PDU_ENERGY_REPEAT_WITH_METRICS_SAMPLING(block_name, count, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++)

#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS_SAMPLING(block_name, count, metric_builder) \
    optkit::energy::pdu::Profiler EXPAND_AND_CONCAT(var, __LINE__){{block_name, "cpu_energy", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder}; \
    for (int32_t i = 0; i < count; i++, EXPAND_AND_CONCAT(var, __LINE__).read_and_store())

#else

#define OPTKIT_PDU_ENERGY(block_name)
#define OPTKIT_PDU_ENERGY_REPEAT(block_name, count)
#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE(block_name, count)
#define OPTKIT_PDU_ENERGY_WITH_METRICS(block_name, metric_builder)
#define OPTKIT_PDU_ENERGY_REPEAT_WITH_METRICS(block_name, count, metric_builder)
#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS(block_name, count, metric_builder)
#define OPTKIT_PDU_ENERGY_SAMPLING(block_name)
#define OPTKIT_PDU_ENERGY_REPEAT_SAMPLING(block_name, count)
#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_SAMPLING(block_name, count)
#define OPTKIT_PDU_ENERGY_WITH_METRICS_SAMPLING(block_name, metric_builder)
#define OPTKIT_PDU_ENERGY_REPEAT_WITH_METRICS_SAMPLING(block_name, count, metric_builder)
#define OPTKIT_PDU_ENERGY_REPEAT_READ_AND_STORE_WITH_METRICS_SAMPLING(block_name, count, metric_builder)

#endif