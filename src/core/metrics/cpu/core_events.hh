#pragma once

#include <string>
namespace optkit::core::metrics::cpu
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class CoreEvents
    {
        BEGIN = 0,

        // Pipeline and Stalls
        DISPATCH_SLOTS, // multiply by 4,6 or 8 based on the cpu
        UNHALTED_CORE_CYCLES,
        // UNHALTED_REFERENCE_CYCLES,
        RESOURCE_STALLS,
        // RECOVERY_CYCLES,

        // Instruction Events
        INST_RETIRED,
        // UOPS_ISSUED,
        // UOPS_EXECUTED,
        // UOPS_RETIRED,
        // IDQ_UOPS_NOT_DELIVERED,

        // Branch Prediction
        BRANCH_INST_RETIRED,
        BRANCH_MISP_RETIRED,
        // MACHINE_CLEARS,

        // Cache Events
        L1_MISSES,
        L1_HITS,

        L2_MISSES,
        L2_HITS,

        L3_MISSES,
        L3_HITS,

        // Memory Events
        MEM_INST_RETIRED,  // any kind of memory instructions
        MEM_LOAD_RETIRED,  // any kind of memory load instructions
        MEM_STORE_RETIRED, // any kind of memory store instructions
        DTLB_MISSES,
        ITLB_MISSES,
        DTLB_MISSES_GOES_PAGE_WALK,
        ITLB_MISSES_GOES_PAGE_WALK,
        SW_LOAD_PREFETCH_ACCESS,

        // FP/Vector
        RETIRED_FLOPS_ANY,     // single FP ops (ie. adding of flop variables)
        RETIRED_VECTOR, // total FP operations caused by see_avx vector instruction

        END,
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);

}