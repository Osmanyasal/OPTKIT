#pragma once

#include "utils/utils.hh"
#include "core/disk/disk_profiler.hh"
#include "core/query.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_DISK_MACROS_ENABLED

#define OPTKIT_DISK_EVENTS(block_name)                                                                    \
    optkit::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__)                                         \
    {                                                                                                     \
        {block_name, "disk_io", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, \
            optkit::metrics::disk::core_metrics::all_metrics()                                            \
    }

#define OPTKIT_DISK_EVENTS_SAMPLING(block_name)                                                           \
    optkit::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__)                                         \
    {                                                                                                     \
        {block_name, "disk_io", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, \
            optkit::metrics::disk::core_metrics::all_metrics()                                            \
    }

#define OPTKIT_DISK_EVENTS_WITH_METRICS(block_name, metric_builder) \
    optkit::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, "disk_io", true, false, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder }

#define OPTKIT_DISK_EVENTS_WITH_METRICS_SAMPLING(block_name, metric_builder) \
    optkit::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, "disk_io", true, true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder }

#else
#include "core/disk/clear.hh"
#endif
