#pragma once
#include "module.hh"
#include "cpu.hh"
#include "gpu.hh"
#include "cgroup.hh"
#include "memory.hh"

class SysConfig : public Module
{
public:
    SysConfig() = default;
    virtual ~SysConfig() = default;

    // Delete copy operations (unique_ptr is not copyable)
    SysConfig(const SysConfig &) = delete;
    SysConfig &operator=(const SysConfig &) = delete;

    // Default move operations
    SysConfig(SysConfig &&) = default;
    SysConfig &operator=(SysConfig &&) = default;

    std::string to_string() const override;
    bool is_valid() const override;
    bool apply() override;
    void load_current_settings(pid_t pid) override;
    nlohmann::json to_json() const override;

    SysConfig &add_module(const std::string &name, std::unique_ptr<Module> mod);
    Module *get_module(const std::string &module_name) const
    {
        if (modules.find(module_name) != modules.end())
        {
            return modules.at(module_name).get();
        }
        throw std::runtime_error("Module not found: " + module_name);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Module>> modules;
};
inline std::ostream &operator<<(std::ostream &os, const SysConfig &config)
{
    return os << config.to_string();
}
