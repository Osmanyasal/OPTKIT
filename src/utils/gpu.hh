#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include "utils/optimizations/cpu_opt.hh"

#if OPTKIT_ENV_LIB_NVML
#include <nvml.h>
#endif

#if OPTKIT_ENV_LIB_ROCM_SMI
#include <rocm_smi.h>
#endif

#include <dlfcn.h>

using fn_t = nvmlReturn_t (*)(nvmlDevice_t, ...);
OPT_FORCE_INLINE fn_t query_nvml_fn(const char *function_name)
{
    static void *lib = dlopen("libnvidia-ml.so", RTLD_NOW | RTLD_NOLOAD);
    return lib ? (fn_t)dlsym(lib, function_name) : nullptr;
}

// Generic runtime NVML call macro with fallback
// method name, device handle, result variable, arguments of the method...
#define NVML_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, RESULT, ...) \
    do                                                       \
    {                                                        \
        static fn_t fn = query_nvml_fn(NAME_STR);            \
        if (fn)                                              \
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
     * @brief Basic GPU device identification information
     */
    struct GpuBasicInfo
    {
        int32_t id;
        std::string name;
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
        int32_t compute_capability_major; // NVIDIA specific
        int32_t compute_capability_minor; // NVIDIA specific
        uint32_t multiprocessor_count;
        uint32_t cuda_cores_per_mp; // NVIDIA specific
        uint32_t total_cuda_cores;  // NVIDIA specific
        uint32_t warp_size;         // 32 for NVIDIA, 64 for AMD
    };

    /**
     * @brief GPU memory information
     */
    struct GpuMemoryInfo
    {
        uint64_t total_global_memory_bytes;
        uint64_t free_memory_bytes;
        uint64_t used_memory_bytes;
        uint32_t memory_bus_width_bits;
        uint32_t memory_clock_rate_khz;
        uint32_t memory_clock_rate_max_khz;
        uint32_t memory_utilization_percent;
    };

    /**
     * @brief GPU clock and frequency information
     */
    struct GpuClockInfo
    {
        uint32_t current_sm_clock_mhz;
        uint32_t current_video_clock_mhz;
        uint32_t current_graphics_clock_mhz;
        uint32_t current_memory_clock_mhz;

        uint32_t max_sm_clock_mhz;
        uint32_t max_video_clock_mhz;
        uint32_t max_graphics_clock_mhz;
        uint32_t max_memory_clock_mhz;
        bool has_frequency_control;
    };

    /**
     * @brief GPU power monitoring information
     */
    struct GpuPowerInfo
    {
        double current_power_watts;
        double power_limit_watts;
        bool has_power_monitoring;
        double min_power_watts;
        double max_power_watts;
        double default_power_watts;
        double current_limit_watts;
        bool is_configurable;
    };

    /**
     * @brief GPU temperature monitoring information
     */
    struct GpuTemperatureInfo
    {
        double current_temperature_celsius;
        double max_temperature_celsius;
        bool has_temperature_monitoring;
    };

    /**
     * @brief GPU utilization and performance information
     */
    struct GpuPerformanceInfo
    {
        uint32_t gpu_utilization_percent;
        bool has_utilization_monitoring;
        uint32_t performance_state;
        uint32_t power_state;
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

    /**
     * @brief Comprehensive GPU device information using composition
     */
    struct GpuDeviceInfo
    {
        GpuBasicInfo basic;
        GpuVersionInfo version;
        GpuComputeInfo compute;
        GpuMemoryInfo memory;
        GpuClockInfo clocks;
        GpuPowerInfo power;
        GpuTemperatureInfo temperature;
        GpuPerformanceInfo performance;
        GpuHardwareInfo hardware;
        GpuCapabilitiesInfo capabilities;
    };

    /**
     * @brief System-wide GPU power information
     */
    struct SystemGpuPowerInfo
    {
        double total_gpu_power_budget_watts;
        double current_gpu_power_usage_watts;
        double available_gpu_power_headroom_watts;
        int32_t num_power_monitored_gpus;
    };

    GpuVendor vendor_from_string(const std::string &vendor_name);
    std::string to_string(GpuVendor vendor);
    std::string to_string(const GpuPowerMethod &method);
    std::string to_string(const GpuBasicInfo &info);
    std::string to_string(const GpuVersionInfo &info);
    std::string to_string(const GpuComputeInfo &info);
    std::string to_string(const GpuMemoryInfo &info);
    std::string to_string(const GpuClockInfo &info);
    std::string to_string(const GpuPowerInfo &info);
    std::string to_string(const GpuTemperatureInfo &info);
    std::string to_string(const GpuPerformanceInfo &info);
    std::string to_string(const GpuHardwareInfo &info);
    std::string to_string(const GpuCapabilitiesInfo &info);
    std::string to_string(const GpuDeviceInfo &info);
    std::string to_string(const SystemGpuPowerInfo &info);

    std::ostream &operator<<(std::ostream &os, GpuVendor vendor);
    std::ostream &operator<<(std::ostream &os, const GpuPowerMethod &method);
    std::ostream &operator<<(std::ostream &os, const GpuBasicInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuVersionInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuComputeInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuMemoryInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuClockInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuPowerInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuTemperatureInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuPerformanceInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuHardwareInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuCapabilitiesInfo &info);
    std::ostream &operator<<(std::ostream &os, const GpuDeviceInfo &info);
    std::ostream &operator<<(std::ostream &os, const SystemGpuPowerInfo &info);
} // namespace optkit::gpu

using optkit::gpu::operator<<; // make available to global namespace
