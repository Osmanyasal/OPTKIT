#include "core/metrics/disk/core_events.hh"

namespace optkit::metrics::disk
{
    std::string to_string(CoreEvents event)
    {
        switch (event)
        {

        case CoreEvents::RCHAR:
            return "rchar";
        case CoreEvents::WCHAR:
            return "wchar";
        case CoreEvents::SYSCR:
            return "syscr";
        case CoreEvents::SYSCW:
            return "syscw";
        case CoreEvents::READ_BYTES:
            return "read_bytes";
        case CoreEvents::WRITE_BYTES:
            return "write_bytes";
        case CoreEvents::CANCELLED_WRITE_BYTES:
            return "cancelled_write_bytes";
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}