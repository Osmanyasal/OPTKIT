
#include "core/energy/gpu/query.hh"
#include "utils/logging/logger.hh"
#include <fstream>
#include <cstdlib>
#include <regex>

// Conditional includes for vendor-specific libraries
#if OPTKIT_ENV_LIB_NVML
#include <nvml.h>
#endif

#if OPTKIT_ENV_LIB_ROCM_SMI
#include <rocm_smi.h>
#endif

namespace optkit::energy::gpu
{
    // NVML helper functions
    namespace nvml
    {
        static bool initialized = false;

        bool init()
        {
#if OPTKIT_ENV_LIB_NVML
            if (!initialized)
            {
                nvmlReturn_t result = nvmlInit();
                initialized = (result == NVML_SUCCESS);
            }
            return initialized;
#else
            return false;
#endif
        }

        void shutdown()
        {
#if OPTKIT_ENV_LIB_NVML
            if (initialized)
            {
                nvmlShutdown();
                initialized = false;
            }
#endif
        }

        bool get_device_power(unsigned int device_index, double &power_watts)
        {
#if OPTKIT_ENV_LIB_NVML
            if (!init())
                return false;

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
#endif
            return false;
        }

        bool get_device_power_limit(unsigned int device_index, double &limit_watts)
        {
#if OPTKIT_ENV_LIB_NVML
            if (!init())
                return false;

            nvmlDevice_t device;
            nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_index, &device);
            if (result != NVML_SUCCESS)
                return false;

            unsigned int limit_mw;
            result = nvmlDeviceGetPowerManagementLimit(device, &limit_mw);
            if (result == NVML_SUCCESS)
            {
                limit_watts = limit_mw / 1000.0; // Convert from milliwatts to watts
                return true;
            }
#endif
            return false;
        }

        bool get_device_temperature(unsigned int device_index, double &temp_celsius)
        {
#if OPTKIT_ENV_LIB_NVML
            if (!init())
                return false;

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
#endif
            return false;
        }

        unsigned int get_device_count()
        {
#if OPTKIT_ENV_LIB_NVML
            if (!init())
                return 0;

            unsigned int count;
            nvmlReturn_t result = nvmlDeviceGetCount(&count);
            return (result == NVML_SUCCESS) ? count : 0;
#else
            return 0;
#endif
        }
    }

    // ROCm helper functions
    namespace rocm
    {
        static bool initialized = false;

        bool init()
        {
#if OPTKIT_ENV_LIB_ROCM_SMI
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

        void shutdown()
        {
#if OPTKIT_ENV_LIB_ROCM_SMI
            if (initialized)
            {
                rsmi_shut_down();
                initialized = false;
            }
#endif
        }

        bool get_device_power(uint32_t device_index, double &power_watts)
        {
#if OPTKIT_ENV_LIB_ROCM_SMI
            if (!init())
                return false;

            uint64_t power_uw;
            rsmi_status_t result = rsmi_dev_power_ave_get(device_index, 0, &power_uw);
            if (result == RSMI_STATUS_SUCCESS)
            {
                power_watts = power_uw / 1000000.0; // Convert from microwatts to watts
                return true;
            }
#endif
            return false;
        }

        bool get_device_temperature(uint32_t device_index, double &temp_celsius)
        {
#if OPTKIT_ENV_LIB_ROCM_SMI
            if (!init())
                return false;

            int64_t temp_millidegrees;
            rsmi_status_t result = rsmi_dev_temp_metric_get(device_index, RSMI_TEMP_TYPE_EDGE, RSMI_TEMP_CURRENT, &temp_millidegrees);
            if (result == RSMI_STATUS_SUCCESS)
            {
                temp_celsius = temp_millidegrees / 1000.0; // Convert from millidegrees to degrees Celsius
                return true;
            }
#endif
            return false;
        }

        uint32_t get_device_count()
        {
#if OPTKIT_ENV_LIB_ROCM_SMI
            if (!init())
                return 0;

            uint32_t count;
            rsmi_status_t result = rsmi_num_monitor_devices(&count);
            return (result == RSMI_STATUS_SUCCESS) ? count : 0;
#else
            return 0;
#endif
        }
    }
    std::string to_string(GpuVendor vendor)
    {
        switch (vendor)
        {
        case GpuVendor::NVIDIA:
            return "NVIDIA";
        case GpuVendor::AMD:
            return "AMD";
        case GpuVendor::INTEL:
            return "Intel";
        case GpuVendor::ARM_MALI:
            return "ARM Mali";
        case GpuVendor::QUALCOMM_ADRENO:
            return "Qualcomm Adreno";
        case GpuVendor::IMAGINATION_POWERVR:
            return "Imagination PowerVR";
        case GpuVendor::UNKNOWN:
        default:
            return "Unknown";
        }
    }

    GpuVendor vendor_from_string(const std::string &vendor_name)
    {
        std::string lower = vendor_name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (lower.find("nvidia") != std::string::npos)
            return GpuVendor::NVIDIA;
        if (lower.find("amd") != std::string::npos || lower.find("ati") != std::string::npos)
            return GpuVendor::AMD;
        if (lower.find("intel") != std::string::npos)
            return GpuVendor::INTEL;
        if (lower.find("mali") != std::string::npos)
            return GpuVendor::ARM_MALI;
        if (lower.find("adreno") != std::string::npos)
            return GpuVendor::QUALCOMM_ADRENO;
        if (lower.find("powervr") != std::string::npos)
            return GpuVendor::IMAGINATION_POWERVR;

        return GpuVendor::UNKNOWN;
    }

    std::ostream &operator<<(std::ostream &os, GpuVendor vendor)
    {
        os << to_string(vendor);
        return os;
    }

    bool Query::is_nvidia_power_available()
    {
        // First try NVML
        if (nvml::get_device_count() > 0)
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
        if (rocm::get_device_count() > 0)
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
                    gpu_info.id = gpu_id++;

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
                            gpu_info.vendor = GpuVendor::NVIDIA;
                        }
                        else if (vendor_id.find("0x1002") != std::string::npos)
                        {
                            gpu_info.vendor = GpuVendor::AMD;
                        }
                        else if (vendor_id.find("0x8086") != std::string::npos)
                        {
                            gpu_info.vendor = GpuVendor::INTEL;
                        }
                        else
                        {
                            gpu_info.vendor = GpuVendor::UNKNOWN;
                        }
                    }
                    else
                    {
                        gpu_info.vendor = GpuVendor::UNKNOWN;
                    }

                    // Try to read GPU device ID for name identification
                    std::string device_id_path = device_base_path + "/device";
                    if (optkit::utils::is_path_exists(device_id_path))
                    {
                        std::string device_id = optkit::utils::read_file(device_id_path);
                        device_id.erase(std::remove_if(device_id.begin(), device_id.end(), ::isspace), device_id.end());
                        gpu_info.name = device_id;
                    }

                    // Try to get a more descriptive name from modalias or other sources
                    std::string modalias_path = device_base_path + "/modalias";
                    if (optkit::utils::is_path_exists(modalias_path))
                    {
                        std::string modalias = optkit::utils::read_file(modalias_path);
                        // Extract useful info from modalias if available
                        if (!modalias.empty() && gpu_info.name.empty())
                        {
                            gpu_info.name = "GPU Device";
                        }
                    }

                    // Fallback name based on vendor
                    if (gpu_info.name.empty())
                    {
                        switch (gpu_info.vendor)
                        {
                        case GpuVendor::NVIDIA:
                            gpu_info.name = "NVIDIA GPU";
                            break;
                        case GpuVendor::AMD:
                            gpu_info.name = "AMD GPU";
                            break;
                        case GpuVendor::INTEL:
                            gpu_info.name = "Intel GPU";
                            break;
                        default:
                            gpu_info.name = "Unknown GPU";
                            break;
                        }
                    }

                    // Check power monitoring capabilities - prioritize vendor-specific APIs
                    gpu_info.has_power_monitoring = false;
                    gpu_info.current_power_watts = 0.0;
                    gpu_info.max_power_watts = 0.0;
                    std::string active_power_path;

                    // For NVIDIA GPUs: Use NVML exclusively if available
                    if (gpu_info.vendor == GpuVendor::NVIDIA)
                    {
                        // Try NVML first - iterate through NVML devices to find matching one
                        unsigned int nvml_device_count = nvml::get_device_count();
                        bool found_via_nvml = false;

                        for (unsigned int nvml_idx = 0; nvml_idx < nvml_device_count && !found_via_nvml; nvml_idx++)
                        {
                            double power_watts;
                            if (nvml::get_device_power(nvml_idx, power_watts))
                            {
                                gpu_info.has_power_monitoring = true;
                                gpu_info.current_power_watts = power_watts;
                                found_via_nvml = true;

                                // Try to get power limit via NVML
                                double limit_watts;
                                if (nvml::get_device_power_limit(nvml_idx, limit_watts))
                                {
                                    gpu_info.max_power_watts = limit_watts;
                                }

                                // Try to get temperature via NVML
                                double temp_celsius;
                                if (nvml::get_device_temperature(nvml_idx, temp_celsius))
                                {
                                    gpu_info.has_temperature_monitoring = true;
                                }

                                // Update name to indicate NVML source
                                if (gpu_info.name.find("NVIDIA") == std::string::npos)
                                {
                                    gpu_info.name = "NVIDIA GPU (NVML)";
                                }
                                break; // Found the first working NVML device for this GPU
                            }
                        } // For NVIDIA: Don't fall back to sysfs - NVML is the authoritative source
                        if (!found_via_nvml)
                        {
                            // NVML not available or no power monitoring - this is expected
                            gpu_info.has_power_monitoring = false;
                            gpu_info.current_power_watts = 0.0;
                        }
                    }
                    // For AMD GPUs: Use ROCm if available, otherwise sysfs
                    else if (gpu_info.vendor == GpuVendor::AMD)
                    {
                        // Try ROCm first - iterate through ROCm devices
                        uint32_t rocm_device_count = rocm::get_device_count();
                        bool found_via_rocm = false;

                        for (uint32_t rocm_idx = 0; rocm_idx < rocm_device_count && !found_via_rocm; rocm_idx++)
                        {
                            double power_watts;
                            if (rocm::get_device_power(rocm_idx, power_watts))
                            {
                                gpu_info.has_power_monitoring = true;
                                gpu_info.current_power_watts = power_watts;
                                found_via_rocm = true;

                                // Try to get temperature via ROCm
                                double temp_celsius;
                                if (rocm::get_device_temperature(rocm_idx, temp_celsius))
                                {
                                    gpu_info.has_temperature_monitoring = true;
                                }

                                // Update name to indicate ROCm source
                                if (gpu_info.name.find("AMD") == std::string::npos)
                                {
                                    gpu_info.name = "AMD GPU (ROCm)";
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
                                    gpu_info.current_power_watts = std::stod(power_str) / 1000000.0; // Convert from microwatts

                                    // Update name to indicate sysfs source
                                    if (gpu_info.name.find("sysfs") == std::string::npos)
                                    {
                                        gpu_info.name = gpu_info.name + " (sysfs)";
                                    }
                                }
                                catch (...)
                                {
                                    gpu_info.current_power_watts = 0.0;
                                }
                            }
                        }
                    }
                    // For Intel and other GPUs: Use sysfs only
                    else
                    {
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
                    }

                    // Check for frequency control (enhanced check)
                    gpu_info.has_frequency_control = false;
                    if (gpu_info.vendor == GpuVendor::INTEL)
                    {
                        std::string freq_path = drm_path + "/" + entry_name + "/gt/gt0/rps";
                        gpu_info.has_frequency_control = optkit::utils::is_path_exists(freq_path);
                    }
                    // For NVIDIA and AMD, frequency control is typically available through their drivers
                    else if (gpu_info.vendor == GpuVendor::NVIDIA || gpu_info.vendor == GpuVendor::AMD)
                    {
                        // Check for basic frequency info
                        std::string freq_path = device_base_path + "/current_link_speed";
                        gpu_info.has_frequency_control = optkit::utils::is_path_exists(freq_path);
                    }

                    // Check temperature monitoring via sysfs only if vendor APIs haven't already detected it
                    if (!gpu_info.has_temperature_monitoring)
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
                                        gpu_info.has_temperature_monitoring = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    // Only add GPUs that have some form of monitoring capability or are from known vendors
                    if (gpu_info.has_power_monitoring || gpu_info.has_temperature_monitoring ||
                        gpu_info.vendor != GpuVendor::UNKNOWN)
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

    GpuPowerLimits Query::get_gpu_power_limits(int32_t gpu_id)
    {
        GpuPowerLimits limits = {};
        // Implementation would query specific GPU power limits
        // This is a placeholder
        return limits;
    }

    bool Query::is_gpu_temperature_available()
    {
        // First check if vendor APIs (NVML/ROCm) are available
        if (nvml::get_device_count() > 0)
        {
            // Check if any NVML device supports temperature
            for (unsigned int i = 0; i < nvml::get_device_count(); i++)
            {
                double temp;
                if (nvml::get_device_temperature(i, temp))
                {
                    return true;
                }
            }
        }

        if (rocm::get_device_count() > 0)
        {
            // Check if any ROCm device supports temperature
            for (uint32_t i = 0; i < rocm::get_device_count(); i++)
            {
                double temp;
                if (rocm::get_device_temperature(i, temp))
                {
                    return true;
                }
            }
        }

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
            if (gpu.has_power_monitoring)
            {
                info.num_power_monitored_gpus++;
                info.current_gpu_power_usage_watts += gpu.current_power_watts;
                info.total_gpu_power_budget_watts += gpu.max_power_watts;
            }
        }

        info.available_gpu_power_headroom_watts =
            info.total_gpu_power_budget_watts - info.current_gpu_power_usage_watts;

        return info;
    }

    // Cleanup function to properly shutdown vendor libraries
    void Query::cleanup_vendor_libraries()
    {
#if OPTKIT_ENV_LIB_NVML
        nvml::shutdown();
#elif OPTKIT_ENV_LIB_ROCM_SMI
        rocm::shutdown();
#endif
    }

} // namespace optkit::energy::gpu
