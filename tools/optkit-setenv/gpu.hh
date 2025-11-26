#pragma once
#include "helper.hh"

struct GPU
{
    std::string persistence_mode; // on, off
    std::string fan_speed;        // percentage (e.g., "50%")
    int64_t core_freq_mhz;        // lock frequency
    int64_t mem_freq_mhz;         // lock memory frequency
    int64_t power_limit_watts;    // power limit in watts
    bool reset_stats;             // true or false

    std::string to_string() const
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
    bool is_valid();
};

inline std::ostream &operator<<(std::ostream &os, const GPU &gpu)
{
    return os << gpu.to_string();
}