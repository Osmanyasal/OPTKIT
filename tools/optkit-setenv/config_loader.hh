#pragma once

#include <fstream>
#include <stdexcept>
#include "config.hh"

// Helper to safely extract int64_t with default value
inline int64_t get_int64_or(const nlohmann::json &j, const std::string &key, int64_t default_val)
{
    if (j.contains(key) && !j[key].is_null())
    {
        if (j[key].is_number())
            return j[key].get<int64_t>();
        if (j[key].is_string())
            return std::stoll(j[key].get<std::string>());
    }
    return default_val;
}

// Helper to safely extract string with default value
inline std::string get_string_or(const nlohmann::json &j, const std::string &key, const std::string &default_val)
{
    if (j.contains(key) && !j[key].is_null() && j[key].is_string())
        return j[key].get<std::string>();
    return default_val;
}

// Helper to safely extract bool with default value
inline bool get_bool_or(const nlohmann::json &j, const std::string &key, bool default_val)
{
    if (j.contains(key) && !j[key].is_null() && j[key].is_boolean())
        return j[key].get<bool>();
    return default_val;
}

// Helper to safely extract vector<int> with default value
inline std::vector<int> get_int_vec_or(const nlohmann::json &j, const std::string &key, const std::vector<int> &default_val)
{
    if (j.contains(key) && !j[key].is_null() && j[key].is_array())
        return j[key].get<std::vector<int>>();
    return default_val;
}

// Load CPU configuration from JSON
inline CPU load_cpu_config(const nlohmann::json &j)
{
    CPU cpu;
    if (j.contains("cpu") && j["cpu"].is_object())
    {
        const auto &cpu_json = j["cpu"];
        cpu.governor = get_string_or(cpu_json, "governor", "");
        cpu.affinity_cores = get_int_vec_or(cpu_json, "affinity_cores", {});
        cpu.offline_cores = get_int_vec_or(cpu_json, "offline_cores", {});
        cpu.core_freq = get_int64_or(cpu_json, "core_freq", 0);
        cpu.uncore_freq = get_int64_or(cpu_json, "uncore_freq", 0);
        cpu.smt_enabled = get_bool_or(cpu_json, "smt_enabled", true);
        cpu.turbo = get_bool_or(cpu_json, "turbo", true);
    }
    return cpu;
}

// Load Memory configuration from JSON
inline Memory load_memory_config(const nlohmann::json &j)
{
    Memory mem;
    if (j.contains("memory") && j["memory"].is_object())
    {
        const auto &mem_json = j["memory"];
        mem.thp_mode = get_string_or(mem_json, "thp_mode", "");
        mem.numa_policy = get_string_or(mem_json, "numa_policy", "");
        mem.malloc_backend = get_string_or(mem_json, "malloc_backend", "");
        mem.hugepages_count = get_int64_or(mem_json, "hugepages_count", 0);
        mem.arena_max = get_int64_or(mem_json, "arena_max", 0);
        mem.swappiness = get_int64_or(mem_json, "swappiness", 60);
        mem.oom_kill_task = get_int64_or(mem_json, "oom_kill_task", 0);
        mem.drop_caches = get_bool_or(mem_json, "drop_caches", false);
        mem.mlock_all = get_bool_or(mem_json, "mlock_all", false);
    }
    return mem;
}

// Load DiskIO configuration from JSON
inline DiskIO load_diskio_config(const nlohmann::json &j)
{
    DiskIO disk;
    if (j.contains("disk_io") && j["disk_io"].is_object())
    {
        const auto &disk_json = j["disk_io"];
        disk.io_scheduler = get_string_or(disk_json, "io_scheduler", "");
        disk.mount_path = get_string_or(disk_json, "mount_path", "");
        disk.mount_options = get_string_or(disk_json, "mount_options", "");
        disk.aio_max_nr = get_int64_or(disk_json, "aio_max_nr", 0);
        disk.sync_disk = get_bool_or(disk_json, "sync_disk", false);
        disk.use_direct_io = get_bool_or(disk_json, "use_direct_io", false);
    }
    return disk;
}

// Load Kernel configuration from JSON
inline Kernel load_kernel_config(const nlohmann::json &j)
{
    Kernel kern;
    if (j.contains("kernel") && j["kernel"].is_object())
    {
        const auto &kern_json = j["kernel"];
        kern.irqbalance = get_string_or(kern_json, "irqbalance", "");
        kern.isolate_irqs = get_string_or(kern_json, "isolate_irqs", "");
        kern.isolate_cpus = get_string_or(kern_json, "isolate_cpus", "");
        kern.mitigations = get_string_or(kern_json, "mitigations", "");
        kern.clocksource = get_string_or(kern_json, "clocksource", "");
        kern.sched_min_granularity_ms = get_int64_or(kern_json, "sched_min_granularity_ms", 0);
        kern.ulimit_n = get_int64_or(kern_json, "ulimit_n", 0);
        kern.disable_watchdogs = get_bool_or(kern_json, "disable_watchdogs", false);
    }
    return kern;
}

// Load GPU configuration from JSON
inline GPU load_gpu_config(const nlohmann::json &j)
{
    GPU gpu;
    if (j.contains("gpu") && j["gpu"].is_object())
    {
        const auto &gpu_json = j["gpu"];
        gpu.persistence_mode = get_string_or(gpu_json, "persistence_mode", "");
        gpu.fan_speed = get_string_or(gpu_json, "fan_speed", "");
        gpu.core_freq_mhz = get_int64_or(gpu_json, "core_freq_mhz", 0);
        gpu.mem_freq_mhz = get_int64_or(gpu_json, "mem_freq_mhz", 0);
        gpu.power_limit_watts = get_int64_or(gpu_json, "power_limit_watts", 0);
        gpu.reset_stats = get_bool_or(gpu_json, "reset_stats", false);
    }
    return gpu;
}

// Load CGroup configuration from JSON
inline CGroup load_cgroup_config(const nlohmann::json &j)
{
    CGroup cg;
    if (j.contains("cgroup") && j["cgroup"].is_object())
    {
        const auto &cg_json = j["cgroup"];
        cg.cpuset = get_string_or(cg_json, "cpuset", "");
        cg.mem_limit = get_string_or(cg_json, "mem_limit", "");
        cg.io_limit_read = get_string_or(cg_json, "io_limit_read", "");
        cg.io_limit_write = get_string_or(cg_json, "io_limit_write", "");
        cg.freeze_state = get_string_or(cg_json, "freeze_state", "");
        cg.cpu_quota_us = get_int64_or(cg_json, "cpu_quota_us", 0);
        cg.cpu_period_us = get_int64_or(cg_json, "cpu_period_us", 0);
        cg.mem_swappiness = get_int64_or(cg_json, "mem_swappiness", 0);
    }
    return cg;
}

/**
 * @brief Load system configuration from env.json file
 *
 * @param json_path Path to the env.json file
 * @return SystemConfig Parsed configuration structure
 * @throws std::runtime_error if file cannot be read or JSON is invalid
 */
inline SystemConfig load_system_config(const std::string &json_path)
{
    SystemConfig config;

    try
    {
        // Read file content using optkit's utility
        std::string content = optkit::utils::read_file(json_path);

        // Parse JSON
        nlohmann::json j = nlohmann::json::parse(content);

        // Load each component
        config.cpu = load_cpu_config(j);
        config.memory = load_memory_config(j);
        config.disk_io = load_diskio_config(j);
        config.kernel = load_kernel_config(j);
        config.gpu = load_gpu_config(j);
        config.cgroup = load_cgroup_config(j);
    }
    catch (const nlohmann::json::parse_error &e)
    {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to load config from ") + json_path + ": " + e.what());
    }

    return config;
}
