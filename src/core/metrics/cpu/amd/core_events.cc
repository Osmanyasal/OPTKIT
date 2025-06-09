#include "core/metrics/cpu/amd/core_events.hh"

namespace optkit::core::metrics::cpu::amd
{

    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY:
            return "RETIRED_SSE_AVX_FLOPS_ANY";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, amd::CoreEvents event)
    {
        return os << to_string(event);
    }
}