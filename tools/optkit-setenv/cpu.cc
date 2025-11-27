#include "cpu.hh"
#include <sched.h>

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
        return false;
    }
    return true;
}

bool CPU::set_governor(const std::string &gov)
{
    if (optkit::frequency::cpu::set_scaling_governor(gov))
    {
        this->governor = gov;
        return true;
    }
    return false;
}

bool CPU::set_affinity_cores(pid_t pid, const std::vector<int> &cores)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for (auto core : cores)
        CPU_SET(core, &mask);

    if (sched_setaffinity(pid, sizeof(mask), &mask) != 0)
    {
        return false;
    }

    this->affinity_cores = cores;
    return true;
}

bool CPU::set_offline_cores(const std::vector<int> &cores)
{
    std::vector<int> successfully_offlined;
    for (auto core : cores)
    {
        if (core == 0) // Cannot offline CPU0
            continue;

        try
        {
            optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "0");
            successfully_offlined.push_back(core);
        }
        catch (const std::exception &e)
        {
            // Continue trying other cores
            OPTKIT_WARN("Failed to offline core {}: {}", core, e.what());
        }
    }

    // Merge with existing offline cores
    for (auto core : successfully_offlined)
        if (std::find(this->offline_cores.begin(), this->offline_cores.end(), core) == this->offline_cores.end())
            this->offline_cores.push_back(core);

    return !successfully_offlined.empty();
}

bool CPU::set_online_cores(const std::vector<int> &cores)
{
    for (auto core : cores)
    {
        if (core == 0) // CPU0 is always online
            continue;

        try
        {
            optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "1");

            // Remove from offline_cores if present
            auto it = std::find(this->offline_cores.begin(), this->offline_cores.end(), core);
            if (it != this->offline_cores.end())
            {
                this->offline_cores.erase(it);
            }
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to online core {}: {}", core, e.what());
        }
    }

    return true;
}
bool CPU::set_all_cores_online()
{
    try
    {
        for (int core = 1; core < OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS; core++)
            optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "1");

        this->offline_cores.clear();
    }
    catch (const std::exception &e)
    {
        return false;
    }
    return true;
}
bool CPU::set_all_cores_offline()
{
    this->offline_cores.clear();

    // Cannot offline CPU0
    for (int core = 1; core < OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS; core++)
    {
        try
        {
            optkit::utils::write_file("/sys/devices/system/cpu/cpu" + std::to_string(core) + "/online", "0");
            this->offline_cores.push_back(core);
        }
        catch (const std::exception &e)
        {
            // Continue trying other cores
        }
    }

    return !this->offline_cores.empty();
}
bool CPU::set_core_freq(int64_t freq_khz)
{
    std::vector<int> changed_sockets;

    for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
    {
        if (optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, socket))
        {
            changed_sockets.push_back(socket);
        }
        else
        {
            // Rollback on failure
            for (auto s : changed_sockets)
            {
                reset_core_freq(s);
            }
            return false;
        }
    }

    this->core_freq = freq_khz;
    return true;
}
bool CPU::set_uncore_freq(int64_t freq_khz)
{
    std::vector<int> changed_sockets;

    for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
    {
        if (optkit::frequency::cpu::Frequency::set_uncore_frequency(freq_khz, socket))
        {
            changed_sockets.push_back(socket);
        }
        else
        {
            // Rollback on failure
            for (auto s : changed_sockets)
            {
                reset_uncore_freq(s);
            }
            return false;
        }
    }

    this->uncore_freq = freq_khz;
    return true;
}

bool CPU::reset_core_freq(int64_t socket)
{
    return optkit::frequency::cpu::Frequency::reset_core_frequency(socket);
}
bool CPU::reset_uncore_freq(int64_t socket)
{
    return optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket);
}
bool CPU::set_smt_enabled(Switch state)
{
    try
    {
        optkit::utils::write_file("/sys/devices/system/cpu/smt/control", (bool)state ? "on" : "off", true);
        this->smt_enabled = (bool)state;
    }
    catch (const std::exception &e)
    {
        return false;
    }
    return true;
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
    bool result = true;
    // Check governor is valid
    const std::vector<std::string> valid_governors = {"performance", "powersave", "ondemand", "conservative", "schedutil", "userspace"};
    if (!governor.empty() && std::find(valid_governors.begin(), valid_governors.end(), governor) == valid_governors.end())
    {
        OPTKIT_WARN("Invalid governor: {}, it should be either performance, powersave, ondemand, conservative, schedutil, or userspace", governor);
        result && = false;
    }

    // Check frequency ranges
    if (core_freq < 0 || uncore_freq < 0)
    {
        OPTKIT_WARN("Frequencies must be non-negative: core_freq={}, uncore_freq={}", core_freq, uncore_freq);
        result && = false;
    }
    if (core_freq > optkit::frequency::cpu::Query::get_cpuinfo_max_freq() || core_freq < optkit::frequency::cpu::Query::get_cpuinfo_min_freq())
    {
        OPTKIT_WARN("Core frequency {}kHz is out of bounds (min: {}kHz, max: {}kHz)", core_freq,
                    optkit::frequency::cpu::Query::get_cpuinfo_min_freq(),
                    optkit::frequency::cpu::Query::get_cpuinfo_max_freq());
        result && = false;
    }

    auto uncore_min_max = optkit::frequency::cpu::Frequency::get_uncore_min_max(0); // Just to check uncore support
    if (uncore_freq < uncore_min_max.first || uncore_freq > uncore_min_max.second)
    {
        OPTKIT_WARN("Uncore frequency {}kHz is out of bounds (min: {}kHz, max: {}kHz)", uncore_freq,
                    uncore_min_max.first,
                    uncore_min_max.second);
        result && = false;
    }

    // Check core indices are valid
    for (auto core : affinity_cores)
    {
        if (core < 0 || core >= OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
        {
            result && = false;
            OPTKIT_WARN("Invalid core index in affinity_cores: {}", core);
            break;
        }
    }

    for (auto core : offline_cores)
    {
        if (core < 0 || core >= OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS)
        {
            result && = false;
            OPTKIT_WARN("Invalid core index in offline_cores: {}", core);
            break;
        }
    }

    // Cannot offline CPU0
    if (std::find(offline_cores.begin(), offline_cores.end(), 0) != offline_cores.end())
    {
        OPTKIT_WARN("Cannot offline CPU0");
        result && = false;
    }

    return result;
}