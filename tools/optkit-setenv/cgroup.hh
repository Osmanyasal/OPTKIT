#pragma once
#include "module.hh"
#include <cstdint>
#include <string>

class CGroup : public Module
{
public:
    static const std::string name;

    // Singleton access
    static CGroup &instance()
    {
        static CGroup instance;
        return instance;
    }

    // Delete copy and move
    CGroup(const CGroup &) = delete;
    CGroup &operator=(const CGroup &) = delete;
    CGroup(CGroup &&) = delete;
    CGroup &operator=(CGroup &&) = delete;

    // CPU controller settings
    struct CPU
    {
        int64_t quota_us;     // cpu.max: max CPU time per period (-1 = unlimited)
        int64_t period_us;    // cpu.max: period (default 100000us = 100ms)
        int64_t max_burst_us; // cpu.max.burst: accumulated unused quota
        // int64_t weight;       // cpu.weight: relative CPU time (1-10000, default 100)
        int64_t weight_nice; // cpu.weight.nice: nice value (-20 to 19)
        int64_t uclamp_min;  // cpu.uclamp.min: minimum utilization clamp (0-100)
        int64_t uclamp_max;  // cpu.uclamp.max: maximum utilization clamp (0-100)
        bool idle;           // cpu.idle: mark as idle workload (0 or 1)

        CPU() : quota_us(-1), period_us(100000), max_burst_us(0),
                // weight(100),
                weight_nice(0), uclamp_min(0), uclamp_max(100),
                idle(false)
        {
        }

        std::string to_string() const;
    };

    // Memory controller settings
    struct Memory
    {
        std::string max;       // memory.max: hard limit (e.g., "4G", "max")
        std::string high;      // memory.high: soft limit, throttle if exceeded
        std::string low;       // memory.low: best-effort protection
        std::string min;       // memory.min: hard protection (won't reclaim below)
        std::string swap_max;  // memory.swap.max: swap limit
        std::string swap_high; // memory.swap.high: throttle swap usage
        std::string zswap_max; // memory.zswap.max: compressed swap limit
        bool oom_group;        // memory.oom.group: kill all on OOM (0 or 1)
        bool zswap_writeback;  // memory.zswap.writeback: allow writeback (0 or 1)

        Memory() : max("max"), high("max"), low("0"), min("0"),
                   swap_max("max"), swap_high("max"), zswap_max("max"),
                   oom_group(false), zswap_writeback(true) {}

        std::string to_string() const;
    };

    // IO controller settings
    struct IO
    {
        std::string max;    // io.max: bandwidth/iops limits (format: "major:minor rbps=X wbps=Y")
        std::string weight; // io.weight: relative IO time (1-10000, default 100)

        IO() : max(""), weight("100") {}

        std::string to_string() const;
    };

    // PID controller settings
    struct PID
    {
        int64_t max; // pids.max: maximum number of processes/threads (-1 = unlimited)

        PID() : max(-1) {}

        std::string to_string() const;
    };

    // Core cgroup settings
    struct Core
    {
        bool freeze;             // cgroup.freeze: freeze all processes (0 or 1)
        int64_t max_depth;       // cgroup.max.depth: max nesting depth (-1 = unlimited)
        int64_t max_descendants; // cgroup.max.descendants: max child cgroups (-1 = unlimited)
        bool pressure;           // cgroup.pressure: enable PSI (0 or 1)

        Core() : freeze(false), max_depth(-1), max_descendants(-1), pressure(true) {}

        std::string to_string() const;
    };

private:
    CGroup() = default;
    virtual ~CGroup()
    {
        destroy_cgroup();
    };

public:
    bool is_valid() const override;
    bool apply(pid_t pid) override;
    std::string to_string() const override;
    void load_current_settings(pid_t pid) override;
    std::string possible_values() const override;
    nlohmann::json to_json() const override;

    // Resource controller configurations
    CGroup::CPU cpu;
    CGroup::Memory memory;
    CGroup::IO io;
    CGroup::PID pid;
    CGroup::Core core;

    // Cgroup path and name
    std::string cgroup_path; // e.g., "/sys/fs/cgroup/optkit_experiment"
    std::string cgroup_name; // e.g., "optkit_experiment"

private:
    bool is_cgroup_v2() const;
    std::string get_cgroup_root() const;
    bool create_cgroup();
    bool destroy_cgroup();
    bool enable_controllers();
    bool add_process(pid_t pid);
};

inline std::ostream &operator<<(std::ostream &os, const CGroup &cg)
{
    return os << cg.to_string();
}
