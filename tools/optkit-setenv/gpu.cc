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
        << ", reset_stats=" << (reset_stats ? "yes" : "no")
        << "}";
    return oss.str();
}
bool GPU::is_valid() const
{
    return true;
}

bool GPU::apply()
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
    this->reset_stats = false;
}

nlohmann::json GPU::to_json() const
{
    nlohmann::json j;
    j["persistence_mode"] = persistence_mode;
    j["fan_speed"] = fan_speed;
    j["core_freq_mhz"] = core_freq_mhz;
    j["mem_freq_mhz"] = mem_freq_mhz;
    j["power_limit_watts"] = power_limit_watts;
    j["reset_stats"] = reset_stats;
    return j;
}