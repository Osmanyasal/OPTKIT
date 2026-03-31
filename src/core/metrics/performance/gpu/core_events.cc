#include "core/metrics/performance/gpu/core_events.hh"

namespace optkit::metrics::performance::gpu
{

    const std::vector<std::string> &get_core_events()
    {
        static std::vector<std::string> core_events{
            "GRAPHICS_UTIL",
            "SM_UTIL",
            "SM_OCCUPANCY",
            "INTEGER_UTIL",
            "ANY_TENSOR_UTIL",
            "DFMA_TENSOR_UTIL",
            "HMMA_TENSOR_UTIL",
            "IMMA_TENSOR_UTIL",
            "DRAM_BW_UTIL",
            "FP64_UTIL",
            "FP32_UTIL",
            "FP16_UTIL",
        };
        return core_events;
    }

    std::string to_string(CoreEvents event)
    {
        switch (event)
        {
        // Pipeline and Stalls
        case CoreEvents::GRAPHICS_UTIL:
            return "GRAPHICS_UTIL";
        case CoreEvents::SM_UTIL:
            return "SM_UTIL";
        case CoreEvents::SM_OCCUPANCY:
            return "SM_OCCUPANCY";
        case CoreEvents::INTEGER_UTIL:
            return "INTEGER_UTIL";
        case CoreEvents::ANY_TENSOR_UTIL:
            return "ANY_TENSOR_UTIL";
        case CoreEvents::DFMA_TENSOR_UTIL:
            return "DFMA_TENSOR_UTIL";
        case CoreEvents::HMMA_TENSOR_UTIL:
            return "HMMA_TENSOR_UTIL";
        case CoreEvents::IMMA_TENSOR_UTIL:
            return "IMMA_TENSOR_UTIL";
        case CoreEvents::DRAM_BW_UTIL:
            return "DRAM_BW_UTIL";
        case CoreEvents::FP64_UTIL:
            return "FP64_UTIL";
        case CoreEvents::FP32_UTIL:
            return "FP32_UTIL";
        case CoreEvents::FP16_UTIL:
            return "FP16_UTIL";

        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, CoreEvents event)
    {
        return os << to_string(event);
    }
}