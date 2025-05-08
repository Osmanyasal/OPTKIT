#pragma once

#include "core/event_recepies/intel/icl/icl_governor.hh"
#include "core/event_recepies/intel/skl/skl_governor.hh"
#include "core/event_recepies/tma_metrics.hh"
#include "core/event_recepies/tmanalysis.hh"
#include "utils/deployment/deployment_config.hh"

using optkit::core::recepies::operator<<;

#if defined(CONF__PMU__MACROS__ENABLED) && CONF__PMU__MACROS__ENABLED == 1

#define OPTKIT_TMA_ANALYSIS(block_name, variable_name, TMA_RECEPIE)                                   \
    std::string variable_name##_event_name = optkit::core::recepies::to_string(TMA_RECEPIE);          \
    optkit::core::recepies::TMAnalysis variable_name(block_name, variable_name##_event_name.c_str()); \
    variable_name.begin_monitoring(TMA_RECEPIE)

#else

#define OPTKIT_TMA_ANALYSIS(...)

#endif