#include "core/metrics/cpu/intel/native_events.hh"

namespace optkit::core::metrics::cpu::intel
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {

            default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}