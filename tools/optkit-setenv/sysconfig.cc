#include "sysconfig.hh"

SysConfig &SysConfig::add_module(std::unique_ptr<Module> mod)
{
    modules.push_back(std::move(mod));
    return *this;
}
std::string SysConfig::to_string() const
{
    std::ostringstream oss;
    oss << "SysConfig{\n"
        << "  " << cpu.to_string() << "\n"
        << "  " << memory.to_string() << "\n"
        << "  " << gpu.to_string() << "\n"
        << "  " << cgroup.to_string() << "\n"
        << "}";
    return oss.str();
}
bool SysConfig::is_valid() const
{
    for (auto &&module : this->modules)
        module->is_valid();
}
void SysConfig::apply()
{
    for (auto &&module : this->modules)
        module->apply();
}
void SysConfig::load_current_settings(pid_t pid)
{
    for (auto &&module : this->modules)
        module->load_current_settings(pid);
}