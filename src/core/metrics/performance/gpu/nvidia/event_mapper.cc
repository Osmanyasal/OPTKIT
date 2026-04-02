#include "core/metrics/performance/gpu/nvidia/event_mapper.hh"
#if OPTKIT_ENV_LIB_NVML
#include <nvml.h>
namespace optkit::metrics::performance::gpu::nvidia
{

    const std::unordered_map<performance::gpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        {performance::gpu::CoreEvents::ANY_TENSOR_UTIL, {(uint64_t)NVML_GPM_METRIC_ANY_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::DFMA_TENSOR_UTIL, {(uint64_t)NVML_GPM_METRIC_DFMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::DRAM_BW_UTIL, {(uint64_t)NVML_GPM_METRIC_DRAM_BW_UTIL}},
        {performance::gpu::CoreEvents::FP16_UTIL, {(uint64_t)NVML_GPM_METRIC_FP16_UTIL}},
        {performance::gpu::CoreEvents::FP32_UTIL, {(uint64_t)NVML_GPM_METRIC_FP32_UTIL}},
        {performance::gpu::CoreEvents::FP64_UTIL, {(uint64_t)NVML_GPM_METRIC_FP64_UTIL}},
        {performance::gpu::CoreEvents::GRAPHICS_UTIL, {(uint64_t)NVML_GPM_METRIC_GRAPHICS_UTIL}},
        {performance::gpu::CoreEvents::HMMA_TENSOR_UTIL, {(uint64_t)NVML_GPM_METRIC_HMMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::IMMA_TENSOR_UTIL, {(uint64_t)NVML_GPM_METRIC_IMMA_TENSOR_UTIL}},
        {performance::gpu::CoreEvents::INTEGER_UTIL, {(uint64_t)NVML_GPM_METRIC_INTEGER_UTIL}},
        {performance::gpu::CoreEvents::SM_OCCUPANCY, {(uint64_t)NVML_GPM_METRIC_SM_OCCUPANCY}},
        {performance::gpu::CoreEvents::SM_UTIL, {(uint64_t)NVML_GPM_METRIC_SM_UTIL}}};

    const std::unordered_map<performance::gpu::nvidia::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

        {performance::gpu::nvidia::NativeEvents::PCIE_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_PCIE_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::PCIE_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_PCIE_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_0_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_0_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_1_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_1_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_2_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_2_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_3_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_3_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_4_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_4_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_5_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_5_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_6_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_6_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVDEC_7_UTIL, {(uint64_t)NVML_GPM_METRIC_NVDEC_7_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_0_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_0_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_1_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_1_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_2_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_2_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_3_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_3_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_4_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_4_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_5_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_5_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_6_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_6_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVJPG_7_UTIL, {(uint64_t)NVML_GPM_METRIC_NVJPG_7_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVOFA_0_UTIL, {(uint64_t)NVML_GPM_METRIC_NVOFA_0_UTIL}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_TOTAL_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_TOTAL_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_TOTAL_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_TOTAL_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L0_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L0_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L0_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L0_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L1_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L1_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L1_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L1_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L2_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L2_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L2_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L2_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L3_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L3_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L3_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L3_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L4_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L4_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L4_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L4_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L5_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L5_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L5_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L5_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L6_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L6_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L6_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L6_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L7_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L7_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L7_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L7_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L8_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L8_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L8_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L8_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L9_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L9_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L9_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L9_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L10_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L10_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L10_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L10_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L11_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L11_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L11_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L11_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L12_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L12_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L12_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L12_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L13_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L13_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L13_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L13_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L14_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L14_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L14_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L14_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L15_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L15_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L15_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L15_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L16_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L16_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L16_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L16_TX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L17_RX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L17_RX_PER_SEC}},
        {performance::gpu::nvidia::NativeEvents::NVLINK_L17_TX_PER_SEC, {(uint64_t)NVML_GPM_METRIC_NVLINK_L17_TX_PER_SEC}}};
        
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
        else if (event == "PCIE_TX_PER_SEC")
            return EventMapper::get(NativeEvents::PCIE_TX_PER_SEC);
        else if (event == "PCIE_RX_PER_SEC")
            return EventMapper::get(NativeEvents::PCIE_RX_PER_SEC);
        else if (event == "NVDEC_0_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_0_UTIL);
        else if (event == "NVDEC_1_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_1_UTIL);
        else if (event == "NVDEC_2_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_2_UTIL);
        else if (event == "NVDEC_3_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_3_UTIL);
        else if (event == "NVDEC_4_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_4_UTIL);
        else if (event == "NVDEC_5_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_5_UTIL);
        else if (event == "NVDEC_6_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_6_UTIL);
        else if (event == "NVDEC_7_UTIL")
            return EventMapper::get(NativeEvents::NVDEC_7_UTIL);
        else if (event == "NVJPG_0_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_0_UTIL);
        else if (event == "NVJPG_1_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_1_UTIL);
        else if (event == "NVJPG_2_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_2_UTIL);
        else if (event == "NVJPG_3_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_3_UTIL);
        else if (event == "NVJPG_4_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_4_UTIL);
        else if (event == "NVJPG_5_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_5_UTIL);
        else if (event == "NVJPG_6_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_6_UTIL);
        else if (event == "NVJPG_7_UTIL")
            return EventMapper::get(NativeEvents::NVJPG_7_UTIL);
        else if (event == "NVOFA_0_UTIL")
            return EventMapper::get(NativeEvents::NVOFA_0_UTIL);
        else if (event == "NVLINK_TOTAL_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_TOTAL_RX_PER_SEC);
        else if (event == "NVLINK_TOTAL_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_TOTAL_TX_PER_SEC);
        else if (event == "NVLINK_L0_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L0_RX_PER_SEC);
        else if (event == "NVLINK_L0_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L0_TX_PER_SEC);
        else if (event == "NVLINK_L1_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L1_RX_PER_SEC);
        else if (event == "NVLINK_L1_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L1_TX_PER_SEC);
        else if (event == "NVLINK_L2_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L2_RX_PER_SEC);
        else if (event == "NVLINK_L2_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L2_TX_PER_SEC);
        else if (event == "NVLINK_L3_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L3_RX_PER_SEC);
        else if (event == "NVLINK_L3_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L3_TX_PER_SEC);
        else if (event == "NVLINK_L4_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L4_RX_PER_SEC);
        else if (event == "NVLINK_L4_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L4_TX_PER_SEC);
        else if (event == "NVLINK_L5_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L5_RX_PER_SEC);
        else if (event == "NVLINK_L5_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L5_TX_PER_SEC);
        else if (event == "NVLINK_L6_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L6_RX_PER_SEC);
        else if (event == "NVLINK_L6_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L6_TX_PER_SEC);
        else if (event == "NVLINK_L7_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L7_RX_PER_SEC);
        else if (event == "NVLINK_L7_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L7_TX_PER_SEC);
        else if (event == "NVLINK_L8_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L8_RX_PER_SEC);
        else if (event == "NVLINK_L8_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L8_TX_PER_SEC);
        else if (event == "NVLINK_L9_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L9_RX_PER_SEC);
        else if (event == "NVLINK_L9_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L9_TX_PER_SEC);
        else if (event == "NVLINK_L10_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L10_RX_PER_SEC);
        else if (event == "NVLINK_L10_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L10_TX_PER_SEC);
        else if (event == "NVLINK_L11_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L11_RX_PER_SEC);
        else if (event == "NVLINK_L11_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L11_TX_PER_SEC);
        else if (event == "NVLINK_L12_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L12_RX_PER_SEC);
        else if (event == "NVLINK_L12_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L12_TX_PER_SEC);
        else if (event == "NVLINK_L13_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L13_RX_PER_SEC);
        else if (event == "NVLINK_L13_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L13_TX_PER_SEC);
        else if (event == "NVLINK_L14_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L14_RX_PER_SEC);
        else if (event == "NVLINK_L14_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L14_TX_PER_SEC);
        else if (event == "NVLINK_L15_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L15_RX_PER_SEC);
        else if (event == "NVLINK_L15_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L15_TX_PER_SEC);
        else if (event == "NVLINK_L16_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L16_RX_PER_SEC);
        else if (event == "NVLINK_L16_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L16_TX_PER_SEC);
        else if (event == "NVLINK_L17_RX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L17_RX_PER_SEC);
        else if (event == "NVLINK_L17_TX_PER_SEC")
            return EventMapper::get(NativeEvents::NVLINK_L17_TX_PER_SEC);
        OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
        return {};
    }
}
#endif // OPTKIT_ENV_LIB_NVML