#pragma once
#include "helper.hh"
struct CGroup
{
    std::string cpuset;         // CPU list or range
    std::string mem_limit;      // bytes or human readable (e.g., "4G")
    std::string io_limit_read;  // major:minor:bytes (e.g., "8:0:10MB")
    std::string io_limit_write; // major:minor:bytes
    std::string freeze_state;   // freeze, thaw
    int64_t cpu_quota_us;       // microseconds
    int64_t cpu_period_us;      // microseconds
    int64_t mem_swappiness;     // 0-100

    bool is_valid();
    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "CGroup{cpuset=" << cpuset
            << ", mem_limit=" << mem_limit
            << ", io_read=" << io_limit_read
            << ", io_write=" << io_limit_write
            << ", freeze=" << freeze_state
            << ", cpu_quota=" << cpu_quota_us << "us"
            << ", cpu_period=" << cpu_period_us << "us"
            << ", mem_swappiness=" << mem_swappiness
            << "}";
        return oss.str();
    }
};

inline std::ostream &operator<<(std::ostream &os, const CGroup &cg)
{
    return os << cg.to_string();
}
