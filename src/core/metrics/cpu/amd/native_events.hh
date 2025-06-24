#pragma once

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
        DISPATCH_STALLS_1,
        BACKEND_STALLS_1,
        SMT_STALLS_1,
        OPS_SOURCE_DISPATCHED_FROM_DECODER,

        END,
    };

    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);

}