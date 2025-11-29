#pragma once
#include "module.hh"

class CGroup : public Module
{
public:
    CGroup()
        : cpuset(""),
          mem_limit(""),
          io_limit_read(""),
          io_limit_write(""),
          freeze_state("thaw"),
          cpu_quota_us(-1),
          cpu_period_us(100000),
          mem_swappiness(-1)
    {
    }
    virtual ~CGroup() = default;
    bool is_valid() const override;
    bool apply() override;
    std::string to_string() const override;
    void load_current_settings(pid_t pid) override;

public:
    std::string cpuset;         // CPU list or range
    std::string mem_limit;      // bytes or human readable (e.g., "4G")
    std::string io_limit_read;  // major:minor:bytes (e.g., "8:0:10MB")
    std::string io_limit_write; // major:minor:bytes
    std::string freeze_state;   // freeze, thaw
    int64_t cpu_quota_us;       // microseconds
    int64_t cpu_period_us;      // microseconds
    int64_t mem_swappiness;     // 0-100
};

inline std::ostream &operator<<(std::ostream &os, const CGroup &cg)
{
    return os << cg.to_string();
}
