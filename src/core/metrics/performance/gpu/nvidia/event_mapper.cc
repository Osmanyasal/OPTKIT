#include "core/metrics/performance/gpu/nvidia/event_mapper.hh"
#if OPTKIT_ENV_LIB_NVML
namespace optkit::metrics::performance::gpu::nvidia
{

    const std::unordered_map<performance::gpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        {performance::gpu::CoreEvents::ANY_TENSOR_UTIL, {(uint64_t)gpu::CoreEvents::ANY_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::DFMA_TENSOR_UTIL, {(uint64_t)gpu::CoreEvents::DFMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::DRAM_BW_UTIL, {(uint64_t)gpu::CoreEvents::DRAM_BW_UTIL}},
        {performance::gpu::CoreEvents::FP16_UTIL, {(uint64_t)gpu::CoreEvents::FP16_UTIL}},
        {performance::gpu::CoreEvents::FP32_UTIL, {(uint64_t)gpu::CoreEvents::FP32_UTIL}},
        {performance::gpu::CoreEvents::FP64_UTIL, {(uint64_t)gpu::CoreEvents::FP64_UTIL}},
        {performance::gpu::CoreEvents::GRAPHICS_UTIL, {(uint64_t)gpu::CoreEvents::GRAPHICS_UTIL}},
        {performance::gpu::CoreEvents::HMMA_TENSOR_UTIL, {(uint64_t)gpu::CoreEvents::HMMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::IMMA_TENSOR_UTIL, {(uint64_t)gpu::CoreEvents::IMMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::INTEGER_UTIL, {(uint64_t)gpu::CoreEvents::INTEGER_UTIL}},
        {performance::gpu::CoreEvents::SM_OCCUPANCY, {(uint64_t)gpu::CoreEvents::SM_OCCUPANCY}},
        {performance::gpu::CoreEvents::SM_UTIL, {(uint64_t)gpu::CoreEvents::SM_UTIL}}
    };

    const std::unordered_map<performance::gpu::nvidia::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

    };
    std::vector<uint64_t> EventMapper::get(std::string event)
    {
        if (event == "ANY_TENSOR_UTIL")
            return EventMapper::get(CoreEvents::ANY_TENSOR_UTIL);
        else if (event == "DFMA_TENSOR_UTIL")
            return EventMapper::get(CoreEvents::DFMA_TENSOR_UTIL);
        else if (event == "DRAM_BW_UTIL")
            return EventMapper::get(CoreEvents::DRAM_BW_UTIL);
        else if (event == "FP16_UTIL")
            return EventMapper::get(CoreEvents::FP16_UTIL);
        else if (event == "FP32_UTIL")
            return EventMapper::get(CoreEvents::FP32_UTIL);
        else if (event == "FP64_UTIL")
            return EventMapper::get(CoreEvents::FP64_UTIL);
        else if (event == "GRAPHICS_UTIL")
            return EventMapper::get(CoreEvents::GRAPHICS_UTIL);
        else if (event == "HMMA_TENSOR_UTIL")
            return EventMapper::get(CoreEvents::HMMA_TENSOR_UTIL);
        else if (event == "IMMA_TENSOR_UTIL")
            return EventMapper::get(CoreEvents::IMMA_TENSOR_UTIL);
        else if (event == "INTEGER_UTIL")
            return EventMapper::get(CoreEvents::INTEGER_UTIL);
        else if (event == "SM_OCCUPANCY")
            return EventMapper::get(CoreEvents::SM_OCCUPANCY);
        else if (event == "SM_UTIL")
            return EventMapper::get(CoreEvents::SM_UTIL);
        OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
        return {};
    }
}
#endif // OPTKIT_ENV_CPU_ARM