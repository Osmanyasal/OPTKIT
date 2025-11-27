#pragma once

#include "sysconfig.hh"

// Convert CPU to JSON
inline nlohmann::json cpu_to_json(const CPU &cpu)
{
    nlohmann::json j;
    j["governor"] = cpu.governor;
    j["affinity_cores"] = cpu.affinity_cores;
    j["offline_cores"] = cpu.offline_cores;
    j["core_freq"] = cpu.core_freq;
    j["uncore_freq"] = cpu.uncore_freq;
    j["smt_enabled"] = cpu.smt_enabled;
    j["turbo"] = cpu.turbo;
    return j;
}

// Convert Memory to JSON
inline nlohmann::json memory_to_json(const Memory &mem)
{
    nlohmann::json j;
    j["thp_mode"] = mem.thp_mode;
    j["malloc_backend"] = mem.malloc_backend;
    j["hugepages_count"] = mem.hugepages_count;
    j["arena_max"] = mem.arena_max;
    j["swappiness"] = mem.swappiness;
    j["oom_kill_task"] = mem.oom_kill_task;
    j["drop_caches"] = mem.drop_caches;
    j["mlock_all"] = mem.mlock_all;
    return j;
}

// Convert DiskIO to JSON
inline nlohmann::json diskio_to_json(const DiskIO &disk)
{
    nlohmann::json j;
    j["io_scheduler"] = disk.io_scheduler;
    j["mount_path"] = disk.mount_path;
    j["mount_options"] = disk.mount_options;
    j["aio_max_nr"] = disk.aio_max_nr;
    j["sync_disk"] = disk.sync_disk;
    j["use_direct_io"] = disk.use_direct_io;
    return j;
}

// Convert Kernel to JSON
inline nlohmann::json kernel_to_json(const Kernel &kern)
{
    nlohmann::json j;
    j["irqbalance"] = kern.irqbalance;
    j["isolate_irqs"] = kern.isolate_irqs;
    j["isolate_cpus"] = kern.isolate_cpus;
    j["mitigations"] = kern.mitigations;
    j["clocksource"] = kern.clocksource;
    j["sched_min_granularity_ms"] = kern.sched_min_granularity_ms;
    j["ulimit_n"] = kern.ulimit_n;
    j["disable_watchdogs"] = kern.disable_watchdogs;
    return j;
}

// Convert GPU to JSON
inline nlohmann::json gpu_to_json(const GPU &gpu)
{
    nlohmann::json j;
    j["persistence_mode"] = gpu.persistence_mode;
    j["fan_speed"] = gpu.fan_speed;
    j["core_freq_mhz"] = gpu.core_freq_mhz;
    j["mem_freq_mhz"] = gpu.mem_freq_mhz;
    j["power_limit_watts"] = gpu.power_limit_watts;
    j["reset_stats"] = gpu.reset_stats;
    return j;
}

// Convert CGroup to JSON
inline nlohmann::json cgroup_to_json(const CGroup &cg)
{
    nlohmann::json j;
    j["cpuset"] = cg.cpuset;
    j["mem_limit"] = cg.mem_limit;
    j["io_limit_read"] = cg.io_limit_read;
    j["io_limit_write"] = cg.io_limit_write;
    j["freeze_state"] = cg.freeze_state;
    j["cpu_quota_us"] = cg.cpu_quota_us;
    j["cpu_period_us"] = cg.cpu_period_us;
    j["mem_swappiness"] = cg.mem_swappiness;
    return j;
}

// Convert SystemConfig to JSON
inline nlohmann::json system_config_to_json(const SystemConfig &config)
{
    nlohmann::json j;
    j["cpu"] = cpu_to_json(config.cpu);
    j["memory"] = memory_to_json(config.memory);
    j["disk_io"] = diskio_to_json(config.disk_io);
    j["kernel"] = kernel_to_json(config.kernel);
    j["gpu"] = gpu_to_json(config.gpu);
    j["cgroup"] = cgroup_to_json(config.cgroup);
    return j;
}

/**
 * @brief Write system configuration to JSON file
 *
 * @param config SystemConfig to write
 * @param json_path Output file path
 * @param indent Number of spaces for JSON indentation (default: 4)
 * @throws std::runtime_error if file cannot be written
 */
inline void save_system_config(const SystemConfig &config, const std::string &json_path, bool truncate = true, int indent = 4)
{
    try
    {
        nlohmann::json j = system_config_to_json(config);
        if (truncate && optkit::utils::is_path_exists(json_path))
            optkit::utils::remove_file(json_path);
        optkit::utils::write_file(json_path, j.dump(indent));
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to save config to ") + json_path + ": " + e.what());
    }
}

/**
 * @brief Create an empty/default SystemConfig template
 *
 * @return SystemConfig with default/empty values
 */
inline SystemConfig create_empty_config()
{
    SystemConfig config;

    // CPU defaults
    config.cpu.governor = "";
    config.cpu.affinity_cores = {};
    config.cpu.offline_cores = {};
    config.cpu.core_freq = 0;
    config.cpu.uncore_freq = 0;
    config.cpu.smt_enabled = false;
    config.cpu.turbo = false;

    // Memory defaults
    config.memory.thp_mode = Memory::THPMode::MADVISE;
    config.memory.malloc_backend = Memory::Backend::GLIBC;
    config.memory.hugepages_count = 0;
    config.memory.arena_max = 0;
    config.memory.swappiness = 0;
    config.memory.oom_kill_task = 0;
    config.memory.drop_caches = false;
    config.memory.mlock_all = false;

    // DiskIO defaults
    config.disk_io.io_scheduler = "";
    config.disk_io.mount_path = "";
    config.disk_io.mount_options = "";
    config.disk_io.aio_max_nr = 0;
    config.disk_io.sync_disk = false;
    config.disk_io.use_direct_io = false;

    // Kernel defaults
    config.kernel.irqbalance = "";
    config.kernel.isolate_irqs = "";
    config.kernel.isolate_cpus = "";
    config.kernel.mitigations = "";
    config.kernel.clocksource = "";
    config.kernel.sched_min_granularity_ms = 0;
    config.kernel.ulimit_n = 0;
    config.kernel.disable_watchdogs = false;

    // GPU defaults
    config.gpu.persistence_mode = "";
    config.gpu.fan_speed = "";
    config.gpu.core_freq_mhz = 0;
    config.gpu.mem_freq_mhz = 0;
    config.gpu.power_limit_watts = 0;
    config.gpu.reset_stats = false;

    // CGroup defaults
    config.cgroup.cpuset = "";
    config.cgroup.mem_limit = "";
    config.cgroup.io_limit_read = "";
    config.cgroup.io_limit_write = "";
    config.cgroup.freeze_state = "";
    config.cgroup.cpu_quota_us = 0;
    config.cgroup.cpu_period_us = 0;
    config.cgroup.mem_swappiness = 0;

    return config;
}