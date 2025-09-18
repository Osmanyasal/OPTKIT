#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>
#include <algorithm>
#include <cstdint>
#include "utils/utils.hh"

// Conditional includes for vendor-specific libraries
#if OPTKIT_ENV_LIB_NVML
#include <nvml.h>
#endif

#include <dlfcn.h>


// Generic runtime NVML call macro with fallback
#define NVML_EXEC_IF_SUPPORTS(NAME_STR, DEVICE, OUT, RESULT) \
do { \
    using fn_t = nvmlReturn_t(*)(nvmlDevice_t, unsigned int*); \
    static fn_t fn = []() -> fn_t { \
        void* lib = dlopen("libnvidia-ml.so", RTLD_NOW | RTLD_NOLOAD); \
        return lib ? (fn_t)dlsym(lib, NAME_STR) : nullptr; \
    }(); \
    if (fn) {fn(DEVICE, OUT); RESULT = NVML_SUCCESS;} \
    else RESULT = NVML_ERROR_NOT_SUPPORTED; \
} while(0)



#ifndef NVML_CUDA_DRIVER_VERSION_MAJOR
#define NVML_CUDA_DRIVER_VERSION_MAJOR(v) (((int32_t)(v))/1000)
#endif

#ifndef NVML_CUDA_DRIVER_VERSION_MINOR
#define NVML_CUDA_DRIVER_VERSION_MINOR(v) (((int32_t)(v)%1000)/10)
#endif

#if OPTKIT_ENV_LIB_ROCM_SMI
#include <rocm_smi.h>
#endif

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
        uint32_t base_clock_rate_khz;
        uint32_t boost_clock_rate_khz;
        uint32_t current_graphics_clock_mhz;
        uint32_t current_memory_clock_mhz;
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

    /**
     * @brief GPU energy measurement capabilities and queries
     */
    class Query final
    {
    public:
        /**
         * @brief Initialize GPU monitoring libraries (NVML, ROCm,...)
         *
         * @return true
         * @return false
         */
        static bool init();
        /**
         * @brief Cleanup vendor-specific libraries (NVML, ROCm)
         * @note Call this when done with GPU monitoring to properly shutdown libraries
         */
        static void shutdown();

        /**
         * @brief Get the driver version object
         *
         * @return double
         */
        static double get_driver_version();

        /**
         * @brief Query GPU device information
         *
         * @param gpu_id
         * @return GpuDeviceInfo
         */
        static GpuDeviceInfo device_query(int32_t gpu_id);

        /**
         * @brief Get the version of the GPU monitoring library
         *
         * @return std::string
         */
        static std::string get_library_version();

        /**
         * @brief Get number of GPU devices detected via vendor libraries
         * @return number of GPU devices
         */
        static uint32_t get_device_count();

        /**
         * @brief Check if NVIDIA GPU power monitoring is available via nvidia-ml
         * @return true if NVIDIA GPUs with power monitoring are detected
         */
        static bool is_nvidia_power_available();

        /**
         * @brief Check if AMD GPU power monitoring is available via ROCm/sysfs
         * @return true if AMD GPUs with power monitoring are detected
         */
        static bool is_amd_power_available();

        /**
         * @brief Check if Intel GPU power monitoring is available via i915/sysfs
         * @return true if Intel integrated GPUs with power monitoring are detected
         */
        static bool is_intel_gpu_power_available();

        /**
         * @brief Get list of available GPU devices with power monitoring
         * @return vector of GPU device info (id, name, vendor, power_cap)
         */
        static std::vector<GpuDeviceInfo> get_power_capable_gpus();

        /**
         * @brief Get available GPU power measurement methods
         * @return bitmask of available methods (NVIDIA_ML, AMD_ROCM, INTEL_I915, etc.)
         */
        static int32_t get_available_power_methods();

        /**
         * @brief Check if GPU frequency scaling is available
         * @return true if GPU frequency can be monitored/controlled
         */
        static bool is_gpu_frequency_control_available();

        /**
         * @brief Check if GPU temperature monitoring is available
         * @return true if GPU temperature sensors are accessible
         */
        static bool is_gpu_temperature_available();

        /**
         * @brief Get GPU compute utilization monitoring capabilities
         * @return true if GPU utilization can be monitored
         */
        static bool is_gpu_utilization_monitoring_available();

        /**
         * @brief Check for GPU memory power monitoring (GDDR/HBM power)
         * @return true if GPU memory power can be monitored separately
         */
        static bool is_gpu_memory_power_available();

        /**
         * @brief Get system-wide GPU power budget info
         * @return total GPU power budget, current usage, available headroom
         */
        static SystemGpuPowerInfo get_system_gpu_power_info();

        // Helper functions for enum conversion
        static std::string to_string(GpuVendor vendor);
        static GpuVendor vendor_from_string(const std::string &vendor_name);
        static void print_device_info(const GpuDeviceInfo &info);

    private:
        static bool initialized;
#if OPTKIT_ENV_LIB_NVML
        static std::vector<nvmlDevice_t> gpu_handles;
#elif OPTKIT_ENV_LIB_ROCM_SMI
        static std::vector<uint32_t> gpu_handles;
#else
#endif

    private:
        Query() = delete;
        ~Query() = delete;

    };

    // Global helper functions
    std::ostream &operator<<(std::ostream &os, GpuVendor vendor);

} // namespace optkit::gpu

using optkit::gpu::operator<<; // make available to global namespace
