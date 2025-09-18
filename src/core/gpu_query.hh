#pragma once

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <unistd.h>
#include <cstdint>
#include "utils/utils.hh"
#include "utils/gpu.hh"

namespace optkit::gpu
{
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

} // namespace optkit::gpu
