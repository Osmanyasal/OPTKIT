#include "core/metrics/cpu/amd/native_events.hh"

namespace optkit::core::metrics::cpu::amd
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        // Pipeline and Stalls
        case NativeEvents::RETIRED_OPS:
            return "RETIRED_OPS";
        case NativeEvents::DISPATCH_STALLS_1:
            return "DISPATCH_STALLS_1";
        case NativeEvents::BACKEND_STALLS_1:
            return "BACKEND_STALLS_1";
        case NativeEvents::SMT_STALLS_1:
            return "SMT_STALLS_1";
        case NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER:
            return "OPS_SOURCE_DISPATCHED_FROM_DECODER";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}