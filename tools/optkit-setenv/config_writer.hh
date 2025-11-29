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
    j["thp_mode"] = mem.to_string_thp_mode(mem.thp_mode);
    j["malloc_backend"] = mem.to_string_malloc_backend(mem.malloc_backend);
    j["hugepages_count"] = mem.hugepages_count;
    j["arena_max"] = mem.arena_max;
    j["swappiness"] = mem.swappiness;
    j["oom_kill_task"] = mem.oom_kill_task;
    j["drop_caches"] = mem.drop_caches;
    j["mlock_all"] = mem.mlock_all;
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

// Convert SysConfig to JSON
inline nlohmann::json system_config_to_json(const SysConfig &config)
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
 * @param config SysConfig to write
 * @param json_path Output file path
 * @param indent Number of spaces for JSON indentation (default: 4)
 * @throws std::runtime_error if file cannot be written
 */
inline void save_system_config(const SysConfig &config, const std::string &json_path, bool truncate = true, int indent = 4)
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
 * @brief Create an empty/default SysConfig template
 *
 * @return SysConfig with default/empty values
 */
inline SysConfig create_empty_config()
{
    SysConfig config{};
    config.add_module(std::make_unique<CPU>())
        .add_module(std::make_unique<GPU>())
        .add_module(std::make_unique<Memory>())
        .add_module(std::make_unique<CGroup>());
    return config;
}