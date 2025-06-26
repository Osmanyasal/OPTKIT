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
        
        
        END,
    };

    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);

}