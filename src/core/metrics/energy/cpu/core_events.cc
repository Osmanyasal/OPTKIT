#include "core/metrics/energy/cpu/core_events.hh"

namespace optkit::metrics::energy::cpu
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        case CoreEvents::PP0:
            return "energy-cores"; // CORES
        case CoreEvents::PP1:
            return "energy-gpu"; // INTEGRATED GPU
        case CoreEvents::PACKAGE:
            return "energy-pkg"; // PP0 + PP1 + SYSTEM AGENT + LAST_LEVEL_CACHE MEMORY CONTROLLER
        case CoreEvents::PSYS:
            return "energy-psys"; // PACKAGE + eDRAM + PCH
        case CoreEvents::DRAM:
            return "energy-dram"; // DRAM DIMM 0 and DRAM DIMM 1
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}