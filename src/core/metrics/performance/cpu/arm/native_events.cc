#include "core/metrics/performance/cpu/arm/native_events.hh"
#if OPTKIT_ENV_CPU_ARM
namespace optkit::metrics::cpu::arm
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        case NativeEvents::STALL_FRONTEND:
            return "STALL_FRONTEND";
        case NativeEvents::STALL_BACKEND:
            return "STALL_BACKEND";
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
        case NativeEvents::L1D_CACHE_ACCESSES:
            return "L1D_CACHE_ACCESSES";
        case NativeEvents::L2_CACHE_ACCESSES:
            return "L2_CACHE_ACCESSES";
        case NativeEvents::L3_CACHE_ACCESSES:
            return "L3_CACHE_ACCESSES";
        case NativeEvents::STALL_FRONTEND_FLUSH:
            return "STALL_FRONTEND_FLUSH";
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}
#endif // OPTKIT_ENV_CPU_ARM