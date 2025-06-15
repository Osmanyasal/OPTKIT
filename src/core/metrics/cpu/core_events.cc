#include "core/metrics/cpu/core_events.hh"

namespace optkit::core::metrics::cpu
{

    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        // Pipeline and Stalls
        case CoreEvents::UNHALTED_CORE_CYCLES:
            return "UNHALTED_CORE_CYCLES";
        case CoreEvents::UNHALTED_REFERENCE_CYCLES:
            return "UNHALTED_REFERENCE_CYCLES";
        case CoreEvents::RESOURCE_STALLS:
            return "RESOURCE_STALLS";
        // case CoreEvents::RECOVERY_CYCLES:
        //     return "RECOVERY_CYCLES";

        // Instruction Events
        case CoreEvents::INST_RETIRED:
            return "INST_RETIRED";
        // case CoreEvents::UOPS_ISSUED:
        //     return "UOPS_ISSUED";
        // case CoreEvents::UOPS_EXECUTED:
        //     return "UOPS_EXECUTED";
        // case CoreEvents::UOPS_RETIRED:
        //     return "UOPS_RETIRED";
        // case CoreEvents::IDQ_UOPS_NOT_DELIVERED:
        //     return "IDQ_UOPS_NOT_DELIVERED";

        // Branch Prediction
        case CoreEvents::BRANCH_INST_RETIRED:
            return "BRANCH_INST_RETIRED";
        case CoreEvents::BRANCH_MISP_RETIRED:
            return "BRANCH_MISP_RETIRED";
        // case CoreEvents::MACHINE_CLEARS:
        //     return "MACHINE_CLEARS";

        // Cache Events
        case CoreEvents::L1_MISSES:
            return "L1_MISSES";
        case CoreEvents::L1_HITS:
            return "L1_HITS";
        case CoreEvents::L2_MISSES:
            return "L2_MISSES";
        case CoreEvents::L2_HITS:
            return "L2_HITS";
        case CoreEvents::L3_MISSES:
            return "L3_MISSES";
        case CoreEvents::L3_HITS:
            return "L3_HITS";

        // Memory Events
        case CoreEvents::MEM_INST_RETIRED:
            return "MEM_INST_RETIRED";
        case CoreEvents::MEM_LOAD_RETIRED:
            return "MEM_LOAD_RETIRED";
        case CoreEvents::MEM_STORE_RETIRED:
            return "MEM_STORE_RETIRED";
        case CoreEvents::DTLB_MISSES:
            return "DTLB_MISSES";
        case CoreEvents::ITLB_MISSES:
            return "ITLB_MISSES";
        case CoreEvents::SW_LOAD_PREFETCH_ACCESS:
            return "SW_LOAD_PREFETCH_ACCESS";

        // FP/Vector
        case CoreEvents::RETIRED_FLOPS_ANY:
            return "RETIRED_FLOPS_ANY";
        case CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY:
            return "RETIRED_SSE_AVX_FLOPS_ANY";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}