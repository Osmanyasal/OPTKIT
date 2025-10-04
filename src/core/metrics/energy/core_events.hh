#pragma once

#include <string>
#include <ostream>

namespace optkit::metrics::energy
{
    // Extended CoreEvents (BEGIN/END sentinels preserved)
    enum class CoreEvents
    {
        BEGIN = 0,

        PP0,     // CORES
        PP1,     // INTEGRATED GPU
        PACKAGE, // PP0 + PP1 + SYSTEM AGENT + LAST_LEVEL_CACHE MEMORY CONTROLLER
        PSYS,    // PACKAGE + eDRAM + PCH
        DRAM,    // DRAM DIMM 0 and DRAM DIMM 1

        END
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);
}