#pragma once

#include "core/pmu/gpu/amd/block_profiler.hh"

#define OPTKIT_GPU_EVENTS(block_name, metric_builder) \
    optkit::pmu::gpu::amd::BlockProfiler EXPAND_AND_CONCAT(var, __LINE__) { {block_name}, metric_builder }