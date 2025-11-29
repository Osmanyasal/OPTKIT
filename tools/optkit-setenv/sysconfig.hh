#pragma once
#include "module.hh"
#include "cpu.hh"
#include "diskio.hh"
#include "kernel.hh"
#include "gpu.hh"
#include "cgroup.hh"
#include "memory.hh"

class SysConfig
{
public:
    SysConfig &add_module(std::unique_ptr<Module> mod);
    std::string to_string() const;
    bool is_valid() const;
    void apply();
    void load_current_settings(pid_t pid);

public:
    std::vector<std::unique_ptr<Module>> modules;
};
inline std::ostream &operator<<(std::ostream &os, const SysConfig &config)
{
    return os << config.to_string();
}
