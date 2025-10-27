
#pragma once
#include <string>
#include "utils/optimizations/cpu_opt.hh"

// AMD GPU architecture constants for AMD - define them if not available
#ifndef AMD_DEVICE_ARCH_GCN_1_0
#define AMD_DEVICE_ARCH_GCN_1_0 1 // Southern Islands (HD 7000 series)
#endif

#ifndef AMD_DEVICE_ARCH_GCN_2_0
#define AMD_DEVICE_ARCH_GCN_2_0 2 // Sea Islands (R9 200/300 series)
#endif

#ifndef AMD_DEVICE_ARCH_GCN_3_0
#define AMD_DEVICE_ARCH_GCN_3_0 3 // Volcanic Islands (R9 Fury series)
#endif

#ifndef AMD_DEVICE_ARCH_GCN_4_0
#define AMD_DEVICE_ARCH_GCN_4_0 4 // Arctic Islands - Polaris (RX 400/500 series)
#endif

#ifndef AMD_DEVICE_ARCH_GCN_5_0
#define AMD_DEVICE_ARCH_GCN_5_0 5 // Vega (RX Vega series, MI50/60)
#endif

#ifndef AMD_DEVICE_ARCH_RDNA_1_0
#define AMD_DEVICE_ARCH_RDNA_1_0 6 // Navi 10/14 (RX 5000 series)
#endif

#ifndef AMD_DEVICE_ARCH_RDNA_2_0
#define AMD_DEVICE_ARCH_RDNA_2_0 7 // Navi 21/22/23/24 (RX 6000 series)
#endif

#ifndef AMD_DEVICE_ARCH_RDNA_3_0
#define AMD_DEVICE_ARCH_RDNA_3_0 8 // Navi 31/32/33 (RX 7000 series)
#endif

#ifndef AMD_DEVICE_ARCH_CDNA_1_0
#define AMD_DEVICE_ARCH_CDNA_1_0 9 // MI100 (Arcturus)
#endif

#ifndef AMD_DEVICE_ARCH_CDNA_2_0
#define AMD_DEVICE_ARCH_CDNA_2_0 10 // MI200 series (Aldebaran)
#endif

#ifndef AMD_DEVICE_ARCH_CDNA_3_0
#define AMD_DEVICE_ARCH_CDNA_3_0 11 // MI300 series (Aqua Vanjaram)
#endif

#ifndef AMD_DEVICE_ARCH_UNKNOWN
#define AMD_DEVICE_ARCH_UNKNOWN 0xFFFFFFFF // Unknown AMD architecture
#endif

inline uint32_t _map_amd_device_id_to_arch(uint32_t device_id)
{
    // Common AMD device IDs to architecture mapping
    switch (device_id)
    {
    // GCN 1.0 (Southern Islands)
    case 0x6780:
    case 0x6784:
    case 0x6788:
    case 0x678A:
        return AMD_DEVICE_ARCH_GCN_1_0;

    // GCN 2.0 (Sea Islands)
    case 0x6610:
    case 0x6611:
    case 0x6613:
    case 0x6617:
        return AMD_DEVICE_ARCH_GCN_2_0;

    // GCN 3.0 (Volcanic Islands - Fiji)
    case 0x7300:
    case 0x730F:
        return AMD_DEVICE_ARCH_GCN_3_0;

    // GCN 4.0 (Polaris)
    case 0x67DF:
    case 0x67C0:
    case 0x67C4:
    case 0x67C7:
    case 0x67EF:
    case 0x67FF:
    case 0x6FDF:
        return AMD_DEVICE_ARCH_GCN_4_0;

    // GCN 5.0 (Vega)
    case 0x687F:
    case 0x6863:
    case 0x6862:
    case 0x66A0:
    case 0x66A1:
    case 0x66A2:
    case 0x66A3:
    case 0x66AF:
        return AMD_DEVICE_ARCH_GCN_5_0;

    // RDNA 1.0 (Navi 10/14)
    case 0x7310:
    case 0x7312:
    case 0x7318:
    case 0x7319:
    case 0x731A:
    case 0x731B:
    case 0x731E:
    case 0x731F:
        return AMD_DEVICE_ARCH_RDNA_1_0;

    // RDNA 2.0 (Navi 21/22/23/24)
    case 0x73A0:
    case 0x73A1:
    case 0x73A2:
    case 0x73A3:
    case 0x73AB:
    case 0x73AE:
    case 0x73AF:
    case 0x73BF:
    case 0x7420:
    case 0x7421:
    case 0x7422:
    case 0x7423:
    case 0x7424:
    case 0x743F:
        return AMD_DEVICE_ARCH_RDNA_2_0;

    // RDNA 3.0 (Navi 31/32/33)
    case 0x744C:
    case 0x7448:
    case 0x7479:
        return AMD_DEVICE_ARCH_RDNA_3_0;

    // CDNA 1.0 (MI100 - Arcturus)
    case 0x738C:
    case 0x738E:
        return AMD_DEVICE_ARCH_CDNA_1_0;

    // CDNA 2.0 (MI200 series - Aldebaran)
    case 0x740C:
    case 0x740F:
        return AMD_DEVICE_ARCH_CDNA_2_0;

    // CDNA 3.0 (MI300 series)
    case 0x74A0:
    case 0x74A1:
        return AMD_DEVICE_ARCH_CDNA_3_0;

    default:
        return AMD_DEVICE_ARCH_UNKNOWN;
    }
}
#if OPTKIT_ENV_LIB_ROCM_SMI

OPT_FORCE_INLINE std::string _rocm_smi_status_to_string(rsmi_status_t status)
{
    const char *status_str = nullptr;
    if (rsmi_status_string(status, &status_str) == RSMI_STATUS_SUCCESS && status_str)
    {
        return std::string(status_str);
    }
    return "Unknown error";
}
#elif OPTKIT_ENV_LIB_AMD

OPT_FORCE_INLINE std::string _amdsmi_status_to_string(amdsmi_status_t status)
{
    const char *status_str = nullptr;
    if (amdsmi_status_code_to_string(status, &status_str) == AMD_STATUS_SUCCESS && status_str)
    {
        return std::string(status_str);
    }
    return "Unknown error";
}

#endif