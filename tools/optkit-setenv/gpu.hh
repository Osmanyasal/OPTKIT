#pragma once
#include "module.hh"

struct GPU : public Module
{
    std::string persistence_mode; // on, off
    std::string fan_speed;        // percentage (e.g., "50%")
    int64_t core_freq_mhz;        // lock frequency
    int64_t mem_freq_mhz;         // lock memory frequency
    int64_t power_limit_watts;    // power limit in watts
    bool reset_stats;             // true or false

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
};

inline std::ostream &operator<<(std::ostream &os, const GPU &gpu)
{
    return os << gpu.to_string();
}