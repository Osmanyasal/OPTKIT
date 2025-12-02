#pragma once
#include "module.hh"

enum class Switch
{
    ON = true,
    OFF = false
};

/**
 * @brief CPU configuration module, applies same settings to all possible CPU sockets
 *
 */
class CPU : public Module
{
public:
    static const std::string name;

public:
    CPU() : governor{"performance"}, affinity_cores{}, offline_cores{}, core_freq{0}, uncore_freq{0}, smt_enabled{false}, turbo{false} {}
    virtual ~CPU() = default;

    bool set_governor(const std::string &gov);
    bool set_affinity_cores(pid_t pid, const std::vector<int16_t> &cores);
    bool set_offline_cores(const std::vector<int16_t> &cores);
    bool set_online_cores(const std::vector<int16_t> &cores);
    bool set_all_cores_offline();
    bool set_all_cores_online();
    bool set_core_freq(int64_t freq_khz);
    bool set_uncore_freq(int64_t freq_khz);
    bool reset_core_freq(int64_t socket);
    bool reset_uncore_freq(int64_t socket);
    bool set_smt_enabled(Switch state);
    bool set_turbo(Switch state);
    std::vector<int16_t> get_offline_cores() const;
    std::string to_string() const override;

    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
    nlohmann::json to_json() const override;

public:
    std::string governor;                // performance, powersave, ondemand, userspace, schedutil
    std::vector<int16_t> affinity_cores; // list of core IDs to set affinity
    std::vector<int16_t> offline_cores;  // list of core IDs to offline
    int64_t core_freq;                   // in kHz
    int64_t uncore_freq;                 // in kHz
    bool smt_enabled;                    // true or false
    bool turbo;                          // true or false
};

inline std::ostream &operator<<(std::ostream &os, const CPU &cpu)
{
    return os << cpu.to_string();
}
