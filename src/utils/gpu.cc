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
        std::string result;
        if (method == GpuPowerMethod::NONE)
            return "NONE";

        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::NVIDIA_ML))
            result += "NVIDIA_ML|";
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::AMD_ROCM))
            result += "AMD_ROCM|";
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::INTEL_I915))
            result += "INTEL_I915|";
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::SYSFS_HWMON))
            result += "SYSFS_HWMON|";
        if (static_cast<uint8_t>(method) & static_cast<uint8_t>(GpuPowerMethod::PERF_UNCORE))
            result += "PERF_UNCORE|";

        if (!result.empty() && result.back() == '|')
            result.pop_back();

        return result.empty() ? "NONE" : result;
    }

    std::string to_string(const GpuBasicInfo &info)
    {
        return "GpuBasicInfo{id=" + std::to_string(info.id) +
               ", name='" + info.name +
               "', device_name='" + info.device_name +
               "', vendor=" + to_string(info.vendor) +
               ", vendor_string='" + info.vendor_string +
               "', is_integrated=" + (info.is_integrated ? "true" : "false") + "}";
    }

    std::string to_string(const GpuVersionInfo &info)
    {
        return "GpuVersionInfo{driver_major_minor=" + std::to_string(info.driver_major_minor) +
               ", driver_version='" + info.driver_version_string +
               "', library_version='" + info.library_version_string + "'}";
    }

    std::string to_string(const GpuComputeInfo &info)
    {
        return "GpuComputeInfo{compute_capability=" + std::to_string(info.compute_capability_major) +
               "." + std::to_string(info.compute_capability_minor) +
               ", multiprocessors=" + std::to_string(info.multiprocessor_count) +
               ", cuda_cores_per_mp=" + std::to_string(info.cuda_cores_per_mp) +
               ", total_cuda_cores=" + std::to_string(info.total_cuda_cores) +
               ", warp_size=" + std::to_string(info.warp_size) + "}";
    }

    std::string to_string(const GpuMemoryInfo &info)
    {
        return "GpuMemoryInfo{total=" + std::to_string(info.total_global_memory_bytes / (1024 * 1024)) + "MB" +
               ", free=" + std::to_string(info.free_memory_bytes / (1024 * 1024)) + "MB" +
               ", used=" + std::to_string(info.used_memory_bytes / (1024 * 1024)) + "MB" +
               ", bus_width=" + std::to_string(info.memory_bus_width_bits) + "bits" +
               ", clock=" + std::to_string(info.memory_clock_rate_khz / 1000) + "MHz" +
               ", max_clock=" + std::to_string(info.memory_clock_rate_max_khz / 1000) + "MHz" +
               ", utilization=" + std::to_string(info.memory_utilization_percent) + "%}";
    }

    std::string to_string(const GpuClockInfo &info)
    {
        return "GpuClockInfo{base_clock=" + std::to_string(info.base_clock_rate_khz / 1000) + "MHz" +
               ", boost_clock=" + std::to_string(info.boost_clock_rate_khz / 1000) + "MHz" +
               ", current_graphics=" + std::to_string(info.current_graphics_clock_mhz) + "MHz" +
               ", current_memory=" + std::to_string(info.current_memory_clock_mhz) + "MHz" +
               ", has_freq_control=" + (info.has_frequency_control ? "true" : "false") + "}";
    }

    std::string to_string(const GpuPowerInfo &info)
    {
        return "GpuPowerInfo{current=" + std::to_string(info.current_power_watts) + "W" +
               ", limit=" + std::to_string(info.power_limit_watts) + "W" +
               ", min=" + std::to_string(info.min_power_watts) + "W" +
               ", max=" + std::to_string(info.max_power_watts) + "W" +
               ", default=" + std::to_string(info.default_power_watts) + "W" +
               ", current_limit=" + std::to_string(info.current_limit_watts) + "W" +
               ", has_monitoring=" + (info.has_power_monitoring ? "true" : "false") +
               ", configurable=" + (info.is_configurable ? "true" : "false") + "}";
    }

    std::string to_string(const GpuTemperatureInfo &info)
    {
        return "GpuTemperatureInfo{current=" + std::to_string(info.current_temperature_celsius) + "°C" +
               ", max=" + std::to_string(info.max_temperature_celsius) + "°C" +
               ", has_monitoring=" + (info.has_temperature_monitoring ? "true" : "false") + "}";
    }

    std::string to_string(const GpuPerformanceInfo &info)
    {
        return "GpuPerformanceInfo{gpu_util=" + std::to_string(info.gpu_utilization_percent) + "%" +
               ", has_util_monitoring=" + (info.has_utilization_monitoring ? "true" : "false") +
               ", perf_state=" + std::to_string(info.performance_state) +
               ", power_state=" + std::to_string(info.power_state) + "}";
    }

    std::string to_string(const GpuHardwareInfo &info)
    {
        return "GpuHardwareInfo{pci_bus='" + info.pci_bus_id +
               "', device_id=0x" + std::to_string(info.pci_device_id) +
               ", subsystem_id=0x" + std::to_string(info.pci_subsystem_id) +
               ", board_id=" + std::to_string(info.board_id) +
               ", multi_gpu=" + (info.multi_gpu_board ? "true" : "false") + "}";
    }

    std::string to_string(const GpuCapabilitiesInfo &info)
    {
        return std::string("GpuCapabilitiesInfo{ecc=") + (info.ecc_enabled ? "true" : "false") +
               ", unified_memory=" + (info.supports_unified_memory ? "true" : "false") +
               ", persistence=" + (info.persistence_mode_enabled ? "true" : "false") + "}";
    }

    std::string to_string(const GpuDeviceInfo &info)
    {
        return std::string("GpuDeviceInfo{\n") +
               "  basic=" + to_string(info.basic) + "\n" +
               "  version=" + to_string(info.version) + "\n" +
               "  compute=" + to_string(info.compute) + "\n" +
               "  memory=" + to_string(info.memory) + "\n" +
               "  clocks=" + to_string(info.clocks) + "\n" +
               "  power=" + to_string(info.power) + "\n" +
               "  temperature=" + to_string(info.temperature) + "\n" +
               "  performance=" + to_string(info.performance) + "\n" +
               "  hardware=" + to_string(info.hardware) + "\n" +
               "  capabilities=" + to_string(info.capabilities) + "\n}";
    }

    std::string to_string(const SystemGpuPowerInfo &info)
    {
        return "SystemGpuPowerInfo{total_budget=" + std::to_string(info.total_gpu_power_budget_watts) + "W" +
               ", current_usage=" + std::to_string(info.current_gpu_power_usage_watts) + "W" +
               ", headroom=" + std::to_string(info.available_gpu_power_headroom_watts) + "W" +
               ", monitored_gpus=" + std::to_string(info.num_power_monitored_gpus) + "}";
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
