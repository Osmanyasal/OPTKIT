#include "sysconfig.hh"

std::string SysConfig::to_string() const
{
    std::ostringstream oss;
    oss << "SysConfig{\n";
    for (const auto &pair : this->modules)
        oss << pair.second->to_string() << "\n";
    oss << "}";
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
    {
        if (!pair.second->apply(pid))
            res = false;
    }
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