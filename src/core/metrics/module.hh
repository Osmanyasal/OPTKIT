#pragma once

// #include "core/metrics/intel/icl/icl_governor.hh"
// #include "core/metrics/intel/skl/skl_governor.hh"
#include "core/metrics/tma_metrics.hh"
#include "core/metrics/tmanalysis.hh"
#include "utils/deployment/deployment_config.hh"

#if defined(CONF__PMU__MACROS__ENABLED) && CONF__PMU__MACROS__ENABLED == 1

#define OPTKIT_TMA_ANALYSIS(block_name, variable_name, TMA_RECEPIE)                                   \
    std::string variable_name##_event_name = optkit::core::metrics::to_string(TMA_RECEPIE);          \
    optkit::core::metrics::TMAnalysis variable_name(block_name, variable_name##_event_name.c_str()); \
    variable_name.begin_monitoring(TMA_RECEPIE)

#else

#define OPTKIT_TMA_ANALYSIS(...)

#endif