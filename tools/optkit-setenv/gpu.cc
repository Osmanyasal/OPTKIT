#include "gpu.hh"

const std::string GPU::name = "gpu";

std::string GPU::to_string() const
{
    std::ostringstream oss;
    oss << "GPU{persistence=" << persistence_mode
        << ", fan=" << fan_speed
        << ", core_freq=" << core_freq_mhz << "MHz"
        << ", mem_freq=" << mem_freq_mhz << "MHz"
        << ", power_limit=" << power_limit_watts << "W"
        << ", reset_device=" << (reset_device ? "yes" : "no")
        << "}";
    return oss.str();
}
bool GPU::is_valid() const
{
    bool result = true;

    return result;
}

bool GPU::apply(pid_t pid)
{
    return true;
}

void GPU::load_current_settings(pid_t pid)
{
    (void)pid; // Unused for GPU module
    // GPU settings are typically per-device, not per-process
    // Would need device_index parameter to query specific GPU
    this->persistence_mode = "";
    this->fan_speed = "";
    this->core_freq_mhz = 0;
    this->mem_freq_mhz = 0;
    this->power_limit_watts = 0;
    this->reset_device = false;
}

nlohmann::json GPU::to_json() const
{
    nlohmann::json j;
    j["persistence_mode"] = persistence_mode;
    j["fan_speed"] = fan_speed;
    j["core_freq_mhz"] = core_freq_mhz;
    j["mem_freq_mhz"] = mem_freq_mhz;
    j["power_limit_watts"] = power_limit_watts;
    j["reset_device"] = reset_device;
    return j;
}
std::string GPU::possible_values() const
{
    std::ostringstream oss;
    OPTKIT_GPU_VENDOR_TRAVERSE(vendor)
    {
        if (optkit::gpu::Query::is_device_exists(vendor))
        {
            optkit::gpu::GpuDeviceInfo device_info{};
            optkit::gpu::Query::device_query(vendor, 0, device_info);
            oss << "persistence_mode: on, off\n";
            oss << "fan_speed: auto, 0-100%\n";
            oss << "\tmem_freq_mhz: ";
            for (auto &&i : device_info.clocks.memory_supported_clock_rates_MHz)
                oss << i << ",";

            if (!device_info.clocks.memory_supported_clock_rates_MHz.empty())
                oss.seekp(-1, oss.cur);
            oss << "\n";

            oss << "\tcore_freq_mhz: ";
            for (auto &&i : device_info.clocks.graphics_supported_clock_rates_MHz)
            {
                oss << "[" << i.first << "]:";
                for (auto &&j : i.second)
                    oss << j << ",";
                oss << "\n";
            }

            if (!device_info.clocks.graphics_supported_clock_rates_MHz.empty())
                oss.seekp(-1, oss.cur);
            oss << "\n";
            oss << "\tpower_limit_watts: " << device_info.power.min_power_watts << "-" << device_info.power.max_power_watts << " limit:" << device_info.power.power_limit_watts << "\n";
            oss << "\treset_device: true, false\n";
        }
    }
    return oss.str();
}