#pragma once
#include "helper.hh"
enum class Switch
{
    ON = true,
    OFF = false
};

struct CPU
{
    std::string governor;            // performance, powersave, ondemand, userspace, schedutil
    std::vector<int> affinity_cores; // list of core IDs to set affinity
    std::vector<int> offline_cores;  // list of core IDs to offline
    int64_t core_freq;               // in kHz
    int64_t uncore_freq;             // in kHz
    bool smt_enabled;                // true or false
    bool turbo;                      // true or false

    bool set_governor(const std::string &gov);
    bool set_affinity_cores(const std::vector<int> &cores);
    bool set_offline_cores(const std::vector<int> &cores);
    bool set_online_cores(const std::vector<int> &cores);
    bool set_all_cores_offline();
    bool set_all_cores_online();
    bool set_core_freq(int64_t freq_khz);
    bool set_uncore_freq(int64_t freq_khz);
    bool set_smt_enabled(Switch state);
    bool set_turbo(Switch state);
    std::string to_string() const;

    bool is_valid();
};

inline std::ostream &operator<<(std::ostream &os, const CPU &cpu)
{
    return os << cpu.to_string();
}
