#pragma once

#include "utils/utils.hh"
#include "core/disk/core_events.hh"
#include "core/disk/disk_profiler.hh"

// Select which way (perf or msr) to macro
#if OPTKIT_CONF_DISK_MACROS_ENABLED 

#define OPTKIT_DISK_EVENTS(block_name, metric_builder, ...) \
    optkit::core::disk::IoDiskProfiler EXPAND_AND_CONCAT(var, __LINE__) { block_name, metric_builder, __VA_ARGS__ }

#else
#define OPTKIT_DISK_EVENTS(block_name, metric_builder, ...)
#endif

using optkit::core::disk::operator<<; // make available to global namespace