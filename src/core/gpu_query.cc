
#include <fstream>
#include <cstdlib>
#include <regex>
#include "utils/logging/logger.hh"
#include "core/gpu_query.hh"

namespace optkit::gpu
{
    // define static variables
    bool Query::initialized = false;

#if OPTKIT_ENV_LIB_NVML
    std::vector<nvmlDevice_t> Query::gpu_handles;
#elif OPTKIT_ENV_LIB_ROCM_SMI
    std::vector<uint32_t> Query::gpu_handles;
#endif

    bool Query::init()
    {
#if OPTKIT_ENV_LIB_NVML
        if (OPT_UNLIKELY(!initialized))
        {
            nvmlReturn_t result = nvmlInit();
            initialized = (result == NVML_SUCCESS);
            for (uint32_t i = 0; i < get_device_count(); i++)
            {
                nvmlDevice_t device;
                result = nvmlDeviceGetHandleByIndex(i, &device);
                if (result == NVML_SUCCESS)
                {
                    Query::gpu_handles.push_back(device);
                }
                else
                {
                    OPTKIT_ERROR("NVML error in nvmlDeviceGetHandleByIndex: {}", std::string(nvmlErrorString(result)));
                    initialized = false;
                }
            }
        }
        else
        {
            OPTKIT_CORE_WARN("Failed to initialize GPU monitoring libraries");
        }
        return initialized;

#elif OPTKIT_ENV_LIB_ROCM_SMI
        if (!initialized)
        {
            rsmi_status_t result = rsmi_init(0);
            initialized = (result == RSMI_STATUS_SUCCESS);
        }
        else
        {
            OPTKIT_CORE_WARN("Failed to initialize GPU monitoring libraries");
        }
        return initialized;
#else
        return false;
#endif
    }

    void Query::shutdown()
    {
        gpu_handles.clear();
        initialized = false;

#if OPTKIT_ENV_LIB_NVML
        if (initialized)
            nvmlShutdown();

#elif OPTKIT_ENV_LIB_ROCM_SMI
        if (initialized)
            rsmi_shut_down();
#endif
    }

    double Query::get_driver_version()
    {
#if OPTKIT_ENV_LIB_NVML
        int32_t version;
        nvmlReturn_t result = nvmlSystemGetCudaDriverVersion(&version);
        if (result == NVML_SUCCESS)
        {
            int32_t major = NVML_CUDA_DRIVER_VERSION_MAJOR(version);
            int32_t minor = NVML_CUDA_DRIVER_VERSION_MINOR(version);
            return major + minor / 10.0; // divide minor by 10
        }
        else
        {
            OPTKIT_ERROR("NVML error in nvmlSystemGetCudaDriverVersion: {}", std::string(nvmlErrorString(result)));
            return -1;
        }
#elif OPTKIT_ENV_LIB_ROCM_SMI
        rsmi_status_t status = rsmi_init(0);
        if (status != RSMI_STATUS_SUCCESS)
        {
            OPTKIT_ERROR("ROCm SMI init failed: {}", status);
            return -1;
        }

        const char *version_cstr = nullptr;
        status = rsmi_version_get(&version_cstr);
        if (status != RSMI_STATUS_SUCCESS || version_cstr == nullptr)
        {
            OPTKIT_ERROR("ROCm SMI failed to get version: {}", status);
            return -1;
        }

        std::string version_str(version_cstr);
        int32_t major = 0, minor = 0;
        try
        {
            std::size_t dot_pos = version_str.find('.');
            if (dot_pos != std::string::npos)
            {
                major = std::stoi(version_str.substr(0, dot_pos));
                std::size_t second_dot = version_str.find('.', dot_pos + 1);
                minor = std::stoi(version_str.substr(dot_pos + 1, second_dot - dot_pos - 1));
            }
            else
            {
                OPTKIT_WARN("ROCm version string '{}' has no dot, cannot parse", version_str);
                return -1;
            }
        }
        catch (const std::exception &e)
        {
            OPTKIT_ERROR("Failed to parse ROCm version '{}': {}", version_str, e.what());
            return -1;
        }
        return major + minor / 10.0; // major.minor as double
#else
        OPTKIT_WARN("Driver version query not supported without NVML or ROCm SMI");
        return -1;
#endif
    }

    std::string Query::get_library_version()
    {
#if OPTKIT_ENV_LIB_NVML
        char version[NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE] = {0};
        nvmlReturn_t result = nvmlSystemGetNVMLVersion(version, NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
        if (result == NVML_SUCCESS)
        {
            return std::string(version);
        }
        else
        {
            OPTKIT_ERROR("NVML error in nvmlSystemGetNVMLVersion: {}", std::string(nvmlErrorString(result)));
            return "unknown";
        }

#elif OPTKIT_ENV_LIB_ROCM_SMI
        rsmi_status_t status = rsmi_init(0);
        if (status != RSMI_STATUS_SUCCESS)
        {
            OPTKIT_ERROR("ROCm SMI init failed: {}", status);
            return "unknown";
        }

        const char *version_cstr = nullptr;
        status = rsmi_version_get(&version_cstr);
        if (status != RSMI_STATUS_SUCCESS || version_cstr == nullptr)
        {
            OPTKIT_ERROR("ROCm SMI failed to get version: {}", status);
            return "unknown";
        }

        std::string version_str(version_cstr);
        return version_str;

#else
        OPTKIT_WARN("Library version query not supported without NVML or ROCm SMI");
        return "unknown";
#endif
    }

    uint32_t Query::get_device_count()
    {
        static uint32_t cached_count = UINT32_MAX; // Use max value as "uninitialized" marker

        if (OPT_UNLIKELY(cached_count == UINT32_MAX))
        {
#if OPTKIT_ENV_LIB_NVML
            uint32_t count;
            nvmlReturn_t result = nvmlDeviceGetCount(&count);
            cached_count = (result == NVML_SUCCESS) ? count : 0;
#elif OPTKIT_ENV_LIB_ROCM_SMI
            uint32_t count;
            rsmi_status_t result = rsmi_num_monitor_devices(&count);
            cached_count = (result == RSMI_STATUS_SUCCESS) ? count : 0;
#else
            cached_count = 0;
#endif
        }

        return cached_count;
    }

    bool Query::get_device_power(uint32_t device_index, double &power_watts)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device = Query::gpu_handles.at(device_index);
        nvmlReturn_t result;
        uint32_t power_mw;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetPowerUsage", device, result, &power_mw);
        if (result == NVML_SUCCESS)
        {
            // note that this is 1 seconds average power usage
            power_watts = power_mw / 1000.0; // Convert from milliwatts to watts
            return true;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerUsage: {}", nvmlErrorString(result));
            power_watts = 0.0; // Not supported
        }
        return false;
#elif OPTKIT_ENV_LIB_ROCM_SMI
        uint64_t power_uw;
        rsmi_status_t result = rsmi_dev_power_ave_get(device_index, 0, &power_uw);
        if (result == RSMI_STATUS_SUCCESS)
        {
            power_watts = power_uw / 1000000.0; // Convert from microwatts to watts
            return true;
        }
        return false;
#endif
    }

    bool Query::get_device_power_limit(uint32_t device_index, double &limit_watts)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device = Query::gpu_handles.at(device_index);
        nvmlReturn_t result;
        uint32_t limit_mw;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementLimit",
            device,
            result,
            &limit_mw);
        if (result == NVML_SUCCESS)
        {
            limit_watts = limit_mw / 1000.0; // Convert from milliwatts to watts
            return true;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerManagementLimit: {}", nvmlErrorString(result));
            limit_watts = 0.0; // Not supported
        }
        return false;
#elif OPTKIT_ENV_LIB_ROCM_SMI
        // ROCm SMI does not provide a direct method to get power limit
        // This would require reading from sysfs or using other methods
        // For now, we return false indicating not implemented
#endif
        return false;
    }

    bool Query::get_device_temperature(uint32_t device_index, double &temp_celsius)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device = Query::gpu_handles.at(device_index);
        nvmlReturn_t result;
        uint32_t temp;

        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetTemperature", device, result, NVML_TEMPERATURE_GPU, &temp);
        if (result == NVML_SUCCESS)
        {
            temp_celsius = static_cast<double>(temp);
            return true;
        }
        else
        {
            temp_celsius = 0.0; // Not supported
            OPTKIT_WARN("nvmlDeviceGetTemperature: {}", nvmlErrorString(result));
        }
        return false;
#elif OPTKIT_ENV_LIB_ROCM_SMI
        int64_t temp_millidegrees;
        rsmi_status_t result = rsmi_dev_temp_metric_get(device_index, RSMI_TEMP_TYPE_EDGE, RSMI_TEMP_CURRENT, &temp_millidegrees);
        if (result == RSMI_STATUS_SUCCESS)
        {
            temp_celsius = temp_millidegrees / 1000.0; // Convert from millidegrees to degrees Celsius
            return true;
        }
        return false;
#endif
        return false;
    }

    bool Query::get_device_info(uint32_t device_index, GpuDeviceInfo &info)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device = Query::gpu_handles.at(device_index);
        nvmlReturn_t result;
        // Get device name
        char name[NVML_DEVICE_NAME_BUFFER_SIZE];
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetName", device, result, &name[0], NVML_DEVICE_NAME_BUFFER_SIZE);
        if (result == NVML_SUCCESS)
        {
            info.basic.device_name = std::string(name);
            info.basic.name = info.basic.device_name;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetName: {}", nvmlErrorString(result));
        }
        // Get compute capability
        int major, minor;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetCudaComputeCapability", device, result, &major, &minor);
        if (result == NVML_SUCCESS)
        {
            info.compute.compute_capability_major = major;
            info.compute.compute_capability_minor = minor;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetCudaComputeCapability: {}", nvmlErrorString(result));
        }

        info.basic.architecture = get_gpu_architecture(device_index);

        // Get memory information
        nvmlMemory_t memory;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMemoryInfo",
            device,
            result,
            &memory);
        if (result == NVML_SUCCESS)
        {
            info.memory.total_global_memory_bytes = memory.total;
            info.memory.free_memory_bytes = memory.free;
            info.memory.used_memory_bytes = memory.used;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMemoryInfo: {}", nvmlErrorString(result));
        }

        // Get memory bus width
        uint32_t busWidth;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMemoryBusWidth",
            device,
            result,
            &busWidth);

        if (result == NVML_SUCCESS)
        {
            info.memory.memory_bus_width_bits = busWidth;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMemoryBusWidth: {}", nvmlErrorString(result));
        }

        // Get multiprocessor count
        uint32_t multiProcessorCount;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMultiProcessorCount",
            device,
            result,
            &multiProcessorCount);
        if (result == NVML_SUCCESS)
        {
            info.compute.multiprocessor_count = multiProcessorCount;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMultiProcessorCount: {}", nvmlErrorString(result));
        }

        // Get current clock rates
        uint32_t clockRate;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetClockInfo",
            device,
            result,
            NVML_CLOCK_GRAPHICS,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_graphics_clock_mhz = clockRate * 1000; // Convert MHz to kHz
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
        }
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetClockInfo",
            device,
            result,
            NVML_CLOCK_MEM,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_memory_clock_mhz = clockRate * 1000; // Convert MHz to kHz
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
        }
        // Get current clocks
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetClockInfo",
            device,
            result,
            NVML_CLOCK_SM,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_sm_clock_mhz = clockRate;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
        }
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetClockInfo",
            device,
            result,
            NVML_CLOCK_VIDEO,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_video_clock_mhz = clockRate;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetClockInfo: {}", nvmlErrorString(result));
        }

        // get max clock rates
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMaxClockInfo",
            device,
            result,
            NVML_CLOCK_GRAPHICS,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_graphics_clock_mhz = clockRate * 1000; // Convert MHz to kHz
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
        }
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMaxClockInfo",
            device,
            result,
            NVML_CLOCK_SM,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.max_sm_clock_mhz = clockRate;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
        }
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMaxClockInfo",
            device,
            result,
            NVML_CLOCK_VIDEO,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.max_video_clock_mhz = clockRate;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
        }
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMaxClockInfo",
            device,
            result,
            NVML_CLOCK_MEM,
            &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.max_memory_clock_mhz = clockRate;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetMaxClockInfo: {}", nvmlErrorString(result));
        }

        // Get power information
        get_device_power(device_index, info.power.current_power_watts);

        uint32_t power;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementLimit",
            device,
            result,
            &power);
        if (result == NVML_SUCCESS)
        {
            info.power.power_limit_watts = power / 1000.0;
            info.power.max_power_watts = info.power.power_limit_watts;
            info.power.current_limit_watts = info.power.power_limit_watts;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerManagementLimit: {}", nvmlErrorString(result));
        }

        // Get power management constraints (min/max/default)
        uint32_t min_power, max_power, default_power;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementLimitConstraints",
            device,
            result,
            &min_power,
            &max_power);
        if (result == NVML_SUCCESS)
        {
            info.power.min_power_watts = min_power / 1000.0;
            info.power.max_power_watts = max_power / 1000.0;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerManagementLimitConstraints: {}", nvmlErrorString(result));
        }

        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementDefaultLimit",
            device,
            result,
            &default_power);
        if (result == NVML_SUCCESS)
        {
            info.power.default_power_watts = default_power / 1000.0;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerManagementDefaultLimit: {}", nvmlErrorString(result));
        }

        // Check if power management is configurable
        nvmlEnableState_t power_management_state;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementMode",
            device,
            result,
            &power_management_state);
        if (result == NVML_SUCCESS)
        {
            info.power.is_configurable = (power_management_state == NVML_FEATURE_ENABLED);
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPowerManagementMode: {}", nvmlErrorString(result));
        }

        // Get temperature
        uint32_t temp;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetTemperature", device, result, NVML_TEMPERATURE_GPU, &temp);
        if (result == NVML_SUCCESS)
        {
            info.temperature.current_temperature_celsius = static_cast<double>(temp);
            info.temperature.has_temperature_monitoring = true;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetTemperature: {}", nvmlErrorString(result));
        }

        // Get utilization
        nvmlUtilization_t utilization;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetUtilizationRates", device, result, &utilization);
        if (result == NVML_SUCCESS)
        {
            info.performance.gpu_utilization_percent = utilization.gpu;
            info.memory.memory_utilization_percent = utilization.memory;
            info.performance.has_utilization_monitoring = true;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetUtilizationRates: {}", nvmlErrorString(result));
        }

        // Get PCI information
        nvmlPciInfo_t pci;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPciInfo",
            device,
            result,
            &pci);
        if (result == NVML_SUCCESS)
        {
            info.hardware.pci_bus_id = std::string(pci.busId);
            info.hardware.pci_device_id = pci.pciDeviceId;
            info.hardware.pci_subsystem_id = pci.pciSubSystemId;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPciInfo: {}", nvmlErrorString(result));
        }

        // Get performance state
        nvmlPstates_t pstate;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetPerformanceState", device, result, &pstate);
        if (result == NVML_SUCCESS)
        {
            info.performance.performance_state = static_cast<uint32_t>(pstate);
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPerformanceState: {}", nvmlErrorString(result));
        }

        // Get ECC mode
        nvmlEnableState_t current, pending;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetEccMode", device, result, &current, &pending);
        if (result == NVML_SUCCESS)
        {
            info.capabilities.ecc_enabled = (current == NVML_FEATURE_ENABLED);
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetEccMode: {}", nvmlErrorString(result));
        }

        // Get board ID for multi-GPU detection
        uint32_t boardId;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetBoardId", device, result, &boardId);
        if (result == NVML_SUCCESS)
        {
            info.hardware.board_id = boardId;
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetBoardId: {}", nvmlErrorString(result));
        }
        // Get persistence mode
        nvmlEnableState_t mode;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetPersistenceMode", device, result, &mode);
        if (result == NVML_SUCCESS)
        {
            info.capabilities.persistence_mode_enabled = (mode == NVML_FEATURE_ENABLED);
        }
        else
        {
            OPTKIT_WARN("nvmlDeviceGetPersistenceMode: {}", nvmlErrorString(result));
        }

        // Estimate CUDA cores (approximation based on compute capability)
        if (info.compute.compute_capability_major > 0)
        {
            // Rough estimates for CUDA cores per SM for different architectures
            uint32_t cores_per_sm = 32; // Default fallback
            if (info.compute.compute_capability_major == 2)
                cores_per_sm = 32;
            else if (info.compute.compute_capability_major == 3)
                cores_per_sm = 192;
            else if (info.compute.compute_capability_major == 5)
                cores_per_sm = 128;
            else if (info.compute.compute_capability_major == 6)
                cores_per_sm = (info.compute.compute_capability_minor == 0) ? 64 : 128;
            else if (info.compute.compute_capability_major == 7)
                cores_per_sm = 64;
            else if (info.compute.compute_capability_major == 8)
                cores_per_sm = (info.compute.compute_capability_minor == 6) ? 128 : 64;
            else if (info.compute.compute_capability_major >= 9)
                cores_per_sm = 128;

            info.compute.cuda_cores_per_mp = cores_per_sm;
            info.compute.total_cuda_cores = info.compute.multiprocessor_count * cores_per_sm;
            info.compute.warp_size = 32; // Standard for NVIDIA
        }

        // Set vendor-specific information
        info.basic.vendor = GpuVendor::NVIDIA;
        info.basic.vendor_string = to_string(GpuVendor::NVIDIA);
        info.clocks.has_frequency_control = true;
        info.basic.is_integrated = false; // NVIDIA discrete GPUs are typically not integrated

        return true;

#elif OPTKIT_ENV_LIB_ROCM_SMI
        // Get device name
        char name[256];
        rsmi_status_t result = rsmi_dev_name_get(device_index, name, 256);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.basic.device_name = std::string(name);
            info.basic.name = info.basic.device_name;
        }

        // Get memory information
        uint64_t total_memory, used_memory;
        result = rsmi_dev_memory_total_get(device_index, RSMI_MEM_TYPE_VRAM, &total_memory);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.memory.total_global_memory_bytes = total_memory;
        }

        result = rsmi_dev_memory_usage_get(device_index, RSMI_MEM_TYPE_VRAM, &used_memory);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.memory.used_memory_bytes = used_memory;
            info.memory.free_memory_bytes = info.memory.total_global_memory_bytes - used_memory;
        }

        // Get power information
        uint64_t power;
        result = rsmi_dev_power_ave_get(device_index, 0, &power);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.power.current_power_watts = power / 1000000.0; // Convert from microwatts
            info.power.has_power_monitoring = true;
        }

        result = rsmi_dev_power_cap_get(device_index, 0, &power);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.power.power_limit_watts = power / 1000000.0;
            info.power.max_power_watts = info.power.power_limit_watts;
            info.power.current_limit_watts = info.power.power_limit_watts;
        }

        // Get power range (min/max power capabilities)
        uint64_t min_power, max_power;
        result = rsmi_dev_power_cap_range_get(device_index, 0, &max_power, &min_power);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.power.min_power_watts = min_power / 1000000.0;
            info.power.max_power_watts = max_power / 1000000.0;
        }

        // For ROCm, assume power is configurable if we can read the cap
        info.power.is_configurable = (rsmi_dev_power_cap_get(device_index, 0, &power) == RSMI_STATUS_SUCCESS);

        // Default power - ROCm doesn't provide this directly, use current limit as approximation
        info.power.default_power_watts = info.power.power_limit_watts;

        // Get temperature
        int64_t temp;
        result = rsmi_dev_temp_metric_get(device_index, RSMI_TEMP_TYPE_EDGE, RSMI_TEMP_CURRENT, &temp);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.temperature.current_temperature_celsius = temp / 1000.0;
            info.temperature.has_temperature_monitoring = true;
        }

        // Get utilization
        uint32_t busy_percent;
        result = rsmi_dev_busy_percent_get(device_index, &busy_percent);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.performance.gpu_utilization_percent = busy_percent;
            info.performance.has_utilization_monitoring = true;
        }

        // Get clock frequencies
        rsmi_frequencies_t frequencies;
        result = rsmi_dev_gpu_clk_freq_get(device_index, RSMI_CLK_TYPE_SYS, &frequencies);
        if (result == RSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
        {
            info.clocks.current_graphics_clock_mhz = frequencies.frequency[frequencies.current] / 1000000; // Convert Hz to MHz
            // Find max frequency
            uint64_t max_freq = 0;
            for (uint32_t i = 0; i < frequencies.num_supported; i++)
            {
                if (frequencies.frequency[i] > max_freq)
                    max_freq = frequencies.frequency[i];
            }
            info.clocks.boost_clock_rate_khz = max_freq / 1000; // Convert Hz to kHz
        }

        result = rsmi_dev_gpu_clk_freq_get(device_index, RSMI_CLK_TYPE_MEM, &frequencies);
        if (result == RSMI_STATUS_SUCCESS && frequencies.num_supported > 0)
        {
            info.clocks.current_memory_clock_mhz = frequencies.frequency[frequencies.current] / 1000000;
            uint64_t max_freq = 0;
            for (uint32_t i = 0; i < frequencies.num_supported; i++)
            {
                if (frequencies.frequency[i] > max_freq)
                    max_freq = frequencies.frequency[i];
            }
            info.memory.memory_clock_rate_max_khz = max_freq / 1000;
        }

        // Get PCI information
        uint64_t pci_info;
        result = rsmi_dev_pci_id_get(device_index, &pci_info);
        if (result == RSMI_STATUS_SUCCESS)
        {
            info.hardware.pci_device_id = static_cast<uint32_t>(pci_info & 0xFFFF);
        }

        // Set vendor-specific information
        info.basic.vendor = GpuVendor::AMD;
        info.basic.vendor_string = Query::to_string(GpuVendor::AMD);
        info.clocks.has_frequency_control = true;
        info.basic.is_integrated = false; // ROCm devices are typically discrete

        // Set defaults for AMD-specific fields
        info.compute.compute_capability_major = 0; // AMD doesn't use CUDA compute capability
        info.compute.compute_capability_minor = 0;
        info.compute.warp_size = 64;        // AMD wavefront size
        info.compute.cuda_cores_per_mp = 0; // Not applicable for AMD
        info.compute.total_cuda_cores = 0;  // Not applicable for AMD

        return true;
#endif
        return false;
    }

    uint32_t Query::get_gpu_architecture(uint32_t device_index)
    {

        nvmlDeviceArchitecture_t arch;
        nvmlDevice_t device = Query::gpu_handles.at(device_index);
        nvmlReturn_t result;
        NVML_EXEC_IF_SUPPORTS("nvmlDeviceGetArchitecture", device, result, &arch);
        if (result == NVML_SUCCESS)
            return static_cast<uint32_t>(arch);
        else
            return NVML_DEVICE_ARCH_UNKNOWN; // Unknown architecture
    }

    GpuDeviceInfo Query::device_query(uint32_t gpu_id)
    {
        GpuDeviceInfo info = {};
        if (OPT_UNLIKELY(gpu_id > get_device_count()))
        {
            OPTKIT_CORE_WARN("Not valid GPU ID: {}, Total device count: {}", gpu_id, get_device_count());
            return info;
        }
        // Rely on value-initialization above (GpuDeviceInfo info = {}).
        // Do not memset structs with non-trivial members.
        info.basic.id = gpu_id;
        info.basic.vendor = GpuVendor::UNKNOWN;
        info.basic.vendor_string = to_string(GpuVendor::UNKNOWN);

        // Reasonable default; vendor-specific code may override (e.g., AMD=64).
        info.compute.warp_size = 32;

        // Get driver and library versions
        info.version.driver_major_minor = get_driver_version();
        info.version.driver_version_string = std::to_string(info.version.driver_major_minor);
        info.version.library_version_string = get_library_version();

        if (get_device_info(static_cast<uint32_t>(gpu_id), info))
            return info;
        else
        {
            OPTKIT_ERROR("Failed to get device info for GPU ID {}", gpu_id);
        }
        return info;
    }

    bool Query::is_power_available()
    {
        // First try NVML
        if (get_device_count() > 0)
            return true;
        return false;
    }
} // namespace optkit::gpu
