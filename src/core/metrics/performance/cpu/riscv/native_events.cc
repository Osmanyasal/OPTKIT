#include "core/metrics/performance/cpu/riscv/native_events.hh"

#if OPTKIT_ENV_CPU_RISCV
namespace optkit::metrics::performance::cpu::riscv
{
    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
        case NativeEvents::LLC_LOAD:
            return "LLC-load";
        case NativeEvents::LLC_STORE:
            return "LLC-store";
        case NativeEvents::LLC_LOAD_MISSES:
            return "LLC-load-misses";
        case NativeEvents::LLC_STORE_MISSES:
            return "LLC-store-misses";
        default:
            return "UNKNOWN_NATIVE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}
#endif // OPTKIT_ENV_CPU_RISCV