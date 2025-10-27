#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <algorithm>
#include "utils/optimizations/cpu_opt.hh"
#include <vector>
#include <dlfcn.h>
#include <unordered_map>

#if OPTKIT_ENV_LIB_NVML
#include <nvml.h>
#include "utils/nvml_failsafe.hh"

using nvml_fn_t = nvmlReturn_t (*)(nvmlDevice_t, ...);
OPT_FORCE_INLINE nvml_fn_t query_nvml_fn(const char *function_name)
{
    static void *lib = dlopen("libnvidia-ml.so", RTLD_NOW | RTLD_NOLOAD);
    return lib ? (nvml_fn_t)dlsym(lib, function_name) : nullptr;
}

// Generic runtime NVML call macro with fallback
// method name, device handle, result variable, arguments of the method...
#define NVML_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...) \
    do                                                       \
    {                                                        \
        static nvml_fn_t fn = query_nvml_fn(NAME_STR);       \
        if (OPT_LIKELY(fn))                                  \
            RESULT = fn(DEVICE, __VA_ARGS__);                \
        else                                                 \
            RESULT = NVML_ERROR_NOT_SUPPORTED;               \
    } while (0)

#ifndef NVML_CUDA_DRIVER_VERSION_MAJOR
#define NVML_CUDA_DRIVER_VERSION_MAJOR(v) (((int32_t)(v)) / 1000)
#endif

#ifndef NVML_CUDA_DRIVER_VERSION_MINOR
#define NVML_CUDA_DRIVER_VERSION_MINOR(v) (((int32_t)(v) % 1000) / 10)
#endif

#else
#define NVML_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...)
#define NVML_CUDA_DRIVER_VERSION_MAJOR(v) 0
#define NVML_CUDA_DRIVER_VERSION_MINOR(v) 0
#endif

#if OPTKIT_ENV_LIB_AMDSMI || OPTKIT_ENV_LIB_ROCM_SMI
#include "utils/amd_failsafe.hh"

#if OPTKIT_ENV_LIB_AMDSMI
#include <amd_smi/amdsmi.h>

using amdsmi_fn_t = amdsmi_status_t (*)(amdsmi_processor_handle, ...);
OPT_FORCE_INLINE amdsmi_fn_t query_amd_smi_fn(const char *function_name)
{
    static void *lib = dlopen("libamd_smi.so", RTLD_NOW | RTLD_NOLOAD);
    return lib ? (amdsmi_fn_t)dlsym(lib, function_name) : nullptr;
}

#define ROCM_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...) \
    do                                                       \
    {                                                        \
        static amdsmi_fn_t fn = query_amd_smi_fn(NAME_STR);  \
        if (fn)                                              \
            RESULT = fn(DEVICE, __VA_ARGS__);                \
        else                                                 \
            RESULT = AMDSMI_STATUS_NOT_SUPPORTED;            \
    } while (0)

#elif OPTKIT_ENV_LIB_ROCM_SMI
#include <rocm_smi/rocm_smi.h>

using rsmi_fn_t = rsmi_status_t (*)(uint32_t, ...);
OPT_FORCE_INLINE rsmi_fn_t query_rocm_smi_fn(const char *function_name)
{
    static void *lib = dlopen("librocm_smi64.so", RTLD_NOW | RTLD_NOLOAD);
    return lib ? (rsmi_fn_t)dlsym(lib, function_name) : nullptr;
}

#define ROCM_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...) \
    do                                                       \
    {                                                        \
        static rsmi_fn_t fn = query_rocm_smi_fn(NAME_STR);   \
        if (fn)                                              \
            RESULT = fn(DEVICE, __VA_ARGS__);                \
        else                                                 \
            RESULT = RSMI_STATUS_NOT_SUPPORTED;              \
    } while (0)
#endif

#else
#define ROCM_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...)
#endif

// vendor IDs from PCI-SIG

#define VENDOR_ID_INTEL 0x8086
#define VENDOR_ID_AMD 0x1002
#define VENDOR_ID_NVIDIA 0x10DE
#define VENDOR_ID_ARM 0x13B5
#define VENDOR_ID_QUALCOMM 0x5143
namespace optkit::gpu
{
    /**
     * @brief GPU vendors for power monitoring
     */
    enum class GpuVendor : uint8_t
    {
        BEGIN = 0,
        NVIDIA = 1,
        AMD = 2,
        INTEL = 3,
        ARM_MALI = 4,
        QUALCOMM_ADRENO = 5,
        IMAGINATION_POWERVR = 6,
        END,
        UNKNOWN,
    };

    /**
     * @brief GPU power measurement methods
     */
    enum class GpuPowerMethod : uint8_t
    {
        NONE = 0,
        NVIDIA_ML = (1 << 0),   // NVIDIA Management Library
        AMD_ROCM = (1 << 1),    // AMD ROCm sysfs interfaces
        INTEL_I915 = (1 << 2),  // Intel i915 driver sysfs
        SYSFS_HWMON = (1 << 3), // Generic sysfs hwmon
        PERF_UNCORE = (1 << 4), // perf uncore events (Intel)
        ALL = NVIDIA_ML | AMD_ROCM | INTEL_I915 | SYSFS_HWMON | PERF_UNCORE
    };

    /**
     * @brief Comprehensive GPU device information using composition
     */
    /**
     * @brief Basic GPU device identification information
     */
    struct GpuBasicInfo
    {
        uint32_t id;
        std::string device_name;
        GpuVendor vendor;
        uint32_t architecture;
        std::string vendor_string;
        bool is_integrated;
    };

    /**
     * @brief GPU driver and library version information
     */
    struct GpuVersionInfo
    {
        double driver_major_minor;
        std::string driver_version_string;
        std::string library_version_string;
    };

    /**
     * @brief GPU compute capability and architecture information
     */
    struct GpuComputeInfo
    {
        int32_t compute_capability_major;
        int32_t compute_capability_minor;
        uint32_t multiprocessor_count;
        uint32_t cores_per_mp;
        uint32_t total_cores;
        uint32_t warp_size;
    };

    /**
     * @brief GPU memory information
     */
    struct GpuMemoryInfo
    {
        uint64_t total_global_memory_MBytes;
        uint64_t free_memory_MBytes;
        uint64_t used_memory_MBytes;
        uint32_t memory_bus_width_bits;
        double memory_utilization_percent;
    };

    /**
     * @brief GPU clock and frequency information
     */
    struct GpuClockInfo
    {
        uint32_t current_sm_clock_MHz;
        uint32_t current_video_clock_MHz;
        uint32_t current_graphics_clock_MHz;
        uint32_t current_memory_clock_MHz;

        uint32_t max_sm_clock_MHz;
        uint32_t max_video_clock_MHz;
        uint32_t max_graphics_clock_MHz;
        uint32_t max_memory_clock_MHz;

        uint32_t min_sm_clock_MHz;
        uint32_t min_video_clock_MHz;
        uint32_t min_graphics_clock_MHz;
        uint32_t min_memory_clock_MHz;

        std::vector<uint32_t> memory_supported_clock_rates_MHz;
        std::unordered_map<uint32_t, std::vector<uint32_t>> graphics_supported_clock_rates_MHz; // for each memory clock, what are the supported graphics clocks
        bool has_frequency_control;
    };

    /**
     * @brief GPU power monitoring information
     */
    struct GpuPowerInfo
    {
        double current_power_watts;
        double power_limit_watts;
        double min_power_watts;
        double max_power_watts;
        double default_power_watts;
        bool has_power_monitoring;
        bool is_configurable;
    };

    /**
     * @brief GPU temperature monitoring information
     */
    struct GpuTemperatureInfo
    {
        double current_device_temperature_celsius;
        double current_memory_temperature_celsius;
        double max_device_temperature_celsius;
        double max_memory_temperature_celsius;
        double min_device_temperature_celsius;
        double min_memory_temperature_celsius;
        bool has_temperature_monitoring;
    };

    /**
     * @brief GPU utilization and performance information
     */
    struct GpuUtilizationInfo
    {
        double gpu_utilization_percent;
        double memory_utilization_percent;
        bool has_utilization_monitoring;
    };

    /**
     * @brief GPU PCI and hardware information
     */
    struct GpuHardwareInfo
    {
        std::string pci_bus_id;
        uint32_t pci_device_id;
        uint32_t pci_subsystem_id;
        uint32_t board_id;
        bool multi_gpu_board;
    };

    /**
     * @brief GPU advanced features and capabilities
     */
    struct GpuCapabilitiesInfo
    {
        bool ecc_enabled;
        bool supports_unified_memory;
        bool persistence_mode_enabled;
    };
    struct GpuDeviceInfo
    {
        GpuBasicInfo basic;
        GpuVersionInfo version;
        GpuComputeInfo compute;
        GpuMemoryInfo memory;
        GpuClockInfo clocks;
        GpuPowerInfo power;
        GpuTemperatureInfo temperature;
        GpuUtilizationInfo utilization;
        GpuHardwareInfo hardware;
        GpuCapabilitiesInfo capabilities;
    };

    GpuVendor vendor_from_string(const std::string &vendor_name);
    std::string to_string(GpuVendor vendor);
    std::string to_string(const GpuPowerMethod &method);
    std::string to_string(const GpuBasicInfo &basic_info);
    std::string to_string(const GpuVersionInfo &version_info);
    std::string to_string(const GpuComputeInfo &compute_info);
    std::string to_string(const GpuMemoryInfo &memory_info);
    std::string to_string(const GpuClockInfo &clk_info);
    std::string to_string(const GpuPowerInfo &power_info);
    std::string to_string(const GpuTemperatureInfo &temperature_info);
    std::string to_string(const GpuUtilizationInfo &utilization_info);
    std::string to_string(const GpuHardwareInfo &hardware_info);
    std::string to_string(const GpuCapabilitiesInfo &capabilities_info);
    std::string to_string(const GpuDeviceInfo &device_info);

    std::ostream &operator<<(std::ostream &os, GpuVendor vendor);
    std::ostream &operator<<(std::ostream &os, const GpuPowerMethod &method);
    std::ostream &operator<<(std::ostream &os, const GpuBasicInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuVersionInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuComputeInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuMemoryInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuClockInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuPowerInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuTemperatureInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuUtilizationInfo &utilization_info);
    std::ostream &operator<<(std::ostream &os, const GpuHardwareInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuCapabilitiesInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuDeviceInfo &info);
} // namespace optkit::gpu

using optkit::gpu::operator<<; // make available to global namespace