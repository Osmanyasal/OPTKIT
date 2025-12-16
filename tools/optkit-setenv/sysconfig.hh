#pragma once
#include "module.hh"
#include "cpu.hh"
#include "gpu.hh"
#include "cgroup.hh"
#include "memory.hh"

class SysConfig : public Module
{
public:
    // Singleton access
    static SysConfig &instance()
    {
        static SysConfig instance;
        return instance;
    }

    // Delete copy and move
    SysConfig(const SysConfig &) = delete;
    SysConfig &operator=(const SysConfig &) = delete;
    SysConfig(SysConfig &&) = delete;
    SysConfig &operator=(SysConfig &&) = delete;

private:
    SysConfig() { initialize_modules(); }
    virtual ~SysConfig() = default;

    void initialize_modules()
    {
        modules[CPU::name] = &CPU::instance();
        modules[Memory::name] = &Memory::instance();
        modules[GPU::name] = &GPU::instance();
        modules[CGroup::name] = &CGroup::instance();
    }

public:
    std::string to_string() const override;
    bool is_valid() const override;
    bool apply(pid_t pid) override;
    void load_current_settings(pid_t pid) override;
    std::string possible_values() const override;
    nlohmann::json to_json() const override;

    Module *get_module(const std::string &module_name) const
    {
        auto it = modules.find(module_name);
        if (it != modules.end())
        {
            return it->second;
        }
        OPTKIT_WARN("Module '{}' not found in SysConfig", module_name);
        return nullptr;
    }

private:
    std::unordered_map<std::string, Module *> modules;
};
inline std::ostream &operator<<(std::ostream &os, const SysConfig &config)
{
    return os << config.to_string();
}
