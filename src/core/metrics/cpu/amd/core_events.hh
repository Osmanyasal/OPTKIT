#pragma once

#include "utils/environment_config.hh"
#include "core/metrics/cpu/core_events.hh"
namespace optkit::core::metrics::cpu::amd
{
    /**
     * @brief These are the core events to be monitored for metrics. Events are specific to AMD chips.
     *
     */
    enum class CoreEvents
    {
        BEGIN = 0,
        RETIRED_SSE_AVX_FLOPS_ANY,

#if OPTKIT_ENV_CPU_MICROARCH_ZEN

#elif OPTKIT_ENV_CPU_MICROARCH_ZENPLUS

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN2

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN3

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
        RETIRED_FP_OPS_BY_TYPE_SCALAR_ADD,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_SUB,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_MUL,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_MAC,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_DIV,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_SQRT,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_CMP,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_CVT,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_BLEND,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_OTHER,
        RETIRED_FP_OPS_BY_TYPE_SCALAR_ALL,

        RETIRED_FP_OPS_BY_TYPE_VECTOR_ADD,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_SUB,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_MUL,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_MAC,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_DIV,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_SQRT,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_CMP,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_CVT,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_BLEND,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_SHUFFLE,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_LOGICAL,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_OTHER,
        RETIRED_FP_OPS_BY_TYPE_VECTOR_ALL,
#endif

        END

    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);
}