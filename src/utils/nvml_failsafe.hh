#pragma once
#if OPTKIT_ENV_LIB_NVML
// do these ifndefs exist, to avoid redefinition warnings
#ifndef NVML_DEVICE_ARCH_KEPLER
#define NVML_DEVICE_ARCH_KEPLER 2 // Devices based on the NVIDIA Kepler architecture
#endif

#ifndef NVML_DEVICE_ARCH_MAXWELL
#define NVML_DEVICE_ARCH_MAXWELL 3 // Devices based on the NVIDIA Maxwell architecture
#endif

#ifndef NVML_DEVICE_ARCH_PASCAL
#define NVML_DEVICE_ARCH_PASCAL 4 // Devices based on the NVIDIA Pascal architecture
#endif

#ifndef NVML_DEVICE_ARCH_VOLTA
#define NVML_DEVICE_ARCH_VOLTA 5 // Devices based on the NVIDIA Volta architecture
#endif

#ifndef NVML_DEVICE_ARCH_TURING
#define NVML_DEVICE_ARCH_TURING 6 // Devices based on the NVIDIA Turing architecture
#endif

#ifndef NVML_DEVICE_ARCH_AMPERE
#define NVML_DEVICE_ARCH_AMPERE 7 // Devices based on the NVIDIA Ampere architecture
#endif

#ifndef NVML_DEVICE_ARCH_ADA
#define NVML_DEVICE_ARCH_ADA 8 // Devices based on the NVIDIA Ada architecture
#endif

#ifndef NVML_DEVICE_ARCH_HOPPER
#define NVML_DEVICE_ARCH_HOPPER 9 // Devices based on the NVIDIA Hopper architecture
#endif

#ifndef NVML_DEVICE_ARCH_BLACKWELL
#define NVML_DEVICE_ARCH_BLACKWELL 10 // Devices based on the NVIDIA Blackwell architecture
#endif

#ifndef NVML_DEVICE_ARCH_T23X
#define NVML_DEVICE_ARCH_T23X 11 // Devices based on NVIDIA Orin architecture
#endif

#ifndef NVML_DEVICE_ARCH_UNKNOWN
#define NVML_DEVICE_ARCH_UNKNOWN 0xFFFFFFFF // Unknown architecture
#endif

// Define nvmlDeviceAttributes_t if it doesn't exist in older NVML versions
#if !defined(NVML_DEVICE_ATTRIBUTES_T_DEFINED) && !defined(nvmlDeviceAttributes_t)
#define NVML_DEVICE_ATTRIBUTES_T_DEFINED
typedef struct nvmlDeviceAttributes_st
{
    unsigned int multiprocessorCount;       //!< Streaming Multiprocessor count
    unsigned int sharedCopyEngineCount;     //!< Shared Copy Engine count
    unsigned int sharedDecoderCount;        //!< Shared Decoder Engine count
    unsigned int sharedEncoderCount;        //!< Shared Encoder Engine count
    unsigned int sharedJpegCount;           //!< Shared JPEG Engine count
    unsigned int sharedOfaCount;            //!< Shared OFA Engine count
    unsigned int gpuInstanceSliceCount;     //!< GPU instance slice count
    unsigned int computeInstanceSliceCount; //!< Compute instance slice count
    unsigned long long memorySizeMB;        //!< Device memory size (in MiB)
} nvmlDeviceAttributes_t;
#endif

#endif