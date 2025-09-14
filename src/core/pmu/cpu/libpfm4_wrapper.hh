#pragma once

#include "perfmon/pfmlib.h"

namespace optkit::pmu::cpu
{
    /**
     * @brief All PMU names,
     * this will be used in combination of @ref perfmon/pfmlib.h "perfmon/pfmlib.h pfm_pmu_t and pfm_pmu_type_t"
     *
     * ---> THIS IS IDENTICAL TO PFM_PMU_T and PFM_PMU_TYPE_T STRUCTURES, DO NOT CHANGE THE ORDER !!
     */
    extern const char *pmu_names[];
    extern const char *pmu_types[];
}
