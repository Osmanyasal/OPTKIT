#pragma once

#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD
#include <string>
#include <vector>
namespace optkit::metrics::performance::cpu::amd
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class NativeEvents
    {
        BEGIN = 0,
        SNOOP_HIT_MODIFIED,
        RETIRED_OPS,
        RETIRED_MICROCODE_OPS,
        DISPATCH_STALLS_1,
        DISPATCH_STALLS_1_0x6,
        BACKEND_STALLS_1,
        SMT_STALLS_1,
        OPS_SOURCE_DISPATCHED_FROM_DECODER,
        RESYNCS,
        CYCLES_NO_RETIRE_NOT_COMPLETE,
        CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE,
        MAB_ALLOCATION_BY_TYPE_LS,
        DEMAND_DATA_CACHE_FILLS_FROM_SYSTEM_LOCAL_CCX,
        DEMAND_DATA_CACHE_FILLS_FROM_SYSTEM_NEAR_CACHE_NEAR_FAR,
        L3_CACHE_ACCESSES,
        L2_CACHE_ACCESSES,
        SCALAR_SINGLE_FLOPS,
        PACKED_SINGLE_FLOPS,
        SCALAR_DOUBLE_FLOPS,
        PACKED_DOUBLE_FLOPS,
        END,
    };
    static const std::vector<std::string> &get_native_events()
    {
        static std::vector<std::string> native_events{
            "RETIRED_OPS",
            "RETIRED_MICROCODE_OPS",
            "DISPATCH_STALLS_1",
            "DISPATCH_STALLS_1_0x6",
            "BACKEND_STALLS_1",
            "SMT_STALLS_1",
            "OPS_SOURCE_DISPATCHED_FROM_DECODER",
            "RESYNCS",
            "CYCLES_NO_RETIRE_NOT_COMPLETE",
            "CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE",
            "MAB_ALLOCATION_BY_TYPE_LS",
            "DEMAND_DATA_CACHE_FILLS_FROM_SYSTEM_LOCAL_CCX",
            "DEMAND_DATA_CACHE_FILLS_FROM_SYSTEM_NEAR_CACHE_NEAR_FAR",
            "L3_CACHE_ACCESSES",
            "L2_CACHE_ACCESSES",
            "SCALAR_SINGLE_FLOPS",
            "PACKED_SINGLE_FLOPS",
            "SCALAR_DOUBLE_FLOPS",
            "PACKED_DOUBLE_FLOPS"};
        return native_events;
    }
    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);

}
#endif // OPTKIT_ENV_CPU_AMD