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
        case NativeEvents::RETIRED_MICROCODE_OPS:
            return "RETIRED_MICROCODE_OPS";
        case NativeEvents::DISPATCH_STALLS_1:
            return "DISPATCH_STALLS_1";
        case NativeEvents::DISPATCH_STALLS_1_0x6:
            return "DISPATCH_STALLS_1_0x6";
        case NativeEvents::BACKEND_STALLS_1:
            return "BACKEND_STALLS_1";
        case NativeEvents::SMT_STALLS_1:
            return "SMT_STALLS_1";
        case NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER:
            return "OPS_SOURCE_DISPATCHED_FROM_DECODER";
        case NativeEvents::RESYNCS:
            return "RESYNCS";
        case NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE:
            return "CYCLES_NO_RETIRE_NOT_COMPLETE";
        case NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE:
            return "CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE";
        case NativeEvents::L3_CACHE_ACCESSES:
            return "L3_CACHE_ACCESSES";
        case NativeEvents::L2_CACHE_ACCESSES:
            return "L2_CACHE_ACCESSES";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}