#pragma once
#include "module.hh"
#include "cpu.hh"
#include "diskio.hh"
#include "kernel.hh"
#include "gpu.hh"
#include "cgroup.hh"
#include "memory.hh"

struct SystemConfig
{
    CPU cpu;
    Memory memory;
    DiskIO disk_io;
    Kernel kernel;
    GPU gpu;
    CGroup cgroup;

    std::string to_string() const;
    bool is_valid() const;
    void apply();
};
inline std::ostream &operator<<(std::ostream &os, const SystemConfig &config)
{
    return os << config.to_string();
}
