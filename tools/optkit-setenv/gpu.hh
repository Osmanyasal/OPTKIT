#pragma once
#include "module.hh"

class GPU : public Module
{
public:
    static const std::string name;

public:
    GPU()
        : persistence_mode("off"),
          fan_speed("auto"),
          core_freq_mhz(0),
          mem_freq_mhz(0),
          power_limit_watts(0),
          reset_stats(false)
    {
    }
    virtual ~GPU() = default;

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
    nlohmann::json to_json() const override;

public:
    std::string persistence_mode; // on, off
    std::string fan_speed;        // percentage (e.g., "50%")
    int64_t core_freq_mhz;        // lock frequency
    int64_t mem_freq_mhz;         // lock memory frequency
    int64_t power_limit_watts;    // power limit in watts
    bool reset_stats;             // true or false
};

inline std::ostream &operator<<(std::ostream &os, const GPU &gpu)
{
    return os << gpu.to_string();
}