#include "cgroup.hh"
#include "utils/utils.hh"
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>

const std::string CGroup::name = "cgroup";

// CPU sub-struct implementation
std::string CGroup::CPU::to_string() const
{
    std::ostringstream oss;
    oss << "CPU{quota=" << quota_us << "us"
        << ", period=" << period_us << "us"
        << ", burst=" << max_burst_us << "us"
        << ", weight=" << weight
        << ", weight_nice=" << weight_nice
        << ", uclamp=[" << uclamp_min << "," << uclamp_max << "]"
        << ", idle=" << (idle ? "yes" : "no")
        << "}";
    return oss.str();
}

// Memory sub-struct implementation
std::string CGroup::Memory::to_string() const
{
    std::ostringstream oss;
    oss << "Memory{max=" << max
        << ", high=" << high
        << ", low=" << low
        << ", min=" << min
        << ", swap_max=" << swap_max
        << ", swap_high=" << swap_high
        << ", zswap_max=" << zswap_max
        << ", oom_group=" << (oom_group ? "yes" : "no")
        << ", zswap_writeback=" << (zswap_writeback ? "yes" : "no")
        << "}";
    return oss.str();
}

// IO sub-struct implementation
std::string CGroup::IO::to_string() const
{
    std::ostringstream oss;
    oss << "IO{max=" << (max.empty() ? "none" : max)
        << ", weight=" << weight
        << "}";
    return oss.str();
}

// PID sub-struct implementation
std::string CGroup::PID::to_string() const
{
    std::ostringstream oss;
    oss << "PID{max=" << (max == -1 ? "unlimited" : std::to_string(max)) << "}";
    return oss.str();
}

// Core sub-struct implementation
std::string CGroup::Core::to_string() const
{
    std::ostringstream oss;
    oss << "Core{freeze=" << (freeze ? "yes" : "no")
        << ", max_depth=" << max_depth
        << ", max_descendants=" << max_descendants
        << ", pressure=" << (pressure ? "yes" : "no")
        << "}";
    return oss.str();
}

// Main CGroup implementation
std::string CGroup::to_string() const
{
    std::ostringstream oss;
    oss << "CGroup{name=" << cgroup_name
        << ", path=" << cgroup_path
        << ", " << cpu.to_string()
        << ", " << memory.to_string()
        << ", " << io.to_string()
        << ", " << pid.to_string()
        << ", " << core.to_string()
        << "}";
    return oss.str();
}

bool CGroup::is_cgroup_v2() const
{
    struct statfs buf;
    if (statfs("/sys/fs/cgroup", &buf) == 0)
    {
        return buf.f_type == 0x63677270; // CGROUP2_SUPER_MAGIC
    }
    return false;
}

std::string CGroup::get_cgroup_root() const
{
    return "/sys/fs/cgroup";
}

bool CGroup::create_cgroup()
{
    if (cgroup_name.empty())
    {
        cgroup_name = "optkit_" + optkit::utils::generateGUID().substr(0, CONF_LOG_PRINT_GUID_LENGTH);
    }

    cgroup_path = get_cgroup_root() + "/" + cgroup_name;

    // Create directory
    if (mkdir(cgroup_path.c_str(), 0755) != 0 && errno != EEXIST)
    {
        std::cerr << "Failed to create cgroup: " << cgroup_path << " (" << strerror(errno) << ")\n";
        return false;
    }

    return true;
}

bool CGroup::destroy_cgroup()
{
    if (cgroup_path.empty())
        return true;

    // Move all processes out first
    try
    {
        std::string procs = optkit::utils::read_file(cgroup_path + "/cgroup.procs");
        std::string parent_procs = get_cgroup_root() + "/cgroup.procs";

        std::istringstream iss(procs);
        std::string pid_str;
        while (std::getline(iss, pid_str))
        {
            if (!pid_str.empty() && pid_str != "\n")
            {
                try
                {
                    optkit::utils::write_file(parent_procs, pid_str);
                }
                catch (...)
                {
                    // Continue
                }
            }
        }
    }
    catch (...)
    {
        // Continue
    }

    // Remove directory
    if (rmdir(cgroup_path.c_str()) != 0)
    {
        std::cerr << "Failed to remove cgroup: " << cgroup_path << " (" << strerror(errno) << ")\n";
        return false;
    }

    return true;
}

bool CGroup::enable_controllers()
{
    // For cgroup v2, need to enable controllers in subtree_control
    std::string parent_path = get_cgroup_root();
    std::string subtree_control = parent_path + "/cgroup.subtree_control";

    try
    {
        // Enable all controllers
        optkit::utils::write_file(subtree_control, "+cpu +memory +io +pids", true);
    }
    catch (const std::exception &e)
    {
        // May fail if already enabled or insufficient permissions
        // Not critical, continue
    }

    return true;
}

bool CGroup::add_process(pid_t process_pid)
{
    std::string procs_path = cgroup_path + "/cgroup.procs";
    try
    {
        optkit::utils::write_file(procs_path, std::to_string(process_pid), true);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to add process to cgroup: " << e.what() << "\n";
        return false;
    }
}

bool CGroup::apply(pid_t pid)
{
    if (!is_cgroup_v2())
    {
        std::cerr << "Only cgroup v2 is supported\n";
        return false;
    }

    // Create cgroup
    if (!create_cgroup())
        return false;

    // Enable controllers
    enable_controllers();

    try
    {
        // Apply CPU settings
        if (cpu.quota_us != -1 || cpu.period_us != 100000)
        {
            std::string cpu_max = (cpu.quota_us == -1) ? "max" : std::to_string(cpu.quota_us);
            cpu_max += " " + std::to_string(cpu.period_us);
            optkit::utils::write_file(cgroup_path + "/cpu.max", cpu_max, true);
        }

        if (cpu.max_burst_us > 0)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.max.burst", std::to_string(cpu.max_burst_us), true);
        }

        if (cpu.weight != 100)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.weight", std::to_string(cpu.weight), true);
        }

        if (cpu.weight_nice != 0)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.weight.nice", std::to_string(cpu.weight_nice), true);
        }

        if (cpu.uclamp_min > 0)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.uclamp.min", std::to_string(cpu.uclamp_min) + ".00", true);
        }

        if (cpu.uclamp_max < 100)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.uclamp.max", std::to_string(cpu.uclamp_max) + ".00", true);
        }

        if (cpu.idle)
        {
            optkit::utils::write_file(cgroup_path + "/cpu.idle", "1", true);
        }

        // Apply Memory settings
        if (!memory.max.empty() && memory.max != "max")
        {
            optkit::utils::write_file(cgroup_path + "/memory.max", memory.max, true);
        }

        if (!memory.high.empty() && memory.high != "max")
        {
            optkit::utils::write_file(cgroup_path + "/memory.high", memory.high, true);
        }

        if (!memory.low.empty() && memory.low != "0")
        {
            optkit::utils::write_file(cgroup_path + "/memory.low", memory.low, true);
        }

        if (!memory.min.empty() && memory.min != "0")
        {
            optkit::utils::write_file(cgroup_path + "/memory.min", memory.min, true);
        }

        if (!memory.swap_max.empty() && memory.swap_max != "max")
        {
            optkit::utils::write_file(cgroup_path + "/memory.swap.max", memory.swap_max, true);
        }

        if (!memory.swap_high.empty() && memory.swap_high != "max")
        {
            optkit::utils::write_file(cgroup_path + "/memory.swap.high", memory.swap_high, true);
        }

        if (!memory.zswap_max.empty() && memory.zswap_max != "max")
        {
            optkit::utils::write_file(cgroup_path + "/memory.zswap.max", memory.zswap_max, true);
        }

        if (memory.oom_group)
        {
            optkit::utils::write_file(cgroup_path + "/memory.oom.group", "1", true);
        }

        if (!memory.zswap_writeback)
        {
            optkit::utils::write_file(cgroup_path + "/memory.zswap.writeback", "0", true);
        }

        // Apply IO settings
        if (!io.max.empty())
        {
            optkit::utils::write_file(cgroup_path + "/io.max", io.max, true);
        }

        if (!io.weight.empty() && io.weight != "100")
        {
            optkit::utils::write_file(cgroup_path + "/io.weight", io.weight, true);
        }

        // Apply PID settings
        if (this->pid.max != -1)
        {
            optkit::utils::write_file(cgroup_path + "/pids.max", std::to_string(this->pid.max), true);
        }

        // Apply Core settings
        if (core.freeze)
        {
            optkit::utils::write_file(cgroup_path + "/cgroup.freeze", "1", true);
        }

        if (core.max_depth != -1)
        {
            optkit::utils::write_file(cgroup_path + "/cgroup.max.depth", std::to_string(core.max_depth), true);
        }

        if (core.max_descendants != -1)
        {
            optkit::utils::write_file(cgroup_path + "/cgroup.max.descendants", std::to_string(core.max_descendants), true);
        }

        if (!core.pressure)
        {
            optkit::utils::write_file(cgroup_path + "/cgroup.pressure", "0", true);
        }

        // Add current process to cgroup
        add_process(getpid());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to apply cgroup settings: " << e.what() << "\n";
        return false;
    }

    return true;
}

void CGroup::load_current_settings(pid_t process_pid)
{
    // Read current cgroup path for the process
    std::string cgroup_file = "/proc/" + std::to_string(process_pid) + "/cgroup";
    try
    {
        std::string content = optkit::utils::read_file(cgroup_file);

        // Parse cgroup v2 format: "0::/path/to/cgroup"
        size_t pos = content.find("::");
        if (pos != std::string::npos)
        {
            std::string rel_path = content.substr(pos + 2);
            // Trim whitespace
            rel_path.erase(rel_path.find_last_not_of(" \n\r\t") + 1);
            cgroup_path = get_cgroup_root() + rel_path;

            // Extract cgroup name
            size_t last_slash = rel_path.find_last_of('/');
            cgroup_name = (last_slash != std::string::npos) ? rel_path.substr(last_slash + 1) : rel_path;
        }

        if (cgroup_path.empty() || cgroup_path == get_cgroup_root())
        {
            // Process is in root cgroup, can't read settings
            return;
        }

        // Load CPU settings
        std::string cpu_max_str = optkit::utils::read_file(cgroup_path + "/cpu.max");
        auto tokens = optkit::utils::str_split(cpu_max_str, " ");
        if (tokens.size() >= 2)
        {
            cpu.quota_us = (tokens[0] == "max") ? -1 : std::stoll(tokens[0]);
            cpu.period_us = std::stoll(tokens[1]);
        }

        std::string cpu_weight_str = optkit::utils::read_file(cgroup_path + "/cpu.weight");
        cpu.weight = std::stoll(optkit::utils::str_trim(cpu_weight_str));

        // Load Memory settings
        memory.max = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.max"));
        memory.high = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.high"));
        memory.swap_max = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.swap.max"));

        // Load PID settings
        std::string pids_max_str = optkit::utils::read_file(cgroup_path + "/pids.max");
        pids_max_str = optkit::utils::str_trim(pids_max_str);
        pid.max = (pids_max_str == "max") ? -1 : std::stoll(pids_max_str);

        // Load Core settings
        std::string freeze_str = optkit::utils::read_file(cgroup_path + "/cgroup.freeze");
        core.freeze = (optkit::utils::str_trim(freeze_str) == "1");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load cgroup settings: " << e.what() << "\n";
    }
}

bool CGroup::is_valid() const
{
    // Validate CPU settings
    if (cpu.period_us <= 0)
    {
        OPTKIT_WARN("cpu.period_us must be positive: {}", cpu.period_us);
        return false;
    }

    if (cpu.weight < 1 || cpu.weight > 10000)
    {
        OPTKIT_WARN("cpu.weight must be between 1 and 10000: {}", cpu.weight);
        return false;
    }

    if (cpu.weight_nice < -20 || cpu.weight_nice > 19)
    {
        OPTKIT_WARN("cpu.weight_nice must be between -20 and 19: {}", cpu.weight_nice);
        return false;
    }
    if (cpu.uclamp_min < 0 || cpu.uclamp_min > 100)
    {
        OPTKIT_WARN("cpu.uclamp_min must be between 0 and 100: {}", cpu.uclamp_min);
        return false;
    }

    if (cpu.uclamp_max < 0 || cpu.uclamp_max > 100)
    {
        OPTKIT_WARN("cpu.uclamp_max must be between 0 and 100: {}", cpu.uclamp_max);
        return false;
    }

    if (cpu.uclamp_min > cpu.uclamp_max)
    {
        OPTKIT_WARN("cpu.uclamp_min must not be greater than cpu.uclamp_max: {} > {}", cpu.uclamp_min, cpu.uclamp_max);
        return false;
    }
    if (pid.max < -1)
    {
        OPTKIT_WARN("pid.max must be greater than or equal to -1: {}", pid.max);
        return false;
    }

    return true;
}

nlohmann::json CGroup::to_json() const
{
    nlohmann::json j;
    j["name"] = cgroup_name;
    j["path"] = cgroup_path;

    // CPU
    j["cpu"]["quota_us"] = cpu.quota_us;
    j["cpu"]["period_us"] = cpu.period_us;
    j["cpu"]["max_burst_us"] = cpu.max_burst_us;
    j["cpu"]["weight"] = cpu.weight;
    j["cpu"]["weight_nice"] = cpu.weight_nice;
    j["cpu"]["uclamp_min"] = cpu.uclamp_min;
    j["cpu"]["uclamp_max"] = cpu.uclamp_max;
    j["cpu"]["idle"] = cpu.idle;

    // Memory
    j["memory"]["max"] = memory.max;
    j["memory"]["high"] = memory.high;
    j["memory"]["low"] = memory.low;
    j["memory"]["min"] = memory.min;
    j["memory"]["swap_max"] = memory.swap_max;
    j["memory"]["swap_high"] = memory.swap_high;
    j["memory"]["zswap_max"] = memory.zswap_max;
    j["memory"]["oom_group"] = memory.oom_group;
    j["memory"]["zswap_writeback"] = memory.zswap_writeback;

    // IO
    j["io"]["max"] = io.max;
    j["io"]["weight"] = io.weight;

    // PID
    j["pids"]["max"] = pid.max;

    // Core
    j["core"]["freeze"] = core.freeze;
    j["core"]["max_depth"] = core.max_depth;
    j["core"]["max_descendants"] = core.max_descendants;
    j["core"]["pressure"] = core.pressure;

    return j;
}
std::string CGroup::possible_values() const
{
    return "";
}