
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
        return initialized;

#elif OPTKIT_ENV_LIB_ROCM_SMI
        if (!initialized)
        {
            rsmi_status_t result = rsmi_init(0);
            initialized = (result == RSMI_STATUS_SUCCESS);
        }
        return initialized;
#else
        return false;
#endif
    }

    void Query::shutdown()
    {
#if OPTKIT_ENV_LIB_NVML
        if (initialized)
        {
            nvmlShutdown();
            gpu_handles.clear();
            initialized = false;
        }

#elif OPTKIT_ENV_LIB_ROCM_SMI
        if (initialized)
        {
            rsmi_shut_down();
            gpu_handles.clear();
            initialized = false;
        }
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

    bool get_device_power_impl(unsigned int device_index, double &power_watts)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device;
        nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if (result != NVML_SUCCESS)
            return false;

        unsigned int power_mw;
        result = nvmlDeviceGetPowerUsage(device, &power_mw);
        if (result == NVML_SUCCESS)
        {
            power_watts = power_mw / 1000.0; // Convert from milliwatts to watts
            return true;
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

    bool get_device_power_limit_impl(unsigned int device_index, double &limit_watts)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device;
        nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if (result != NVML_SUCCESS)
            return false;

        unsigned int limit_mw;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetPowerManagementLimit",
            device,
            &limit_mw,
            result);
        if (result == NVML_SUCCESS)
        {
            limit_watts = limit_mw / 1000.0; // Convert from milliwatts to watts
            return true;
        }
        else if (result == NVML_ERROR_NOT_SUPPORTED)
        {
            limit_watts = 0.0; // Not supported
            OPTKIT_INFO("NVML does not support power limit query on this device");
            return true;
        }
#elif OPTKIT_ENV_LIB_ROCM_SMI
        // ROCm SMI does not provide a direct method to get power limit
        // This would require reading from sysfs or using other methods
        // For now, we return false indicating not implemented
#endif
        return false;
    }

    bool get_device_temperature_impl(unsigned int device_index, double &temp_celsius)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device;
        nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if (result != NVML_SUCCESS)
            return false;

        unsigned int temp;
        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
        if (result == NVML_SUCCESS)
        {
            temp_celsius = static_cast<double>(temp);
            return true;
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

    // NVIDIA device query helper functions
    bool get_device_info(unsigned int device_index, GpuDeviceInfo &info)
    {
#if OPTKIT_ENV_LIB_NVML
        nvmlDevice_t device;
        nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if (result != NVML_SUCCESS)
            return false;

        // Get device name
        char name[NVML_DEVICE_NAME_BUFFER_SIZE];
        result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
        if (result == NVML_SUCCESS)
        {
            info.basic.device_name = std::string(name);
            info.basic.name = info.basic.device_name;
        }

        // Get compute capability
        int major, minor;
        result = nvmlDeviceGetCudaComputeCapability(device, &major, &minor);
        if (result == NVML_SUCCESS)
        {
            info.compute.compute_capability_major = major;
            info.compute.compute_capability_minor = minor;
        }

        // Get memory information
        nvmlMemory_t memory;
        result = nvmlDeviceGetMemoryInfo(device, &memory);
        if (result == NVML_SUCCESS)
        {
            info.memory.total_global_memory_bytes = memory.total;
            info.memory.free_memory_bytes = memory.free;
            info.memory.used_memory_bytes = memory.used;
        }

        // Get memory bus width
        unsigned int busWidth;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMemoryBusWidth",
            device,
            &busWidth,
            result);

        if (result == NVML_SUCCESS)
        {
            info.memory.memory_bus_width_bits = busWidth;
        }
        else if (result == NVML_ERROR_NOT_SUPPORTED)
        {
            info.memory.memory_bus_width_bits = 0; // Not supported
            OPTKIT_INFO("NVML does not support memory bus width query on this device");
        }

        // Get multiprocessor count
        unsigned int multiProcessorCount;
        NVML_EXEC_IF_SUPPORTS(
            "nvmlDeviceGetMultiProcessorCount",
            device,
            &multiProcessorCount,
            result);
        if (result == NVML_SUCCESS)
        {
            info.compute.multiprocessor_count = multiProcessorCount;
        }
        else if (result == NVML_ERROR_NOT_SUPPORTED)
        {
            info.compute.multiprocessor_count = 0; // Not supported
            OPTKIT_INFO("NVML does not support multiprocessor count query on this device");
        }

        // Get clock rates
        unsigned int clockRate;
        result = nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_GRAPHICS, &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.boost_clock_rate_khz = clockRate * 1000; // Convert MHz to kHz
        }

        result = nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_MEM, &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.memory.memory_clock_rate_max_khz = clockRate * 1000; // Convert MHz to kHz
        }

        // Get current clocks
        result = nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_graphics_clock_mhz = clockRate;
        }

        result = nvmlDeviceGetClockInfo(device, NVML_CLOCK_MEM, &clockRate);
        if (result == NVML_SUCCESS)
        {
            info.clocks.current_memory_clock_mhz = clockRate;
        }

        // Get power information
        unsigned int power;
        result = nvmlDeviceGetPowerUsage(device, &power);
        if (result == NVML_SUCCESS)
        {
            info.power.current_power_watts = power / 1000.0;
            info.power.has_power_monitoring = true;
        }

        result = nvmlDeviceGetPowerManagementLimit(device, &power);
        if (result == NVML_SUCCESS)
        {
            info.power.power_limit_watts = power / 1000.0;
            info.power.max_power_watts = info.power.power_limit_watts;
            info.power.current_limit_watts = info.power.power_limit_watts;
        }

        // Get power management constraints (min/max/default)
        unsigned int min_power, max_power, default_power;
        result = nvmlDeviceGetPowerManagementLimitConstraints(device, &min_power, &max_power);
        if (result == NVML_SUCCESS)
        {
            info.power.min_power_watts = min_power / 1000.0;
            info.power.max_power_watts = max_power / 1000.0;
        }

        result = nvmlDeviceGetPowerManagementDefaultLimit(device, &default_power);
        if (result == NVML_SUCCESS)
        {
            info.power.default_power_watts = default_power / 1000.0;
        }

        // Check if power management is configurable
        nvmlEnableState_t power_management_state;
        result = nvmlDeviceGetPowerManagementMode(device, &power_management_state);
        if (result == NVML_SUCCESS)
        {
            info.power.is_configurable = (power_management_state == NVML_FEATURE_ENABLED);
        }

        // Get temperature
        unsigned int temp;
        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
        if (result == NVML_SUCCESS)
        {
            info.temperature.current_temperature_celsius = static_cast<double>(temp);
            info.temperature.has_temperature_monitoring = true;
        }

        // Get utilization
        nvmlUtilization_t utilization;
        result = nvmlDeviceGetUtilizationRates(device, &utilization);
        if (result == NVML_SUCCESS)
        {
            info.performance.gpu_utilization_percent = utilization.gpu;
            info.memory.memory_utilization_percent = utilization.memory;
            info.performance.has_utilization_monitoring = true;
        }

        // Get PCI information
        nvmlPciInfo_t pci;
        result = nvmlDeviceGetPciInfo(device, &pci);
        if (result == NVML_SUCCESS)
        {
            info.hardware.pci_bus_id = std::string(pci.busId);
            info.hardware.pci_device_id = pci.pciDeviceId;
            info.hardware.pci_subsystem_id = pci.pciSubSystemId;
        }

        // Get performance state
        nvmlPstates_t pstate;
        result = nvmlDeviceGetPerformanceState(device, &pstate);
        if (result == NVML_SUCCESS)
        {
            info.performance.performance_state = static_cast<uint32_t>(pstate);
        }

        // Get ECC mode
        nvmlEnableState_t current, pending;
        result = nvmlDeviceGetEccMode(device, &current, &pending);
        if (result == NVML_SUCCESS)
        {
            info.capabilities.ecc_enabled = (current == NVML_FEATURE_ENABLED);
        }

        // Get board ID for multi-GPU detection
        unsigned int boardId;
        result = nvmlDeviceGetBoardId(device, &boardId);
        if (result == NVML_SUCCESS)
        {
            info.hardware.board_id = boardId;
        }

        // Get persistence mode
        nvmlEnableState_t mode;
        result = nvmlDeviceGetPersistenceMode(device, &mode);
        if (result == NVML_SUCCESS)
        {
            info.capabilities.persistence_mode_enabled = (mode == NVML_FEATURE_ENABLED);
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

    GpuDeviceInfo Query::device_query(int32_t gpu_id)
    {
        GpuDeviceInfo info = {};
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

        if (gpu_id >= 0 && gpu_id < static_cast<int32_t>(get_device_count()))
            if (get_device_info(static_cast<unsigned int>(gpu_id), info))
                return info;

        // Fallback: try to get basic info from sysfs
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (optkit::utils::is_path_exists(drm_path))
            {
                std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);
                int32_t current_id = 0;

                for (const auto &entry_name : drm_entries)
                {
                    if (entry_name.find("card") == 0 && entry_name.find("-") == std::string::npos)
                    {
                        if (current_id == gpu_id)
                        {
                            std::string device_base_path = drm_path + "/" + entry_name + "/device";

                            // Get vendor information
                            std::string vendor_path = device_base_path + "/vendor";
                            if (optkit::utils::is_path_exists(vendor_path))
                            {
                                std::string vendor_id = optkit::utils::read_file(vendor_path);
                                vendor_id.erase(std::remove_if(vendor_id.begin(), vendor_id.end(), ::isspace), vendor_id.end());

                                if (vendor_id.find("0x10de") != std::string::npos)
                                {
                                    info.basic.vendor = GpuVendor::NVIDIA;
                                    info.basic.vendor_string = "NVIDIA";
                                    info.basic.name = "NVIDIA GPU (sysfs)";
                                }
                                else if (vendor_id.find("0x1002") != std::string::npos)
                                {
                                    info.basic.vendor = GpuVendor::AMD;
                                    info.basic.vendor_string = "AMD";
                                    info.basic.name = "AMD GPU (sysfs)";
                                }
                                else if (vendor_id.find("0x8086") != std::string::npos)
                                {
                                    info.basic.vendor = GpuVendor::INTEL;
                                    info.basic.vendor_string = "Intel";
                                    info.basic.name = "Intel GPU (sysfs)";
                                    info.basic.is_integrated = true;
                                }
                            }

                            // Get device ID
                            std::string device_id_path = device_base_path + "/device";
                            if (optkit::utils::is_path_exists(device_id_path))
                            {
                                std::string device_id = optkit::utils::read_file(device_id_path);
                                device_id.erase(std::remove_if(device_id.begin(), device_id.end(), ::isspace), device_id.end());
                                // Convert hex string to uint32_t
                                try
                                {
                                    info.hardware.pci_device_id = std::stoul(device_id, nullptr, 16);
                                }
                                catch (...)
                                {
                                    info.hardware.pci_device_id = 0;
                                }
                            }

                            break;
                        }
                        current_id++;
                    }
                }
            }
        }
        catch (...)
        {
            // Continue with default values
        }

        return info;
    }

    bool Query::is_nvidia_power_available()
    {
        // First try NVML
        if (get_device_count() > 0)
        {
            return true;
        }

        // Fallback to sysfs detection
        try
        {
            if (optkit::utils::is_path_exists("/proc/driver/nvidia/version"))
                return true;

            // Check for NVIDIA devices in DRM (check common card numbers)
            for (int i = 0; i < 16; ++i)
            {
                std::string vendor_path = "/sys/class/drm/card" + std::to_string(i) + "/device/vendor";
                if (optkit::utils::is_path_exists(vendor_path))
                {
                    std::string vendor = optkit::utils::read_file(vendor_path);
                    if (vendor.find("0x10de") != std::string::npos)
                    { // NVIDIA vendor ID
                        return true;
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    bool Query::is_amd_power_available()
    {
        // First try ROCm
        if (get_device_count() > 0)
        {
            return true;
        }

        // Fallback to sysfs detection
        try
        {
            // Check for AMD devices in DRM (check common card numbers)
            for (int i = 0; i < 16; ++i)
            {
                std::string vendor_path = "/sys/class/drm/card" + std::to_string(i) + "/device/vendor";
                if (optkit::utils::is_path_exists(vendor_path))
                {
                    std::string vendor = optkit::utils::read_file(vendor_path);
                    if (vendor.find("0x1002") != std::string::npos)
                    { // AMD vendor ID
                        // Check for power monitoring capability
                        std::string power_path = "/sys/class/drm/card" + std::to_string(i) + "/device/power1_average";
                        if (optkit::utils::is_path_exists(power_path))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    bool Query::is_intel_gpu_power_available()
    {
        // Check for Intel GPU power monitoring via i915/sysfs
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (optkit::utils::is_path_exists(drm_path))
            {
                std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);
                for (const auto &entry_name : drm_entries)
                {
                    if (entry_name.find("card") == 0) // Check if it starts with "card"
                    {
                        std::string device_path = drm_path + "/" + entry_name + "/device";
                        std::string vendor_path = device_path + "/vendor";

                        if (optkit::utils::is_path_exists(vendor_path))
                        {
                            std::string vendor_content = optkit::utils::read_file(vendor_path);
                            // Intel vendor ID is 0x8086
                            if (vendor_content.find("0x8086") != std::string::npos)
                            {
                                // Check for Intel i915 power monitoring
                                std::string power_path = device_path + "/power1_average";
                                if (optkit::utils::is_path_exists(power_path))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    std::vector<GpuDeviceInfo> Query::get_power_capable_gpus()
    {
        std::vector<GpuDeviceInfo> gpus;
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (!optkit::utils::is_path_exists(drm_path))
            {
                return gpus;
            }

            int32_t gpu_id = 0;
            std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);

            for (const auto &entry_name : drm_entries)
            {
                // Only process actual card entries (not render nodes, control nodes, etc.)
                if (entry_name.find("card") == 0 && entry_name.find("-") == std::string::npos)
                {
                    // Extract card number and verify it's a base card (not a sub-device)
                    std::string card_num_str = entry_name.substr(4); // Remove "card" prefix
                    bool is_digit_only = !card_num_str.empty() &&
                                         std::all_of(card_num_str.begin(), card_num_str.end(), ::isdigit);

                    if (!is_digit_only)
                        continue; // Skip non-numeric card entries

                    GpuDeviceInfo gpu_info = {};
                    gpu_info.basic.id = gpu_id++;

                    std::string device_base_path = drm_path + "/" + entry_name + "/device";

                    // Read vendor ID
                    std::string vendor_path = device_base_path + "/vendor";
                    if (optkit::utils::is_path_exists(vendor_path))
                    {
                        std::string vendor_id = optkit::utils::read_file(vendor_path);
                        // Remove whitespace and newlines
                        vendor_id.erase(std::remove_if(vendor_id.begin(), vendor_id.end(), ::isspace), vendor_id.end());

                        if (vendor_id.find("0x10de") != std::string::npos)
                        {
                            gpu_info.basic.vendor = GpuVendor::NVIDIA;
                        }
                        else if (vendor_id.find("0x1002") != std::string::npos)
                        {
                            gpu_info.basic.vendor = GpuVendor::AMD;
                        }
                        else if (vendor_id.find("0x8086") != std::string::npos)
                        {
                            gpu_info.basic.vendor = GpuVendor::INTEL;
                        }
                        else
                        {
                            gpu_info.basic.vendor = GpuVendor::UNKNOWN;
                        }
                    }
                    else
                    {
                        gpu_info.basic.vendor = GpuVendor::UNKNOWN;
                    }

                    // Try to read GPU device ID for name identification
                    std::string device_id_path = device_base_path + "/device";
                    if (optkit::utils::is_path_exists(device_id_path))
                    {
                        std::string device_id = optkit::utils::read_file(device_id_path);
                        device_id.erase(std::remove_if(device_id.begin(), device_id.end(), ::isspace), device_id.end());
                        gpu_info.basic.name = device_id;
                    }

                    // Try to get a more descriptive name from modalias or other sources
                    std::string modalias_path = device_base_path + "/modalias";
                    if (optkit::utils::is_path_exists(modalias_path))
                    {
                        std::string modalias = optkit::utils::read_file(modalias_path);
                        // Extract useful info from modalias if available
                        if (!modalias.empty() && gpu_info.basic.name.empty())
                        {
                            gpu_info.basic.name = "GPU Device";
                        }
                    }

                    // Fallback name based on vendor
                    if (gpu_info.basic.name.empty())
                    {
                        switch (gpu_info.basic.vendor)
                        {
                        case GpuVendor::NVIDIA:
                            gpu_info.basic.name = "NVIDIA GPU";
                            break;
                        case GpuVendor::AMD:
                            gpu_info.basic.name = "AMD GPU";
                            break;
                        case GpuVendor::INTEL:
                            gpu_info.basic.name = "Intel GPU";
                            break;
                        default:
                            gpu_info.basic.name = "Unknown GPU";
                            break;
                        }
                    }

                    // Check power monitoring capabilities - prioritize vendor-specific APIs
                    gpu_info.power.has_power_monitoring = false;
                    gpu_info.power.current_power_watts = 0.0;
                    gpu_info.power.max_power_watts = 0.0;
                    std::string active_power_path;
#if OPTKIT_ENV_LIB_NVML
                    // For NVIDIA GPUs: Use NVML exclusively if available
                    // Try NVML first - iterate through NVML devices to find matching one
                    unsigned int nvml_device_count = get_device_count();
                    bool found_via_nvml = false;

                    for (unsigned int nvml_idx = 0; nvml_idx < nvml_device_count && !found_via_nvml; nvml_idx++)
                    {
                        uint32_t power_watts;
                        nvmlReturn_t result;
                        NVML_EXEC_IF_SUPPORTS(
                            "get_nvidia_device_power",
                            gpu_handles.at(nvml_idx),
                            &power_watts,
                            result);

                        if (result == NVML_SUCCESS)
                        {
                            gpu_info.power.has_power_monitoring = true;
                            gpu_info.power.current_power_watts = power_watts;
                            found_via_nvml = true;

                            // Try to get power limit via NVML
                            uint32_t limit_watts;
                            NVML_EXEC_IF_SUPPORTS(
                                "get_nvidia_device_power_limit",
                                gpu_handles.at(nvml_idx),
                                &limit_watts,
                                result);

                            if (result == NVML_SUCCESS)
                            {
                                gpu_info.power.max_power_watts = limit_watts;
                            }

                            // Try to get temperature via NVML
                            uint32_t temp_celsius;
                            NVML_EXEC_IF_SUPPORTS(
                                "get_nvidia_device_temperature",
                                gpu_handles.at(nvml_idx),
                                &temp_celsius,
                                result);

                            if (result == NVML_SUCCESS)
                            {
                                gpu_info.temperature.has_temperature_monitoring = true;
                            }

                            // Update name to indicate NVML source
                            if (gpu_info.basic.name.find("NVIDIA") == std::string::npos)
                            {
                                gpu_info.basic.name = "NVIDIA GPU (NVML)";
                            }
                            break; // Found the first working NVML device for this GPU
                        }
                    } // For NVIDIA: Don't fall back to sysfs - NVML is the authoritative source
                    if (!found_via_nvml)
                    {
                        // NVML not available or no power monitoring - this is expected
                        gpu_info.power.has_power_monitoring = false;
                        gpu_info.power.current_power_watts = 0.0;
                    }
#elif OPTKIT_ENV_LIB_ROCM_SMI
                    // For AMD GPUs: Use ROCm if available, otherwise sysfs
                    // Try ROCm first - iterate through ROCm devices
                    uint32_t rocm_device_count = get_device_count();
                    bool found_via_rocm = false;

                    for (uint32_t rocm_idx = 0; rocm_idx < rocm_device_count && !found_via_rocm; rocm_idx++)
                    {
                        double power_watts;
                        nvmlReturn_t result;

                        if (get_device_power_impl(rocm_idx, power_watts))
                        {
                            gpu_info.power.has_power_monitoring = true;
                            gpu_info.power.current_power_watts = power_watts;
                            found_via_rocm = true;

                            // Try to get temperature via ROCm
                            double temp_celsius;
                            if (get_device_temperature_impl(rocm_idx, temp_celsius))
                            {
                                gpu_info.temperature.has_temperature_monitoring = true;
                            }

                            // Update name to indicate ROCm source
                            if (gpu_info.basic.name.find("AMD") == std::string::npos)
                            {
                                gpu_info.basic.name = "AMD GPU (ROCm)";
                            }
                            break; // Found the first working ROCm device for this GPU
                        }
                    }

                    // For AMD: Fall back to sysfs if ROCm not available
                    if (!found_via_rocm)
                    {
                        // Try sysfs as fallback for AMD
                        std::vector<std::string> power_files = {
                            device_base_path + "/power1_average",
                            device_base_path + "/power1_input",
                            device_base_path + "/power_average",
                            device_base_path + "/power_input"};

                        // Check direct power files first
                        for (const auto &power_path : power_files)
                        {
                            if (optkit::utils::is_path_exists(power_path))
                            {
                                gpu_info.has_power_monitoring = true;
                                active_power_path = power_path;
                                break;
                            }
                        }

                        // If no direct power files, check in hwmon subdirectories
                        if (!gpu_info.has_power_monitoring)
                        {
                            std::string hwmon_path = device_base_path + "/hwmon";
                            if (optkit::utils::is_path_exists(hwmon_path))
                            {
                                std::vector<std::string> hwmon_entries = optkit::utils::get_all_files(hwmon_path);
                                for (const auto &hwmon_entry : hwmon_entries)
                                {
                                    if (hwmon_entry.find("hwmon") == 0)
                                    {
                                        std::string hwmon_subdir = hwmon_path + "/" + hwmon_entry;
                                        for (const auto &pf : {"power1_average", "power1_input"})
                                        {
                                            std::string hwmon_power_path = hwmon_subdir + "/" + pf;
                                            if (optkit::utils::is_path_exists(hwmon_power_path))
                                            {
                                                gpu_info.has_power_monitoring = true;
                                                active_power_path = hwmon_power_path;
                                                break;
                                            }
                                        }
                                        if (gpu_info.has_power_monitoring)
                                            break;
                                    }
                                }
                            }
                        }

                        // Read power from sysfs if we found a path and ROCm didn't work
                        if (gpu_info.has_power_monitoring && !active_power_path.empty())
                        {
                            try
                            {
                                std::string power_str = optkit::utils::read_file(active_power_path);
                                power_str.erase(std::remove_if(power_str.begin(), power_str.end(), ::isspace), power_str.end());
                                gpu_info.power.current_power_watts = std::stod(power_str) / 1000000.0; // Convert from microwatts

                                // Update name to indicate sysfs source
                                if (gpu_info.basic.name.find("sysfs") == std::string::npos)
                                {
                                    gpu_info.basic.name = gpu_info.basic.name + " (sysfs)";
                                }
                            }
                            catch (...)
                            {
                                gpu_info.current_power_watts = 0.0;
                            }
                        }
                    }
#else
                    // For Intel and other GPUs: Use sysfs only
                    // Primary power monitoring files to check
                    std::vector<std::string> power_files = {
                        device_base_path + "/power1_average",
                        device_base_path + "/power1_input",
                        device_base_path + "/power_average",
                        device_base_path + "/power_input"};

                    // Check direct power files first
                    for (const auto &power_path : power_files)
                    {
                        if (optkit::utils::is_path_exists(power_path))
                        {
                            gpu_info.has_power_monitoring = true;
                            active_power_path = power_path;
                            break;
                        }
                    }

                    // If no direct power files, check in hwmon subdirectories
                    if (!gpu_info.has_power_monitoring)
                    {
                        std::string hwmon_path = device_base_path + "/hwmon";
                        if (optkit::utils::is_path_exists(hwmon_path))
                        {
                            std::vector<std::string> hwmon_entries = optkit::utils::get_all_files(hwmon_path);
                            for (const auto &hwmon_entry : hwmon_entries)
                            {
                                if (hwmon_entry.find("hwmon") == 0)
                                {
                                    std::string hwmon_subdir = hwmon_path + "/" + hwmon_entry;
                                    for (const auto &pf : {"power1_average", "power1_input"})
                                    {
                                        std::string hwmon_power_path = hwmon_subdir + "/" + pf;
                                        if (optkit::utils::is_path_exists(hwmon_power_path))
                                        {
                                            gpu_info.has_power_monitoring = true;
                                            active_power_path = hwmon_power_path;
                                            break;
                                        }
                                    }
                                    if (gpu_info.has_power_monitoring)
                                        break;
                                }
                            }
                        }
                    }

                    // Read power from sysfs if we found a path
                    if (gpu_info.has_power_monitoring && !active_power_path.empty())
                    {
                        try
                        {
                            std::string power_str = optkit::utils::read_file(active_power_path);
                            power_str.erase(std::remove_if(power_str.begin(), power_str.end(), ::isspace), power_str.end());
                            gpu_info.current_power_watts = std::stod(power_str) / 1000000.0; // Convert from microwatts
                        }
                        catch (...)
                        {
                            gpu_info.current_power_watts = 0.0;
                        }
                    }
#endif
                    // Check for frequency control (enhanced check)
                    gpu_info.clocks.has_frequency_control = false;
                    if (gpu_info.basic.vendor == GpuVendor::INTEL)
                    {
                        std::string freq_path = drm_path + "/" + entry_name + "/gt/gt0/rps";
                        gpu_info.clocks.has_frequency_control = optkit::utils::is_path_exists(freq_path);
                    }
                    // For NVIDIA and AMD, frequency control is typically available through their drivers
                    else if (gpu_info.basic.vendor == GpuVendor::NVIDIA || gpu_info.basic.vendor == GpuVendor::AMD)
                    {
                        // Check for basic frequency info
                        std::string freq_path = device_base_path + "/current_link_speed";
                        gpu_info.clocks.has_frequency_control = optkit::utils::is_path_exists(freq_path);
                    }

                    // Check temperature monitoring via sysfs only if vendor APIs haven't already detected it
                    if (!gpu_info.temperature.has_temperature_monitoring)
                    {
                        std::string hwmon_path = device_base_path + "/hwmon";
                        if (optkit::utils::is_path_exists(hwmon_path))
                        {
                            std::vector<std::string> hwmon_entries = optkit::utils::get_all_files(hwmon_path);
                            for (const auto &hwmon_entry : hwmon_entries)
                            {
                                if (hwmon_entry.find("hwmon") == 0)
                                {
                                    std::string temp_path = hwmon_path + "/" + hwmon_entry + "/temp1_input";
                                    if (optkit::utils::is_path_exists(temp_path))
                                    {
                                        gpu_info.temperature.has_temperature_monitoring = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    // Only add GPUs that have some form of monitoring capability or are from known vendors
                    if (gpu_info.power.has_power_monitoring || gpu_info.temperature.has_temperature_monitoring ||
                        gpu_info.basic.vendor != GpuVendor::UNKNOWN)
                    {
                        gpus.push_back(gpu_info);
                    }
                }
            }
        }
        catch (...)
        {
            // Return what we have so far
        }

        return gpus;
    }

    int32_t Query::get_available_power_methods()
    {
        int32_t methods = static_cast<int32_t>(GpuPowerMethod::NONE);

        if (is_nvidia_power_available())
        {
            methods |= static_cast<int32_t>(GpuPowerMethod::NVIDIA_ML);
        }

        if (is_amd_power_available())
        {
            methods |= static_cast<int32_t>(GpuPowerMethod::AMD_ROCM);
        }

        if (is_intel_gpu_power_available())
        {
            methods |= static_cast<int32_t>(GpuPowerMethod::INTEL_I915);
        }

        // Check for generic hwmon interfaces
        try
        {
            if (optkit::utils::is_path_exists("/sys/class/hwmon"))
            {
                std::vector<std::string> hwmon_entries = optkit::utils::get_all_files("/sys/class/hwmon");
                for (const auto &entry_name : hwmon_entries)
                {
                    std::string name_path = "/sys/class/hwmon/" + entry_name + "/name";
                    if (optkit::utils::is_path_exists(name_path))
                    {
                        std::string name = optkit::utils::read_file(name_path);
                        if (name.find("gpu") != std::string::npos || name.find("GPU") != std::string::npos)
                        {
                            methods |= static_cast<int32_t>(GpuPowerMethod::SYSFS_HWMON);
                            break;
                        }
                    }
                }
            }
        }
        catch (...)
        {
            // Ignore hwmon check failure
        }

        return methods;
    }

    bool Query::is_gpu_frequency_control_available()
    {
        // Check for Intel GPU frequency control
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (optkit::utils::is_path_exists(drm_path))
            {
                std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);
                for (const auto &entry_name : drm_entries)
                {
                    if (entry_name.find("card") == 0) // Check if it starts with "card"
                    {
                        std::string gt_path = drm_path + "/" + entry_name + "/gt/gt0/rps";
                        if (optkit::utils::is_path_exists(gt_path))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    bool Query::is_gpu_temperature_available()
    {
        // First check if vendor APIs (NVML/ROCm) are available
#if OPTKIT_ENV_LIB_NVML
        if (get_device_count() > 0)
        {
            // Check if any NVML device supports temperature
            for (unsigned int i = 0; i < get_device_count(); i++)
            {
                double temp;
                if (get_device_temperature_impl(i, temp))
                {
                    return true;
                }
            }
        }
#endif

#if OPTKIT_ENV_LIB_ROCM_SMI
        if (get_device_count() > 0)
        {
            // Check if any ROCm device supports temperature
            for (uint32_t i = 0; i < get_device_count(); i++)
            {
                double temp;
                if (get_device_temperature_impl(i, temp))
                {
                    return true;
                }
            }
        }
#endif

        // Fallback to sysfs detection
        try
        {
            if (optkit::utils::is_path_exists("/sys/class/hwmon"))
            {
                std::vector<std::string> hwmon_entries = optkit::utils::get_all_files("/sys/class/hwmon");
                for (const auto &entry_name : hwmon_entries)
                {
                    std::string name_path = "/sys/class/hwmon/" + entry_name + "/name";
                    if (optkit::utils::is_path_exists(name_path))
                    {
                        std::string name = optkit::utils::read_file(name_path);
                        if (name.find("gpu") != std::string::npos || name.find("GPU") != std::string::npos)
                        {
                            // Check for temperature sensors
                            std::string temp_path = "/sys/class/hwmon/" + entry_name + "/temp1_input";
                            if (optkit::utils::is_path_exists(temp_path))
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    bool Query::is_gpu_utilization_monitoring_available()
    {
        // Check for GPU utilization monitoring capabilities
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (optkit::utils::is_path_exists(drm_path))
            {
                std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);
                for (const auto &entry_name : drm_entries)
                {
                    if (entry_name.find("card") == 0) // Check if it starts with "card"
                    {
                        // Check for Intel GPU busy stats
                        std::string busy_path = drm_path + "/" + entry_name + "/gt/gt0/rps/busy";
                        if (optkit::utils::is_path_exists(busy_path))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    bool Query::is_gpu_memory_power_available()
    {
        // Check for separate GPU memory power monitoring
        try
        {
            const std::string drm_path = "/sys/class/drm";
            if (optkit::utils::is_path_exists(drm_path))
            {
                std::vector<std::string> drm_entries = optkit::utils::get_all_files(drm_path);
                for (const auto &entry_name : drm_entries)
                {
                    if (entry_name.find("card") == 0) // Check if it starts with "card"
                    {
                        // Check for memory power monitoring (implementation specific)
                        std::string mem_power_path = drm_path + "/" + entry_name + "/device/power2_average";
                        if (optkit::utils::is_path_exists(mem_power_path))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }

    SystemGpuPowerInfo Query::get_system_gpu_power_info()
    {
        SystemGpuPowerInfo info = {};

        auto gpus = get_power_capable_gpus();
        info.num_power_monitored_gpus = 0;
        info.current_gpu_power_usage_watts = 0.0;

        for (const auto &gpu : gpus)
        {
            if (gpu.power.has_power_monitoring)
            {
                info.num_power_monitored_gpus++;
                info.current_gpu_power_usage_watts += gpu.power.current_power_watts;
                info.total_gpu_power_budget_watts += gpu.power.max_power_watts;
            }
        }

        info.available_gpu_power_headroom_watts =
            info.total_gpu_power_budget_watts - info.current_gpu_power_usage_watts;

        return info;
    }

} // namespace optkit::gpu
