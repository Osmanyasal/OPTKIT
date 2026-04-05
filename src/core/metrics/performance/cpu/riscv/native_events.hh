#pragma once

#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_RISCV

#include <string>
#include <vector>

namespace optkit::metrics::performance::cpu::riscv
{
    enum class NativeEvents
    {
        BEGIN = 0,
        LLC_LOAD_MISSES,
        LLC_STORE_MISSES,
        END,
    };

    static const std::vector<std::string> &get_native_events()
    {
        static const std::vector<std::string> native_events{
            "LLC-load-misses",
            "LLC-store-misses",
        };
        return native_events;
    }

    std::string to_string(NativeEvents event);
    std::ostream &operator<<(std::ostream &os, NativeEvents event);
}

#endif // OPTKIT_ENV_CPU_RISCV