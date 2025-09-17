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

namespace optkit::energy::gpu
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
     * @brief GPU device information for power monitoring
     */
    struct GpuDeviceInfo
    {
        int32_t id;
        std::string name;
        GpuVendor vendor;
        double max_power_watts;
        double current_power_watts;
        bool has_power_monitoring;
        bool has_frequency_control;
        bool has_temperature_monitoring;
    };

    /**
     * @brief GPU power limits and capabilities
     */
    struct GpuPowerLimits
    {
        double min_power_watts;
        double max_power_watts;
        double default_power_watts;
        double current_limit_watts;
        bool is_configurable;
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
         * @brief Get GPU power limits and capabilities
         * @param gpu_id GPU device ID
         * @return power limits (min, max, default) in watts
         */
        static GpuPowerLimits get_gpu_power_limits(int32_t gpu_id);

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

        /**
         * @brief Cleanup vendor-specific libraries (NVML, ROCm)
         * @note Call this when done with GPU monitoring to properly shutdown libraries
         */
        static void cleanup_vendor_libraries();

    private:
        Query() = delete;
        ~Query() = delete;
    };

    // Helper functions for enum conversion
    std::string to_string(GpuVendor vendor);
    GpuVendor vendor_from_string(const std::string &vendor_name);

    std::ostream &operator<<(std::ostream &os, GpuVendor vendor);

} // namespace optkit::energy::gpu
