#pragma once

#include <string>
#include <ostream>

namespace optkit::core::temperature
{
    enum class CoreEvents
    {
        BEGIN = 0,

        // All available sensors
        ALL,
        // CPU Temperature Sensors
        CPU,
        // Storage Temperature
        STORAGE,
        // CPU's GPU Temperature
        CPUGPU,
        // external GPU Temperature
        GPU,
        // Network Temperature
        NETWORK,
        // Motherboard Temperature
        MOTHERBOARD,

        END,
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);
}