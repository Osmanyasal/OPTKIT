#include "core/metrics/temperature/core_events.hh"

namespace optkit::core::temperature
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::ALL:
            return "all";
        case CoreEvents::CPU:
            return "cpu";
        case CoreEvents::STORAGE:
            return "storage";
        case CoreEvents::CPUGPU:
            return "cpugpu";
        case CoreEvents::GPU:
            return "gpu";
        case CoreEvents::MOTHERBOARD:
            return "motherboard";
        case CoreEvents::NETWORK:
            return "network";
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}