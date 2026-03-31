#pragma once

#include <string>
#include <vector>
namespace optkit::metrics::performance::gpu
{
    /**
     * @brief These are the core events to be monitored for metrics. It is expected to exists in every CPUs
     * for CPU specific events, check their respective CoreEvents.
     */
    enum class CoreEvents
    {
        BEGIN = 0,
        GRAPHICS_UTIL,
        SM_UTIL,
        SM_OCCUPANCY,
        INTEGER_UTIL,
        ANY_TENSOR_UTIL,
        DFMA_TENSOR_UTIL,
        HMMA_TENSOR_UTIL,
        IMMA_TENSOR_UTIL,
        DRAM_BW_UTIL,
        FP64_UTIL,
        FP32_UTIL,
        FP16_UTIL,
        END,
    };

    const std::vector<std::string> &get_core_events();
    std::string to_string(CoreEvents event);
    std::ostream &operator<<(std::ostream &os, CoreEvents event);

}