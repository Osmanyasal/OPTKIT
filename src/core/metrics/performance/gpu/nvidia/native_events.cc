#include "core/metrics/performance/gpu/nvidia/native_events.hh"
#if OPTKIT_ENV_LIB_NVML
namespace optkit::metrics::performance::gpu::nvidia
{

    std::string to_string(NativeEvents event)
    {
        switch (event)
        {
            
                case NativeEvents::PCIE_TX_PER_SEC:
                    return "PCIE_TX_PER_SEC";
                case NativeEvents::PCIE_RX_PER_SEC:
                    return "PCIE_RX_PER_SEC";
                case NativeEvents::NVDEC_0_UTIL:
                    return "NVDEC_0_UTIL";
                case NativeEvents::NVDEC_1_UTIL:
                    return "NVDEC_1_UTIL";
                case NativeEvents::NVDEC_2_UTIL:
                    return "NVDEC_2_UTIL";
                case NativeEvents::NVDEC_3_UTIL:
                    return "NVDEC_3_UTIL";
                case NativeEvents::NVDEC_4_UTIL:
                    return "NVDEC_4_UTIL";
                case NativeEvents::NVDEC_5_UTIL:
                    return "NVDEC_5_UTIL";
                case NativeEvents::NVDEC_6_UTIL:
                    return "NVDEC_6_UTIL";
                case NativeEvents::NVDEC_7_UTIL:
                    return "NVDEC_7_UTIL";
                case NativeEvents::NVJPG_0_UTIL:
                    return "NVJPG_0_UTIL";
                case NativeEvents::NVJPG_1_UTIL:
                    return "NVJPG_1_UTIL";
                case NativeEvents::NVJPG_2_UTIL:
                    return "NVJPG_2_UTIL";
                case NativeEvents::NVJPG_3_UTIL:
                    return "NVJPG_3_UTIL";
                case NativeEvents::NVJPG_4_UTIL:
                    return "NVJPG_4_UTIL";
                case NativeEvents::NVJPG_5_UTIL:
                    return "NVJPG_5_UTIL";
                case NativeEvents::NVJPG_6_UTIL:
                    return "NVJPG_6_UTIL";
                case NativeEvents::NVJPG_7_UTIL:    
                    return "NVJPG_7_UTIL";
                case NativeEvents::NVOFA_0_UTIL:
                    return "NVOFA_0_UTIL";
                case NativeEvents::NVLINK_TOTAL_RX_PER_SEC:
                    return "NVLINK_TOTAL_RX_PER_SEC";
                case NativeEvents::NVLINK_TOTAL_TX_PER_SEC:
                    return "NVLINK_TOTAL_TX_PER_SEC";
                case NativeEvents::NVLINK_L0_RX_PER_SEC:
                    return "NVLINK_L0_RX_PER_SEC";
                case NativeEvents::NVLINK_L0_TX_PER_SEC:
                    return "NVLINK_L0_TX_PER_SEC";  
                case NativeEvents::NVLINK_L1_RX_PER_SEC:
                    return "NVLINK_L1_RX_PER_SEC";
                case NativeEvents::NVLINK_L1_TX_PER_SEC:
                    return "NVLINK_L1_TX_PER_SEC";
                case NativeEvents::NVLINK_L2_RX_PER_SEC:
                    return "NVLINK_L2_RX_PER_SEC";
                case NativeEvents::NVLINK_L2_TX_PER_SEC:
                    return "NVLINK_L2_TX_PER_SEC";
                case NativeEvents::NVLINK_L3_RX_PER_SEC:
                    return "NVLINK_L3_RX_PER_SEC";
                case NativeEvents::NVLINK_L3_TX_PER_SEC:
                    return "NVLINK_L3_TX_PER_SEC";
                case NativeEvents::NVLINK_L4_RX_PER_SEC:
                    return "NVLINK_L4_RX_PER_SEC";
                case NativeEvents::NVLINK_L4_TX_PER_SEC:
                    return "NVLINK_L4_TX_PER_SEC";
                case NativeEvents::NVLINK_L5_RX_PER_SEC:
                    return "NVLINK_L5_RX_PER_SEC";
                case NativeEvents::NVLINK_L5_TX_PER_SEC:
                    return "NVLINK_L5_TX_PER_SEC";
                case NativeEvents::NVLINK_L6_RX_PER_SEC:
                    return "NVLINK_L6_RX_PER_SEC";
                case NativeEvents::NVLINK_L6_TX_PER_SEC:
                    return "NVLINK_L6_TX_PER_SEC";
                case NativeEvents::NVLINK_L7_RX_PER_SEC:
                    return "NVLINK_L7_RX_PER_SEC";
                case NativeEvents::NVLINK_L7_TX_PER_SEC:
                    return "NVLINK_L7_TX_PER_SEC";
                case NativeEvents::NVLINK_L8_RX_PER_SEC:
                    return "NVLINK_L8_RX_PER_SEC";  
                case NativeEvents::NVLINK_L8_TX_PER_SEC:
                    return "NVLINK_L8_TX_PER_SEC";
                case NativeEvents::NVLINK_L9_RX_PER_SEC:
                    return "NVLINK_L9_RX_PER_SEC";
                case NativeEvents::NVLINK_L9_TX_PER_SEC:    
                    return "NVLINK_L9_TX_PER_SEC";  
                case NativeEvents::NVLINK_L10_RX_PER_SEC:
                    return "NVLINK_L10_RX_PER_SEC";
                case NativeEvents::NVLINK_L10_TX_PER_SEC:
                    return "NVLINK_L10_TX_PER_SEC";
                case NativeEvents::NVLINK_L11_RX_PER_SEC:       
                    return "NVLINK_L11_RX_PER_SEC";
                case NativeEvents::NVLINK_L11_TX_PER_SEC:
                    return "NVLINK_L11_TX_PER_SEC";
                case NativeEvents::NVLINK_L12_RX_PER_SEC:
                    return "NVLINK_L12_RX_PER_SEC";
                case NativeEvents::NVLINK_L12_TX_PER_SEC:
                    return "NVLINK_L12_TX_PER_SEC";
                case NativeEvents::NVLINK_L13_RX_PER_SEC:
                    return "NVLINK_L13_RX_PER_SEC";
                case NativeEvents::NVLINK_L13_TX_PER_SEC:
                    return "NVLINK_L13_TX_PER_SEC";
                case NativeEvents::NVLINK_L14_RX_PER_SEC:
                    return "NVLINK_L14_RX_PER_SEC";
                case NativeEvents::NVLINK_L14_TX_PER_SEC:
                    return "NVLINK_L14_TX_PER_SEC";
                case NativeEvents::NVLINK_L15_RX_PER_SEC:
                    return "NVLINK_L15_RX_PER_SEC";
                case NativeEvents::NVLINK_L15_TX_PER_SEC:
                    return "NVLINK_L15_TX_PER_SEC";
                case NativeEvents::NVLINK_L16_RX_PER_SEC:
                    return "NVLINK_L16_RX_PER_SEC";
                case NativeEvents::NVLINK_L16_TX_PER_SEC:
                    return "NVLINK_L16_TX_PER_SEC";
                case NativeEvents::NVLINK_L17_RX_PER_SEC:
                    return "NVLINK_L17_RX_PER_SEC";
                case NativeEvents::NVLINK_L17_TX_PER_SEC:
                    return "NVLINK_L17_TX_PER_SEC";
                
        default:
            return "UNKNOWN_CORE_EVENT";
        }
    }

    std::ostream &operator<<(std::ostream &os, NativeEvents event)
    {
        return os << to_string(event);
    }
}
#endif // OPTKIT_ENV_LIB_NVML