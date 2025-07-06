#include "core/metrics/cpu/arm/native_events.hh"

namespace optkit::core::metrics::cpu::arm
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        case NativeEvents::STALL_FRONTEND:
            return "STALL_FRONTEND";
        case NativeEvents::STALL_BACKEND:
            return "STALL_BACKEND";
#if !OPTKIT_ENV_CPU_ARM_N1
        case NativeEvents::STALL_SLOT:
            return "STALL_SLOT";
        case NativeEvents::STALL_SLOT_FRONTEND:
            return "STALL_SLOT_FRONTEND";
        case NativeEvents::STALL_SLOT_BACKEND:
            return "STALL_SLOT_BACKEND";
        case NativeEvents::OP_RETIRED:
            return "OP_RETIRED";
        case NativeEvents::OP_SPEC:
            return "OP_SPEC";
#endif
        case NativeEvents::L1D_CACHE_ACCESSES:
            return "L1D_CACHE_ACCESSES";
        case NativeEvents::L2_CACHE_ACCESSES:
            return "L2_CACHE_ACCESSES";
        case NativeEvents::L3_CACHE_ACCESSES:
            return "L3_CACHE_ACCESSES";
#if OPTKIT_ENV_CPU_ARM_N3 || OPTKIT_ENV_CPU_ARM_V3
        case NativeEvents::STALL_FRONTEND_FLUSH:
            return "STALL_FRONTEND_FLUSH";
#endif
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}