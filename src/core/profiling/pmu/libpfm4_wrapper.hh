#pragma once

#include <perfmon/pfmlib.h>

namespace optkit::core::pmu
{
    /**
     * @brief All PMU names,
     * this will be used in combination of @ref perfmon/pfmlib.h "perfmon/pfmlib.h pfm_pmu_t and pfm_pmu_type_t"
     *
     */
    extern const char *pmu_names[];
    extern const char *pmu_types[];
}
