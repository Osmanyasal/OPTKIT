#include "core/metrics/performance/cpu/intel/native_events.hh"

#if OPTKIT_ENV_CPU_INTEL
namespace optkit::metrics::cpu::intel
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        case NativeEvents::BR_INST_RETIRED_NEAR_CALL:
            return "BR_INST_RETIRED_NEAR_CALL";
        case NativeEvents::L2_DEMAND_REFERENCES:
            return "L2_DEMAND_REFERENCES";
        case NativeEvents::RESOURCE_STALLS_SB:
            return "RESOURCE_STALLS_SB";
        case NativeEvents::UOPS_CORE_CYCLES_THREAD:
            return "UOPS_CORE_CYCLES_THREAD";
        case NativeEvents::UOPS_CORE_CYCLES_GE_1:
            return "UOPS_CORE_CYCLES_GE_1";
        case NativeEvents::L3_DEMAND_REFERENCES:
            return "L3_DEMAND_REFERENCES";
        case NativeEvents::UOPS_ISSUED:
            return "UOPS_ISSUED";
        case NativeEvents::UOPS_EXECUTED:
            return "UOPS_EXECUTED";
        case NativeEvents::UOPS_RETIRED_SLOTS:
            return "UOPS_RETIRED_SLOTS";
        case NativeEvents::IDQ_MS_UOPS:
            return "IDQ_MS_UOPS";
        case NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE:
            return "IDQ_UOPS_NOT_DELIVERED_CORE";
        case NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0:
            return "IDQ_UOPS_NOT_DELIVERED_CYCLES_0";
        case NativeEvents::INT_MISC_RECOVERY_CYCLES:
            return "INT_MISC_RECOVERY_CYCLES";
        case NativeEvents::MACHINE_CLEARS_COUNT:
            return "MACHINE_CLEARS_COUNT";
        case NativeEvents::STALLS_L1D_MISS:
            return "STALLS_L1D_MISS";
        case NativeEvents::STALLS_L2_MISS:
            return "STALLS_L2_MISS";
        case NativeEvents::STALLS_L3_MISS:
            return "STALLS_L3_MISS";
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
#endif // OPTKIT_ENV_CPU_INTEL