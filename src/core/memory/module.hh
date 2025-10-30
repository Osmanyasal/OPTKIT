#pragma once

#include "utils/utils.hh"
#include "core/memory/memory_profiler.hh"
#include "core/query.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_MEMORY_MACROS_ENABLED

#define OPTKIT_MEMORY_EVENTS(block_name)                                                              \
    optkit::memory::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__)                                   \
    {                                                                                                 \
        {block_name, "memory_io", true, optkit::Query::create_folder, !optkit::Query::create_folder}, \
            optkit::metrics::memory::core_metrics::all_metrics()                                      \
    }

#define OPTKIT_MEMORY_EVENTS_WITH_METRICS(block_name, metric_builder) \
    optkit::memory::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, "memory_io", true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder }

#else
#define OPTKIT_MEMORY_EVENTS_WITH_METRICS(block_name)
#define OPTKIT_MEMORY_EVENTS_WITH_METRICS(block_name, metric_builder)
#endif
