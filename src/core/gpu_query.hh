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
        static GpuDeviceInfo device_query(uint32_t gpu_id);

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
         * @brief Get available GPU power measurement methods
         * @return bitmask of available methods (NVIDIA_ML, AMD_ROCM, INTEL_I915, etc.)
         */
        static int32_t get_available_power_methods();

        /**
         * @brief Get comprehensive GPU device information
         * @param device_index Index of the GPU device (0 to get_device_count()-1)
         * @param info Output parameter to store the GPU device information
         * @return true if device information was successfully retrieved, false otherwise
         */
        static bool get_device_info(uint32_t device_index, GpuDeviceInfo &info);

        /**
         * @brief Get GPU power usage in Watts
         * @param device_index Index of the GPU device (0 to get_device_count()-1)
         * @param power_watts Output parameter to store the power usage in Watts
         * @return true if power reading was successful, false otherwise
         */
        static bool get_device_power_limit(uint32_t device_index, double &limit_watts);

        /**
         * @brief Get GPU temperature in degrees Celsius
         * @param device_index Index of the GPU device (0 to get_device_count()-1)
         * @param temp_celsius Output parameter to store the temperature in degrees Celsius
         * @return true if temperature reading was successful, false otherwise
         */
        static bool get_device_temperature(uint32_t device_index, double &temp_celsius);
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
