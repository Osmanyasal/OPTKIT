#pragma once

#include <string>
#include <ostream>

namespace optkit::metrics::energy::gpu
{
    // Extended CoreEvents (BEGIN/END sentinels preserved)
    enum class CoreEvents
    {
        BEGIN = 0,
        GPU,
        MEMORY,
        END
    };

    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);
}