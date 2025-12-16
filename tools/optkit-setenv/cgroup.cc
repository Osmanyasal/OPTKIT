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

#ifndef CGROUP2_SUPER_MAGIC
#define CGROUP2_SUPER_MAGIC 0x63677270
#endif

std::pair<std::string, std::string> CGroup::get_cgroup_path_and_name_of_process(pid_t pid)
{
    try
    {
        // Construct path to the process's cgroup file
        std::string cgroup_file = "/proc/" + std::to_string(pid) + "/cgroup";
        std::string content = optkit::utils::read_file(cgroup_file);

        // Parse cgroup v2 format: "0::/path/to/cgroup"
        size_t pos = content.find("::");
        if (pos == std::string::npos)
        {
            return {"", ""};
        }

        // Extract relative path after "::"
        std::string rel_path = content.substr(pos + 2);

        // Trim trailing whitespace
        size_t end = rel_path.find_last_not_of(" \n\r\t");
        if (end != std::string::npos)
        {
            rel_path = rel_path.substr(0, end + 1);
        }

        // Build absolute cgroup path
        std::string cgroup_path = CGroup::instance().get_cgroup_root() + rel_path;

        // Extract cgroup name (last component of path)
        std::string cgroup_name;
        size_t last_slash = rel_path.find_last_of('/');
        cgroup_name = (last_slash != std::string::npos) ? rel_path.substr(last_slash + 1) : rel_path;

        return {cgroup_path, cgroup_name};
    }
    catch (const std::exception &e)
    {
        OPTKIT_WARN("Failed to get cgroup for PID {}: {}", pid, e.what());
        return {"", ""};
    }
}

bool CGroup::is_cgroup_v2() const
{
    struct statfs buf;
    if (statfs("/sys/fs/cgroup", &buf) == 0)
    {
        return buf.f_type == CGROUP2_SUPER_MAGIC;
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
        exit(EXIT_FAILURE);
    }

    OPTKIT_INFO("CGroup created at path: {}", cgroup_path);
    return true;
}

bool CGroup::destroy_cgroup()
{
    if (cgroup_path.empty())
        return true;

    // Move all processes out first
    try
    {
        std::string procs = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/cgroup.procs"));
        std::string parent_procs = get_cgroup_root() + "/cgroup.procs";
        OPTKIT_INFO("Moving processes {} out of cgroup before destruction", procs);

        std::istringstream iss(procs);
        std::string pid_str;
        while (std::getline(iss, pid_str))
        {
            if (!pid_str.empty() && pid_str != "\n")
            {
                try
                {
                    optkit::utils::write_file(parent_procs, pid_str, false);
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
        OPTKIT_ERROR("CGroup destruction failed at path: {}", cgroup_path);
        return false;
    }
    OPTKIT_INFO("CGroup destroyed at path: {}", cgroup_path);
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

        // NOTE!
        // # echo "+cpu +memory -io" > cgroup.subtree_control
        // When multiple operations are specified as above, either they all succeed or fail.
        // If multiple operations on the same controller are specified, the last one is effective.
        // That's why we do them one by one here.
        optkit::utils::write_file(subtree_control, "+cpuset ", true);
        optkit::utils::write_file(subtree_control, "+cpu", true);
        optkit::utils::write_file(subtree_control, "+io", true);
        optkit::utils::write_file(subtree_control, "+memory", true);
        optkit::utils::write_file(subtree_control, "+hugetlb", true);
        optkit::utils::write_file(subtree_control, "+pids", true);
        optkit::utils::write_file(subtree_control, "+rdma", true);
        optkit::utils::write_file(subtree_control, "+misc", true);
        optkit::utils::write_file(subtree_control, "+dmem", true);
    }
    catch (const std::exception &e)
    {
        // May fail if already enabled, insufficient permissions
        // Not critical, continue
    }

    return true;
}

bool CGroup::add_process(pid_t process_pid)
{
    std::string procs_path = cgroup_path + "/cgroup.procs";
    try
    {
        optkit::utils::write_file(procs_path, std::to_string(process_pid));
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

    bool result = true;

    // Normalize memory values to bytes
    normalize_memory_values();

    // Enable controllers - continue on failure
    if (!enable_controllers())
        result = false;

    // Apply CPU settings - try to write all settings, report on failure and continue
    try
    {
        std::string cpu_max = (cpu.quota_us == -1) ? "max" : std::to_string(cpu.quota_us);
        cpu_max += " " + std::to_string(cpu.period_us);

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.max", cpu_max);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.max: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.max.burst", std::to_string(cpu.max_burst_us));
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.max.burst: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.weight.nice", std::to_string(cpu.weight_nice));
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.weight.nice: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.uclamp.min", std::to_string(cpu.uclamp_min) + ".00");
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.uclamp.min: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.uclamp.max", std::to_string(cpu.uclamp_max) + ".00");
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.uclamp.max: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/cpu.idle", cpu.idle ? "1" : "0");
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set cpu.idle: {}", e.what());
            result = false;
        }

        // Apply Memory settings
        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.max", memory.max);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.max: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.high", memory.high);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.high: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.low", memory.low);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.low: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.min", memory.min);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.min: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.swap.max", memory.swap_max);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.swap.max: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.swap.high", memory.swap_high);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.swap.high: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.zswap.max", memory.zswap_max);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.zswap.max: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.oom.group", memory.oom_group ? "1" : "0");
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.oom.group: {}", e.what());
            result = false;
        }

        try
        {
            optkit::utils::write_file(cgroup_path + "/memory.zswap.writeback", "0");
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set memory.zswap.writeback: {}", e.what());
            result = false;
        }

        // Apply PID settings
        std::string pid_max_str = (this->pid.max == -1) ? "max" : std::to_string(this->pid.max);
        try
        {
            optkit::utils::write_file(cgroup_path + "/pids.max", pid_max_str);
        }
        catch (const std::exception &e)
        {
            OPTKIT_WARN("Failed to set pids.max: {}", e.what());
            result = false;
        }

        // Add current process to cgroup - continue on failure
        if (!add_process(pid))
            result = false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error during cgroup setup: " << e.what() << "\n";
        result = false;
    }

    return result;
}

void CGroup::load_current_settings(pid_t process_pid)
{
    // Read current cgroup path for the process
    try
    {
        auto cgroup_path_and_name = get_cgroup_path_and_name_of_process(process_pid);
        this->cgroup_path = cgroup_path_and_name.first;
        this->cgroup_name = cgroup_path_and_name.second;

        if (cgroup_path.empty() || cgroup_path == get_cgroup_root())
        {
            // Process is in root cgroup, can't read settings
            std::cout << "Process is in root cgroup, no specific settings to load.\n";
            return;
        }
        else
        {

            std::cout << "Loading cgroup settings from path: " << cgroup_path << "\n";
            std::cout << "CGroup name: " << cgroup_name << "\n";
        }

        // Load CPU settings
        std::string cpu_max_str = optkit::utils::read_file(cgroup_path + "/cpu.max", true);
        auto tokens = optkit::utils::str_split(cpu_max_str, " ");
        if (tokens.size() >= 2)
        {
            cpu.quota_us = (tokens[0] == "max") ? -1 : std::stoll(tokens[0]);
            cpu.period_us = std::stoll(tokens[1]);
        }
        std::string cpu_max_burst_str = optkit::utils::read_file(cgroup_path + "/cpu.max.burst", true);
        cpu.max_burst_us = std::stoll(optkit::utils::str_trim(cpu_max_burst_str));
        std::string cpu_weight_nice_str = optkit::utils::read_file(cgroup_path + "/cpu.weight.nice", true);
        cpu.weight_nice = std::stoll(optkit::utils::str_trim(cpu_weight_nice_str));
        std::string cpu_uclamp_min_str = optkit::utils::read_file(cgroup_path + "/cpu.uclamp.min", true);
        cpu.uclamp_min = std::stoll(optkit::utils::str_trim(cpu_uclamp_min_str));
        std::string cpu_uclamp_max_str = optkit::utils::read_file(cgroup_path + "/cpu.uclamp.max", true);
        std::string trimmed_cpu_uclamp_max_str = optkit::utils::str_trim(cpu_uclamp_max_str);
        cpu.uclamp_max = std::stoll(trimmed_cpu_uclamp_max_str == "max" ? "100" : trimmed_cpu_uclamp_max_str);
        std::string cpu_idle_str = optkit::utils::read_file(cgroup_path + "/cpu.idle");
        cpu.idle = (optkit::utils::str_trim(cpu_idle_str) == "1");

        // Load Memory settings
        memory.max = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.max", true));
        memory.high = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.high", true));
        memory.low = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.low", true));
        memory.min = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.min", true));
        memory.swap_max = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.swap.max", true));
        memory.swap_high = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.swap.high", true));
        memory.zswap_max = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.zswap.max", true));
        // memory.zswap_writeback = optkit::utils::str_trim(optkit::utils::read_file(cgroup_path + "/memory.zswap.writeback",true));

        // Load PID settings
        std::string pids_max_str = optkit::utils::read_file(cgroup_path + "/pids.max", true);
        pids_max_str = optkit::utils::str_trim(pids_max_str);
        pid.max = (pids_max_str == "max") ? -1 : std::stoll(pids_max_str);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load cgroup settings: " << e.what() << "\n";
    }
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

    j["pid"]["max"] = pid.max;

    return j;
}

bool CGroup::is_valid() const
{
    // check cpu settings
    if (cpu.quota_us < -1)
    {
        OPTKIT_WARN("cpu.quota_us must be greater than, equal to -1: {}", cpu.quota_us);
        return false;
    }
    if (cpu.period_us != -1 && cpu.period_us <= 0)
    {
        OPTKIT_WARN("cpu.period_us must be positive, -1 for max: {}", cpu.period_us);
        return false;
    }

    if (cpu.max_burst_us < 0)
    {
        OPTKIT_WARN("cpu.max_burst_us must be non-negative: {}", cpu.max_burst_us);
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

    // check pid settings
    if (pid.max < -1)
    {
        OPTKIT_WARN("pid.max must be greater than or equal to -1 (unlimited): {}", pid.max);
        return false;
    }

    // check memory settings - values should already be normalized to bytes
    auto is_valid_memory_value = [](const std::string &val) -> bool
    {
        if (val == "max" || val.empty())
            return true;

        // After normalization, should be a plain number (bytes)
        try
        {
            std::stoll(val);
            return true;
        }
        catch (...)
        {
            OPTKIT_ERROR("Invalid memory value format: {}", val);
            return false;
        }
    };

    if (!is_valid_memory_value(memory.max))
    {
        OPTKIT_WARN("memory.max must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.max);
        return false;
    }

    if (!is_valid_memory_value(memory.high))
    {
        OPTKIT_WARN("memory.high must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.high);
        return false;
    }

    if (!is_valid_memory_value(memory.low))
    {
        OPTKIT_WARN("memory.low must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.low);
        return false;
    }

    if (!is_valid_memory_value(memory.min))
    {
        OPTKIT_WARN("memory.min must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.min);
        return false;
    }

    if (!is_valid_memory_value(memory.swap_max))
    {
        OPTKIT_WARN("memory.swap_max must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.swap_max);
        return false;
    }

    if (!is_valid_memory_value(memory.swap_high))
    {
        OPTKIT_WARN("memory.swap_high must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.swap_high);
        return false;
    }

    if (!is_valid_memory_value(memory.zswap_max))
    {
        OPTKIT_WARN("memory.zswap_max must be 'max', integer with size with suffix (B/KB/MB/GB/TB): {}", memory.zswap_max);
        return false;
    }

    return true;
}

std::string CGroup::possible_values() const
{
    std::ostringstream oss;

    oss << "CPU Settings:\n"
        << "\tperiod_us: positive integer (microseconds), typically up to 100000\n"
        << "\tquota_us: (max if -1), positive integer (microseconds), typically up to 100000\n"
        << "\tmax_burst_us: positive integer (microseconds)\n"
        << "\tweight_nice: -20 to 19 (weight priority)\n"
        << "\tuclamp_min: 0-100 (minimum CPU utilization percentage)\n"
        << "\tuclamp_max: 0-100 (maximum CPU utilization percentage)\n"
        << "\tidle: true, false (enable/disable idle CPU time)\n\n"

        << "Memory Settings:\n"
        << "\tmax: 'max', size string (e.g., '512M', '1G', bytes as number)\n"
        << "\thigh: 'max', size string (memory high threshold)\n"
        << "\tlow: 'max', size string (memory low threshold)\n"
        << "\tmin: 'max', size string (minimum memory guarantee)\n"
        << "\tswap_max: 'max', size string (maximum swap usage)\n"
        << "\tswap_high: 'max', size string (swap high threshold)\n"
        << "\tzswap_max: 'max', size string (zswap max usage)\n"
        << "\toom_group: true, false (OOM killer for entire group)\n"
        << "\tzswap_writeback: true, false (enable zswap writeback)\n\n"

        << "PID Settings:\n"
        << "  max (unlimited), positive integer (max process count)\n\n";

    return oss.str();
}

void CGroup::normalize_memory_values()
{
    // Helper to convert memory size string to bytes
    auto memory_to_bytes = [](const std::string &val) -> std::string
    {
        if (val == "max" || val.empty())
            return val;

        std::string trimmed = optkit::utils::str_trim(val);

        // Plain number (already in bytes)
        if (std::all_of(trimmed.begin(), trimmed.end(), ::isdigit))
        {
            return trimmed;
        }

        // Extract numeric part and suffix
        size_t suffix_pos = trimmed.find_first_not_of("0123456789");
        if (suffix_pos == std::string::npos)
        {
            return trimmed;
        }

        std::string num_str = trimmed.substr(0, suffix_pos);
        std::string suffix = trimmed.substr(suffix_pos);

        // Convert suffix to uppercase for comparison
        std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::toupper);

        int64_t value = std::stoll(num_str);
        int64_t multiplier = 1;

        // Binary (IEC) units - base 1024
        if (suffix == "KB" || suffix == "K")
            multiplier = 1024LL;
        else if (suffix == "MB" || suffix == "M")
            multiplier = 1024LL * 1024LL;
        else if (suffix == "GB" || suffix == "G")
            multiplier = 1024LL * 1024LL * 1024LL;
        else if (suffix == "TB" || suffix == "T")
            multiplier = 1024LL * 1024LL * 1024LL * 1024LL;
        else if (suffix == "B")
            multiplier = 1;
        else
            throw std::invalid_argument("Unknown memory suffix: " + suffix);

        return std::to_string(value * multiplier);
    };

    // Normalize all memory values to bytes
    try
    {
        memory.max = memory_to_bytes(memory.max);
        memory.high = memory_to_bytes(memory.high);
        memory.low = memory_to_bytes(memory.low);
        memory.min = memory_to_bytes(memory.min);
        memory.swap_max = memory_to_bytes(memory.swap_max);
        memory.swap_high = memory_to_bytes(memory.swap_high);
        memory.zswap_max = memory_to_bytes(memory.zswap_max);
    }
    catch (const std::exception &e)
    {
        OPTKIT_ERROR("Failed to normalize memory values: {}", e.what());
    }
}

// CPU sub-struct implementation
std::string CGroup::CPU::to_string() const
{
    std::ostringstream oss;
    oss << "CPU{quota=" << quota_us << "us"
        << ", period=" << period_us << "us"
        << ", burst=" << max_burst_us << "us"
        << ", weight_nice=" << weight_nice
        << ", uclamp=[" << uclamp_min << "," << uclamp_max << "]"
        << ", idle=" << (idle ? "true" : "false")
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
        << ", oom_group=" << (oom_group ? "true" : "false")
        << ", zswap_writeback=" << (zswap_writeback ? "true" : "false")
        << "}";
    return oss.str();
}

// PID sub-struct implementation
std::string CGroup::PID::to_string() const
{
    std::ostringstream oss;
    oss << "PID{max=" << max << "}";
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
        << ", " << pid.to_string()
        << "}";
    return oss.str();
}