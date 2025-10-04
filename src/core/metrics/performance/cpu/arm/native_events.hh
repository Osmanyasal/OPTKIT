#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_ARM
#include <string>
namespace optkit::metrics::cpu::arm
{
        /**
         * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
         * for CPU specific events, check their respective CoreEvents.
         */
        enum class NativeEvents
        {
                BEGIN = 0,
                L1D_CACHE_ACCESSES,
                L2_CACHE_ACCESSES,
                L3_CACHE_ACCESSES,
                STALL_FRONTEND, // No operation has been sent for execution
                STALL_BACKEND,  // No operation has been sent for execution due to the backend
                STALL_SLOT_FRONTEND,
                STALL_SLOT_BACKEND,
                STALL_SLOT,
                OP_RETIRED,
                OP_SPEC,
                STALL_FRONTEND_FLUSH,

                END,
        };

        std::string to_string(NativeEvents event);
        std::ostream &operator<<(std::ostream &os, NativeEvents event);

}
#endif // OPTKIT_ENV_CPU_ARM