#pragma once

// =============================================
// Include subsystem headers
// =============================================

#include "core/pmu/cpu/module.hh" // if use_msr is enabled, include defaults + use_msr module likewise for perf. same macro but different classes is defined for them.
#include "core/energy/module.hh"
#include "core/frequency/module.hh"
#include "core/disk/module.hh"
#include "core/temperature/module.hh"
#include "core/metrics/module.hh"
#include "core/gpu_query.hh"
#include "core/query.hh"

// based on the configuration, each module is responsible including "ways" for the module.