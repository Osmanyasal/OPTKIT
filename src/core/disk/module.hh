#pragma once

#include "utils/utils.hh"
#include "core/disk/disk_profiler.hh"
#include "core/query.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_DISK_MACROS_ENABLED

#define OPTKIT_DISK_EVENTS(block_name, metric_builder, ...) \
    optkit::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name, "disk_io", true, optkit::Query::create_folder, !optkit::Query::create_folder}, metric_builder, __VA_ARGS__ }

#else
#define OPTKIT_DISK_EVENTS(block_name, metric_builder, ...)
#endif
