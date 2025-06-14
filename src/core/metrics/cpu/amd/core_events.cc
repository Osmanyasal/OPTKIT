#include "core/metrics/cpu/amd/core_events.hh"

namespace optkit::core::metrics::cpu::amd
{

    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY:
            return "RETIRED_SSE_AVX_FLOPS_ANY";
            
#if OPTKIT_ENV_CPU_MICROARCH_ZEN

#elif OPTKIT_ENV_CPU_MICROARCH_ZENPLUS

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN2

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN3

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_ADD:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_ADD";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_SUB:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_SUB";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_MUL:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_MUL";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_MAC:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_MAC";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_DIV:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_DIV";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_SQRT:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_SQRT";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_CMP:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_CMP";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_CVT:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_CVT";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_BLEND:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_BLEND";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_OTHER:
        //     return "RETIRED_FP_OPS_BY_TYPE_SCALAR_OTHER";
        case CoreEvents::RETIRED_FP_OPS_BY_TYPE_SCALAR_ALL:
            return "RETIRED_FP_OPS_BY_TYPE_SCALAR_ALL";

        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_ADD:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_ADD";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_SUB:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_SUB";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_MUL:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_MUL";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_MAC:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_MAC";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_DIV:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_DIV";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_SQRT:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_SQRT";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_CMP:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_CMP";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_CVT:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_CVT";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_BLEND:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_BLEND";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_SHUFFLE:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_SHUFFLE";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_LOGICAL:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_LOGICAL";
        // case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_OTHER:
        //     return "RETIRED_FP_OPS_BY_TYPE_VECTOR_OTHER";
        case CoreEvents::RETIRED_FP_OPS_BY_TYPE_VECTOR_ALL:
            return "RETIRED_FP_OPS_BY_TYPE_VECTOR_ALL";
#endif
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, amd::CoreEvents event)
    {
        return os << to_string(event);
    }
}