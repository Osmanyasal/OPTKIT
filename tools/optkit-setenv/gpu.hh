#pragma once
#include "module.hh"

class GPU : public Module
{
public:
    static const std::string name;

    // Singleton access
    static GPU &instance()
    {
        static GPU instance;
        return instance;
    }

    // Delete copy and move
    GPU(const GPU &) = delete;
    GPU &operator=(const GPU &) = delete;
    GPU(GPU &&) = delete;
    GPU &operator=(GPU &&) = delete;

private:
    GPU()
        : persistence_mode("off"),
          fan_speed("auto"),
          core_freq_mhz(0),
          mem_freq_mhz(0),
          power_limit_watts(0),
          reset_device(true)
    {
    }
    virtual ~GPU() = default;

public:
    std::string to_string() const override;
    bool is_valid() const override;
    bool apply(pid_t pid) override;
    void load_current_settings(pid_t pid) override;
    nlohmann::json to_json() const override;
    std::string possible_values() const override;

public:
    std::string persistence_mode; // on, off
    std::string fan_speed;        // percentage (e.g., "50%")
    int64_t core_freq_mhz;        // lock frequency
    int64_t mem_freq_mhz;         // lock memory frequency
    int64_t power_limit_watts;    // power limit in watts
    bool reset_device;            // true or false
};

inline std::ostream &operator<<(std::ostream &os, const GPU &gpu)
{
    return os << gpu.to_string();
}