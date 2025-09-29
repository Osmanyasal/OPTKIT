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
#include <unordered_map>

#include "utils/utils.hh"
#include "utils/gpu.hh"

namespace optkit::gpu
{
    /**
     * @brief GPU energy measurement capabilities and queries
     *
     * This class provides methods to initialize GPU monitoring libraries, query various GPU properties,
     * and retrieve detailed information about GPU devices from different vendors such as NVIDIA and AMD.
     * It supports querying basic information, version details, compute capabilities, memory statistics,
     * clock speeds, power usage, temperature readings, performance metrics, hardware specifics, and capabilities
     *
     * it has C style static methods and return values are passed by reference.
     * All methods return bool to indicate success or failure of the query.
     * if a query fails to call an API method for any reason, the output parameters may be left unchanged or set to default values and returns false by printing the error message.
     */
    class Query final
    {
#if OPTKIT_ENV_LIB_AMDSMI
    private:
        friend inline uint32_t _amdsmi_populate_device_count_and_fill_handlers();
#endif
    public:
        /**
         * @brief Initialize GPU monitoring libraries (NVML, ROCm,...)
         *
         * @return true
         * @return false
         */
        static bool init(GpuVendor vendor);

        /**
         * @brief Cleanup vendor-specific libraries (NVML, ROCm)
         * @note Call this when done with GPU monitoring to properly shutdown libraries
         */
        static bool shutdown(GpuVendor vendor);

        static bool get_basic_info(GpuVendor vendor, uint32_t device_index, GpuBasicInfo &basic_info);
        static bool get_version_info(GpuVendor vendor, uint32_t device_index, GpuVersionInfo &version_info);
        static bool get_compute_info(GpuVendor vendor, uint32_t device_index, GpuComputeInfo &compute_info);
        static bool get_memory_info(GpuVendor vendor, uint32_t device_index, GpuMemoryInfo &memory_info);
        static bool get_clock_info(GpuVendor vendor, uint32_t device_index, GpuClockInfo &clock_info);
        static bool get_power_info(GpuVendor vendor, uint32_t device_index, GpuPowerInfo &power_info);
        static bool get_temperature_info(GpuVendor vendor, uint32_t device_index, GpuTemperatureInfo &temperature_info);
        static bool get_utilization_info(GpuVendor vendor, uint32_t device_index, GpuUtilizationInfo &utilization_info);
        static bool get_hardware_info(GpuVendor vendor, uint32_t device_index, GpuHardwareInfo &hardware_info);
        static bool get_capabilities_info(GpuVendor vendor, uint32_t device_index, GpuCapabilitiesInfo &capabilities_info);

        /**
         * @brief Get the warp size of the GPU
         */
        static bool get_warp_size(GpuVendor vendor, uint32_t device_index, uint32_t &warp_size);

        /**
         * @brief Query GPU device information, returns all information about the GPU
         *
         * @param gpu_id
         * @return GpuDeviceInfo
         */
        static GpuDeviceInfo device_query(GpuVendor vendor, uint32_t gpu_id = 0);

        /**
         * @brief Get the driver version of the GPU
         *
         * @return double -> major.minor
         */
        static bool get_driver_version(GpuVendor vendor, double &driver_version);

        /**
         * @brief Get the version of the GPU monitoring library
         *
         * @return std::string
         */
        static bool get_library_version(GpuVendor vendor, std::string &library_version);

        /**
         * @brief Get number of GPU devices detected via vendor libraries
         * @return number of GPU devices
         */
        static bool get_device_count(GpuVendor vendor, uint32_t &device_count);

        /**
         * @brief Get the device power implementation object
         *
         * @param vendor
         * @param device_index
         * @param power_watts
         * @return true
         * @return false
         */
        static bool get_device_power(GpuVendor vendor, uint32_t device_index, double &power_watts);

        /**
         * @brief Get the cpu architecture object
         *
         * @return bool
         */
        static bool get_architecture(GpuVendor vendor, uint32_t device_index, uint32_t &architecture);

        /**
         * @brief Get GPU power limits in Watts
         *
         * @param vendor
         * @param device_index
         * @param limit_watts
         * @param default_power
         * @param min_limit_watts
         * @param max_limit_watts
         * @return true
         * @return false
         */
        static bool get_device_power_limits(GpuVendor vendor, uint32_t device_index, double &limit_watts, double &default_power, double &min_limit_watts, double &max_limit_watts, bool &is_configurable);

        /**
         * @brief Get GPU temperature in degrees Celsius
         * @param device_index Index of the GPU device (0 to get_device_count()-1)
         * @param temp_celsius Output parameter to store the temperature in degrees Celsius
         * @return true if temperature reading was successful, false otherwise
         */
        static bool get_device_temperature(GpuVendor vendor, uint32_t device_index, double &temp_celsius);

        /**
         * @brief Get the name of the GPU device
         *
         * @param vendor
         * @param device_index
         * @param name
         * @return true
         * @return false
         */
        static bool get_device_name(GpuVendor vendor, uint32_t device_index, std::string &device_name);

        /**
         * @brief Get comprehensive GPU device information
         * @param device_index Index of the GPU device (0 to get_device_count()-1)
         * @param info Output parameter to store the GPU device information
         * @return true if device information was successfully retrieved, false otherwise
         */
        // TODO: NOT OK
        static bool get_device_info(GpuVendor vendor, uint32_t device_index, GpuDeviceInfo &info);

        static bool get_device_temperature_thresholds(GpuVendor vendor, uint32_t device_index, double &max_temp_celsius);

    private:
        static std::unordered_map<GpuVendor, bool> initialized;

#if OPTKIT_ENV_LIB_NVML
        static std::vector<nvmlDevice_t> gpu_handles_nvml;
#endif

#if OPTKIT_ENV_LIB_AMDSMI
        static std::vector<amdsmi_socket_handle> socket_handles_amdsmi;
        static std::vector<amdsmi_processor_handle> gpu_handles_amdsmi;
#endif

    private:
        Query() = delete;
        ~Query() = delete;
    };

} // namespace optkit::gpu
