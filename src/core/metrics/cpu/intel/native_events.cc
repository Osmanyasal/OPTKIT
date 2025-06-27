#include "core/metrics/cpu/intel/native_events.hh"

namespace optkit::core::metrics::cpu::intel
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        case NativeEvents::RESOURCE_STALLS_SB:
            return "RESOURCE_STALLS_SB";

        case NativeEvents::L2_DEMAND_REFERENCES:
            return "L2_DEMAND_REFERENCES";

        case NativeEvents::UOPS_ISSUED:
            return "UOPS_ISSUED";
        case NativeEvents::UOPS_EXECUTED:
            return "UOPS_EXECUTED";
        case NativeEvents::UOPS_RETIRED_SLOTS:
            return "UOPS_RETIRED_SLOTS";
        case NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE:
            return "IDQ_UOPS_NOT_DELIVERED_CORE";
        case NativeEvents::INT_MISC_RECOVERY_CYCLES:
            return "INT_MISC_RECOVERY_CYCLES";
        case NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE:
            return "FP_ARITH_INST_RETIRED_SCALAR_SINGLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE:
            return "FP_ARITH_INST_RETIRED_SCALAR_DOUBLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE:
            return "FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE:
            return "FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE:
            return "FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE:
            return "FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE:
            return "FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE:
            return "FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE";
        case NativeEvents::FP_ARITH_INST_RETIRED_SCALAR:
            return "FP_ARITH_INST_RETIRED_SCALAR";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}