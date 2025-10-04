#include "core/metrics/energy/core_events.hh"

namespace optkit::metrics::energy
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::PP0:
            return "PP0"; // CORES
        case CoreEvents::PP1:
            return "PP1"; // INTEGRATED GPU
        case CoreEvents::PACKAGE:
            return "PACKAGE"; // PP0 + PP1 + SYSTEM AGENT + LAST_LEVEL_CACHE MEMORY CONTROLLER
        case CoreEvents::PSYS:
            return "PSYS"; // PACKAGE + eDRAM + PCH
        case CoreEvents::DRAM:
            return "DRAM"; // DRAM DIMM 0 and DRAM DIMM 1
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}