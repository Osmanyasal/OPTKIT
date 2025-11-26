#pragma once
#include "cpu.hh"
#include "diskio.hh"
#include "kernel.hh"
#include "gpu.hh"
#include "cgroup.hh"

struct SystemConfig
{
    CPU cpu;
    Memory memory;
    DiskIO disk_io;
    Kernel kernel;
    GPU gpu;
    CGroup cgroup;

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "SystemConfig{\n"
            << "  " << cpu.to_string() << "\n"
            << "  " << memory.to_string() << "\n"
            << "  " << disk_io.to_string() << "\n"
            << "  " << kernel.to_string() << "\n"
            << "  " << gpu.to_string() << "\n"
            << "  " << cgroup.to_string() << "\n"
            << "}";
        return oss.str();
    }
};
inline std::ostream &operator<<(std::ostream &os, const SystemConfig &config)
{
    return os << config.to_string();
}

bool is_requested_config_valid(const SystemConfig &config);
void apply_requested_config(const SystemConfig &config);
