#pragma once

// #include "core/metrics/intel/icl/icl_governor.hh"
// #include "core/metrics/intel/skl/skl_governor.hh"
#include "core/metrics/cpu/tma_metrics.hh"
#include "core/metrics/cpu/tmanalysis.hh"
#include "utils/deployment/deployment_config.hh"

#if OPTKIT_CONF_PMU_MACROS_ENABLED

#define OPTKIT_TMA_ANALYSIS(block_name, variable_name, TMA_RECEPIE)                                   \
    std::string variable_name##_event_name = optkit::core::metrics::to_string(TMA_RECEPIE);          \
    optkit::core::metrics::TMAnalysis variable_name(block_name, variable_name##_event_name.c_str()); \
    variable_name.begin_monitoring(TMA_RECEPIE)

#else

#define OPTKIT_TMA_ANALYSIS(...)

#endif