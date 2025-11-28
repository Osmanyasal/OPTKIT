#include "sysconfig.hh"

std::string SystemConfig::to_string() const
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
bool SystemConfig::is_valid() const
{
    return memory.is_valid() &&
           cpu.is_valid() &&
           disk_io.is_valid() &&
           kernel.is_valid() &&
           gpu.is_valid() &&
           cgroup.is_valid();
}
void SystemConfig::apply()
{
    cpu.apply();
    memory.apply();
    disk_io.apply();
    kernel.apply();
    gpu.apply();
    cgroup.apply();
}
void SystemConfig::load_current_settings()
{
    cpu.load_current_settings();
    memory.load_current_settings();
    disk_io.load_current_settings();
    kernel.load_current_settings();
    gpu.load_current_settings();
    cgroup.load_current_settings();
}