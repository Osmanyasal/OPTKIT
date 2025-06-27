#pragma once

#include <string>
namespace optkit::core::metrics::cpu::intel
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class NativeEvents
    {
        BEGIN = 0,
        RESOURCE_STALLS_SB,
        L2_DEMAND_REFERENCES,
        UOPS_ISSUED,
        UOPS_EXECUTED,
        UOPS_RETIRED_SLOTS,
        INT_MISC_RECOVERY_CYCLES,
        IDQ_UOPS_NOT_DELIVERED_CORE,
        FP_ARITH_INST_RETIRED_SCALAR_SINGLE,
        FP_ARITH_INST_RETIRED_SCALAR_DOUBLE,
        FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE,
        FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE,
        FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE,
        FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE,
        FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE,
        FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE,
        FP_ARITH_INST_RETIRED_SCALAR,
        FP_ARITH_INST_RETIRED_VECTOR,
        END,
    };

    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);

}