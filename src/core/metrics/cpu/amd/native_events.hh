#pragma once

#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_AMD
#include <string>
namespace optkit::core::metrics::cpu::amd
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class NativeEvents
    {
        BEGIN = 0,
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
        L3_CACHE_ACCESSES,
        L2_CACHE_ACCESSES,
        END,
    };

    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);

}
#endif // OPTKIT_ENV_CPU_AMD