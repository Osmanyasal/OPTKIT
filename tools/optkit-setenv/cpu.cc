#include "cpu.hh"

bool CPU::set_turbo(Switch state)
{
    std::string turbo_path = "/sys/devices/system/cpu/intel_pstate/no_turbo";
    try
    {
        // no_turbo file is inverted: 0 = turbo enabled, 1 = turbo disabled
        optkit::utils::write_file(turbo_path, (state == Switch::ON) ? "0" : "1");
        this->turbo = (bool)state;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

bool CPU::set_governor(const std::string &gov)
{
    optkit::frequency::cpu::set_scaling_governor(gov);
}
bool CPU::set_affinity_cores(const std::vector<int> &cores)
{
}
bool CPU::set_offline_cores(const std::vector<int> &cores)
{
    for (auto core : cores)
        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "0");
}
bool CPU::set_online_cores(const std::vector<int> &cores)
{
    for (auto core : cores)
        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "1");
}
bool CPU::set_all_cores_online()
{
    for (auto core : OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "1");
}
bool CPU::set_all_cores_offline()
{
    for (auto core : OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
        optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "0");
}
bool CPU::set_core_freq(int64_t freq_khz)
{
    for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
        optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, socket);
}
bool CPU::set_uncore_freq(int64_t freq_khz)
{
    for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
        optkit::frequency::cpu::Frequency::set_uncore_frequency(freq_khz, socket);
}
bool CPU::set_smt_enabled(Switch state)
{
    optkit::utils::write_file("/sys/devices/system/cpu/smt/control", (bool)state ? "on" : "off");
}
std::string CPU::to_string() const
{
    std::ostringstream oss;
    oss << "CPU{governor=" << governor
        << ", core_freq=" << core_freq << "kHz"
        << ", uncore_freq=" << uncore_freq << "kHz"
        << ", smt=" << (smt_enabled ? "on" : "off")
        << ", turbo=" << (turbo ? "on" : "off")
        << ", affinity_cores=[";
    for (size_t i = 0; i < affinity_cores.size(); ++i)
    {
        if (i > 0)
            oss << ",";
        oss << affinity_cores[i];
    }
    oss << "], offline_cores=[";
    for (size_t i = 0; i < offline_cores.size(); ++i)
    {
        if (i > 0)
            oss << ",";
        oss << offline_cores[i];
    }
    oss << "]}";
    return oss.str();
}

bool CPU::is_valid()
{
    return true;
}