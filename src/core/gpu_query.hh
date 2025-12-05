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
     * @brief GPU measurement capabilities and queries
     *
     * This class provides methods to initialize GPU monitoring libraries, query various GPU properties,
     * and retrieve detailed information about GPU devices from different vendors such as NVIDIA and AMD.
     * It supports querying basic information, version details, compute capabilities, memory statistics,
     * clock speeds, power usage, temperature readings, performance metrics, hardware specifics, and capabilities along with altering device settings.
     *
     * it has C style static methods and return values are passed by reference.
     * All methods return bool to indicate success or failure of the query. By default it returns false, if query is made successfully, it returns true.
     * @note some methods perform many API calls to gather information, if any of the calls fail, the method returns false. but results upto that point are still filled in the output parameters.
     */
    class Query final
    {
#if OPTKIT_ENV_LIB_AMDSMI
    private:
        friend inline uint32_t _amdsmi_populate_device_count_and_fill_handlers();
#endif
    public:
        static bool init(GpuVendor vendor);
        static bool is_init(GpuVendor vendor);
        static bool shutdown(GpuVendor vendor);
        static bool is_device_exists(GpuVendor vendor);
        static bool shutdown_amdsmi();
        static bool shutdown_nvml();
        static bool device_query(GpuVendor vendor, uint32_t device_index, GpuDeviceInfo &info);
        static bool get_basic_info(GpuVendor vendor, uint32_t device_index, GpuBasicInfo &basic_info);
        static bool get_version_info(GpuVendor vendor, uint32_t device_index, GpuVersionInfo &version_info);
        static bool get_memory_info(GpuVendor vendor, uint32_t device_index, GpuMemoryInfo &memory_info);
        static bool get_clock_info(GpuVendor vendor, uint32_t device_index, GpuClockInfo &clock_info);
        static bool get_temperature_info(GpuVendor vendor, uint32_t device_index, GpuTemperatureInfo &temperature_info);
        static bool get_compute_info(GpuVendor vendor, uint32_t device_index, GpuComputeInfo &compute_info);
        static bool get_power_info(GpuVendor vendor, uint32_t device_index, GpuPowerInfo &power_info);
        static bool get_utilization_info(GpuVendor vendor, uint32_t device_index, GpuUtilizationInfo &utilization_info);
        static bool get_hardware_info(GpuVendor vendor, uint32_t device_index, GpuHardwareInfo &hardware_info);
        static bool get_capabilities_info(GpuVendor vendor, uint32_t device_index, GpuCapabilitiesInfo &capabilities_info);

        //****** these may not be supported on consumer grade GPUs ****** //
        static bool set_clock(GpuVendor vendor, uint32_t device_index, uint32_t mem_clk_mhz, uint32_t graphics_clk_mhz);
        static bool reset_clock(GpuVendor vendor, uint32_t device_index);
        static bool reset_device(GpuVendor vendor, uint32_t device_index);
        static bool set_persistence_mode(GpuVendor vendor, uint32_t device_index, bool enable);
        static bool set_fan_speed(GpuVendor vendor, uint32_t device_index, const std::string &fan_speed_percent);
        static bool reset_fan_speed(GpuVendor vendor, uint32_t device_index);
        //****** these may not be supported on consumer grade GPUs ****** //

        // utilities to be called by above methods.
        static bool get_warp_size(GpuVendor vendor, uint32_t device_index, uint32_t &warp_size);
        static bool get_driver_version(GpuVendor vendor, double &driver_version);
        static bool get_library_version(GpuVendor vendor, std::string &library_version);
        static bool get_device_count(GpuVendor vendor, uint32_t &device_count);
        static bool get_device_power(GpuVendor vendor, uint32_t device_index, double &power_watts);
        static bool get_architecture(GpuVendor vendor, uint32_t device_index, uint32_t &architecture);
        static bool get_device_power_limits(GpuVendor vendor, uint32_t device_index, double &limit_watts, double &default_power, double &min_limit_watts, double &max_limit_watts, bool &is_configurable);
        static bool get_device_temperature(GpuVendor vendor, uint32_t device_index, double &device_temp_celsius, double &memory_temp_celsius);
        static bool get_device_name(GpuVendor vendor, uint32_t device_index, std::string &device_name);
        static bool get_device_temperature_thresholds(GpuVendor vendor, uint32_t device_index, double &max_gpu_temp_celsius, double &max_mem_temp_celsius, double &min_gpu_temp_celsius, double &min_mem_temp_celsius);

    private:
        static std::unordered_map<GpuVendor, bool> initialized;

#if OPTKIT_ENV_LIB_NVML
        static std::vector<nvmlDevice_t> gpu_handles_nvml;
#endif

#if OPTKIT_ENV_LIB_AMDSMI
        static std::vector<amdsmi_socket_handle> socket_handles_amdsmi;
        static std::vector<amdsmi_processor_handle> gpu_handles_amdsmi;
#elif OPTKIT_ENV_LIB_ROCM_SMI
        static std::vector<uint32_t> gpu_handles_rocm_smi; // ROCm SMI uses device indices
#endif

    private:
        Query() = delete;
        ~Query() = delete;
    };

#if OPTKIT_ENV_LIB_AMDSMI
    inline uint32_t _amdsmi_populate_device_count_and_fill_handlers()
    {
        static bool is_amdsmi_initialized = false;
        static uint32_t total_device_count = 0;
        if (!is_amdsmi_initialized)
        {
            is_amdsmi_initialized = true;
            amdsmi_status_t result;
            uint32_t socket_count = 0;

            result = amdsmi_get_socket_handles(&socket_count, nullptr);
            if (result == AMDSMI_STATUS_SUCCESS)
            {
                Query::socket_handles_amdsmi.reserve(socket_count);

                // First, get all socket handles
                for (uint32_t i = 0; i < socket_count; i++)
                {
                    amdsmi_socket_handle socket;
                    result = amdsmi_get_socket_handles(&socket_count, &socket);
                    if (result == AMDSMI_STATUS_SUCCESS)
                    {
                        Query::socket_handles_amdsmi.push_back(socket);
                    }
                    else
                    {
                        OPTKIT_WARN("Failed to get socket handle {}: {}", i, _amdsmi_status_to_string(result));
                    }
                }

                // Then, accumulate device counts from each socket
                for (auto &&socket : Query::socket_handles_amdsmi)
                {
                    uint32_t socket_device_count = 0;
                    result = amdsmi_get_processor_handles(socket, &socket_device_count, nullptr);
                    if (result == AMDSMI_STATUS_SUCCESS)
                    {
                        total_device_count += socket_device_count;

                        // Reserve space for each socket at least (usually 1 socket = 1 GPU but not always)
                        Query::gpu_handles_amdsmi.reserve(total_device_count);

                        // Get the actual processor handles for this socket
                        std::vector<amdsmi_processor_handle> socket_handles(socket_device_count);
                        result = amdsmi_get_processor_handles(socket, &socket_device_count, socket_handles.data());
                        if (result == AMDSMI_STATUS_SUCCESS)
                        {
                            // Add handles from this socket to the main vector
                            Query::gpu_handles_amdsmi.insert(
                                Query::gpu_handles_amdsmi.end(),
                                socket_handles.begin(),
                                socket_handles.end());
                        }
                        else
                        {
                            OPTKIT_WARN("Failed to get processor handles for socket: {}", _amdsmi_status_to_string(result));
                        }
                    }
                    else
                    {
                        OPTKIT_WARN("Failed to get processor count for socket: {}", _amdsmi_status_to_string(result));
                    }
                }
            }
            else
            {
                OPTKIT_ERROR("AMD SMI error in amdsmi_get_socket_handles: {}", _amdsmi_status_to_string(result));
            }
        }

        return total_device_count;
    }
#endif

} // namespace optkit::gpu
