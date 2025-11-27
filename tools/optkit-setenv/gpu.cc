#include "gpu.hh"

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