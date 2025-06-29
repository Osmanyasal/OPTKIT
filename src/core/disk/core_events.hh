#pragma once

#include <string>
namespace optkit::core::disk
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class CoreEvents
    {
        BEGIN = 0,
        RCHAR,
        WCHAR,
        SYSCR,
        SYSCW,
        READ_BYTES,
        WRITE_BYTES,
        CANCELLED_WRITE_BYTES,
        END,
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);

}