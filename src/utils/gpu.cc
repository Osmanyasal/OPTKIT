#include "utils/gpu.hh"

namespace optkit::gpu
{

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

    std::string to_string(const GpuPowerMethod &method)
    {
        if (method == GpuPowerMethod::NONE)
            return "[\"NONE\"]";

        std::string result = "[";
        bool first = true;

        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::NVIDIA_ML))
        {
            if (!first)
                result += ",";
            result += "\"NVIDIA_ML\"";
            first = false;
        }
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::AMD_ROCM))
        {
            if (!first)
                result += ",";
            result += "\"AMD_ROCM\"";
            first = false;
        }
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::INTEL_I915))
        {
            if (!first)
                result += ",";
            result += "\"INTEL_I915\"";
            first = false;
        }
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::SYSFS_HWMON))
        {
            if (!first)
                result += ",";
            result += "\"SYSFS_HWMON\"";
            first = false;
        }
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::PERF_UNCORE))
        {
            if (!first)
                result += ",";
            result += "\"PERF_UNCORE\"";
            first = false;
        }

        result += "]";
        return first ? "[\"NONE\"]" : result;
    }

    std::string to_string(const GpuBasicInfo &info)
    {
        return "{\"id\":" + std::to_string(info.id) +
               ",\"name\":\"" + info.name + "\"" +
               ",\"device_name\":\"" + info.device_name + "\"" +
               ",\"vendor\":\"" + to_string(info.vendor) + "\"" +
               ",\"vendor_string\":\"" + info.vendor_string + "\"" +
               ",\"is_integrated\":" + (info.is_integrated ? "true" : "false") + "}";
    }

    std::string to_string(const GpuVersionInfo &info)
    {
        return "{\"driver_major_minor\":" + std::to_string(info.driver_major_minor) +
               ",\"driver_version\":\"" + info.driver_version_string + "\"" +
               ",\"library_version\":\"" + info.library_version_string + "\"}";
    }

    std::string to_string(const GpuComputeInfo &info)
    {
        return "{\"compute_capability_major\":" + std::to_string(info.compute_capability_major) +
               ",\"compute_capability_minor\":" + std::to_string(info.compute_capability_minor) +
               ",\"multiprocessor_count\":" + std::to_string(info.multiprocessor_count) +
               ",\"cuda_cores_per_mp\":" + std::to_string(info.cuda_cores_per_mp) +
               ",\"total_cuda_cores\":" + std::to_string(info.total_cuda_cores) +
               ",\"warp_size\":" + std::to_string(info.warp_size) + "}";
    }

    std::string to_string(const GpuMemoryInfo &info)
    {
        return "{\"total_global_memory_bytes\":" + std::to_string(info.total_global_memory_bytes) +
               ",\"free_memory_bytes\":" + std::to_string(info.free_memory_bytes) +
               ",\"used_memory_bytes\":" + std::to_string(info.used_memory_bytes) +
               ",\"memory_bus_width_bits\":" + std::to_string(info.memory_bus_width_bits) +
               ",\"memory_clock_rate_khz\":" + std::to_string(info.memory_clock_rate_khz) +
               ",\"memory_clock_rate_max_khz\":" + std::to_string(info.memory_clock_rate_max_khz) +
               ",\"memory_utilization_percent\":" + std::to_string(info.memory_utilization_percent) + "}";
    }

    std::string to_string(const GpuClockInfo &info)
    {
        return "{\"base_clock_rate_khz\":" + std::to_string(info.base_clock_rate_khz) +
               ",\"boost_clock_rate_khz\":" + std::to_string(info.boost_clock_rate_khz) +
               ",\"current_graphics_clock_mhz\":" + std::to_string(info.current_graphics_clock_mhz) +
               ",\"current_memory_clock_mhz\":" + std::to_string(info.current_memory_clock_mhz) +
               ",\"has_frequency_control\":" + (info.has_frequency_control ? "true" : "false") + "}";
    }

    std::string to_string(const GpuPowerInfo &info)
    {
        return "{\"current_power_watts\":" + std::to_string(info.current_power_watts) +
               ",\"power_limit_watts\":" + std::to_string(info.power_limit_watts) +
               ",\"min_power_watts\":" + std::to_string(info.min_power_watts) +
               ",\"max_power_watts\":" + std::to_string(info.max_power_watts) +
               ",\"default_power_watts\":" + std::to_string(info.default_power_watts) +
               ",\"current_limit_watts\":" + std::to_string(info.current_limit_watts) +
               ",\"has_power_monitoring\":" + (info.has_power_monitoring ? "true" : "false") +
               ",\"is_configurable\":" + (info.is_configurable ? "true" : "false") + "}";
    }

    std::string to_string(const GpuTemperatureInfo &info)
    {
        return "{\"current_temperature_celsius\":" + std::to_string(info.current_temperature_celsius) +
               ",\"max_temperature_celsius\":" + std::to_string(info.max_temperature_celsius) +
               ",\"has_temperature_monitoring\":" + (info.has_temperature_monitoring ? "true" : "false") + "}";
    }

    std::string to_string(const GpuPerformanceInfo &info)
    {
        return "{\"gpu_utilization_percent\":" + std::to_string(info.gpu_utilization_percent) +
               ",\"has_utilization_monitoring\":" + (info.has_utilization_monitoring ? "true" : "false") +
               ",\"performance_state\":" + std::to_string(info.performance_state) +
               ",\"power_state\":" + std::to_string(info.power_state) + "}";
    }

    std::string to_string(const GpuHardwareInfo &info)
    {
        return "{\"pci_bus_id\":\"" + info.pci_bus_id + "\"" +
               ",\"pci_device_id\":" + std::to_string(info.pci_device_id) +
               ",\"pci_subsystem_id\":" + std::to_string(info.pci_subsystem_id) +
               ",\"board_id\":" + std::to_string(info.board_id) +
               ",\"multi_gpu_board\":" + (info.multi_gpu_board ? "true" : "false") + "}";
    }

    std::string to_string(const GpuCapabilitiesInfo &info)
    {
        return "{\"ecc_enabled\":" + std::string(info.ecc_enabled ? "true" : "false") +
               ",\"supports_unified_memory\":" + (info.supports_unified_memory ? "true" : "false") +
               ",\"persistence_mode_enabled\":" + (info.persistence_mode_enabled ? "true" : "false") + "}";
    }

    std::string to_string(const GpuDeviceInfo &info)
    {
        return "{\"basic\":" + to_string(info.basic) +
               ",\"version\":" + to_string(info.version) +
               ",\"compute\":" + to_string(info.compute) +
               ",\"memory\":" + to_string(info.memory) +
               ",\"clocks\":" + to_string(info.clocks) +
               ",\"power\":" + to_string(info.power) +
               ",\"temperature\":" + to_string(info.temperature) +
               ",\"performance\":" + to_string(info.performance) +
               ",\"hardware\":" + to_string(info.hardware) +
               ",\"capabilities\":" + to_string(info.capabilities) + "}";
    }

    std::string to_string(const SystemGpuPowerInfo &info)
    {
        return "{\"total_gpu_power_budget_watts\":" + std::to_string(info.total_gpu_power_budget_watts) +
               ",\"current_gpu_power_usage_watts\":" + std::to_string(info.current_gpu_power_usage_watts) +
               ",\"available_gpu_power_headroom_watts\":" + std::to_string(info.available_gpu_power_headroom_watts) +
               ",\"num_power_monitored_gpus\":" + std::to_string(info.num_power_monitored_gpus) + "}";
    }

    // Stream operators for all structures
    std::ostream &operator<<(std::ostream &os, const GpuPowerMethod &method)
    {
        os << to_string(method);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuBasicInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuVersionInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuComputeInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuMemoryInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuClockInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuPowerInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuTemperatureInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuPerformanceInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuHardwareInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuCapabilitiesInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const GpuDeviceInfo &info)
    {
        os << to_string(info);
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const SystemGpuPowerInfo &info)
    {
        os << to_string(info);
        return os;
    }

} // namespace optkit::gpu
