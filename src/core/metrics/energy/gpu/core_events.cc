#include "core/metrics/energy/gpu/core_events.hh"

namespace optkit::metrics::energy::gpu
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::GPU:
            return "gpu"; // GPU
        case CoreEvents::MEMORY:
            return "memory"; // MEMORY
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}