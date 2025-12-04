#include "sysconfig.hh"

std::string SysConfig::to_string() const
{
    std::ostringstream oss;
    oss << "SysConfig{\n"
        << "  " << get_module(CPU::name)->to_string() << "\n"
        << "  " << get_module(Memory::name)->to_string() << "\n"
        << "  " << get_module(GPU::name)->to_string() << "\n"
        << "  " << get_module(CGroup::name)->to_string() << "\n"
        << "}";
    return oss.str();
}
bool SysConfig::is_valid() const
{
    bool valid = true;
    for (const auto &pair : this->modules)
        valid = valid && pair.second->is_valid();
    return valid;
}
bool SysConfig::apply(pid_t pid)
{
    bool res = true;
    for (auto &pair : this->modules)
        res = res && pair.second->apply(pid);
    return res;
}
void SysConfig::load_current_settings(pid_t pid)
{
    for (auto &pair : this->modules)
        pair.second->load_current_settings(pid);
}
nlohmann::json SysConfig::to_json() const
{
    nlohmann::json j;
    for (const auto &pair : this->modules)
        j[pair.first] = pair.second->to_json();
    return j;
}
std::string SysConfig::possible_values() const
{
    std::ostringstream oss;
    for (const auto &pair : this->modules)
    {
        oss << pair.first << "\n"
            << pair.second->possible_values() << "\n";
    }
    return oss.str();
}