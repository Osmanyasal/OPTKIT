#include "core/metrics/temperature/core_events.hh"

namespace optkit::metrics::temperature
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::CPU:
            return "CPU";
        case CoreEvents::STORAGE:
            return "STORAGE";
        case CoreEvents::CPUGPU:
            return "CPUGPU";
        case CoreEvents::GPU:
            return "GPU";
        case CoreEvents::NETWORK:
            return "NETWORK";
        case CoreEvents::MOTHERBOARD:
            return "MOTHERBOARD";
        case CoreEvents::MEMORY:
            return "MEMORY";
        case CoreEvents::USB:
            return "USB";
        case CoreEvents::AIO_COOLANT:
            return "AIO_COOLANT";
        case CoreEvents::PSU:
            return "PSU";
        case CoreEvents::BMC:
            return "BMC";
        case CoreEvents::FAN:
            return "FAN";
        case CoreEvents::BATTERY:
            return "BATTERY";
        case CoreEvents::ACPI_THERMAL:
            return "ACPI_THERMAL";
        case CoreEvents::GENERIC_I2C:
            return "GENERIC_I2C";
        case CoreEvents::SOC_PLATFORM:
            return "SOC_PLATFORM";
        case CoreEvents::ACCELERATOR:
            return "ACCELERATOR";
        case CoreEvents::UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}