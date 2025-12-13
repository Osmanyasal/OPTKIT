#pragma once

#include "sysconfig.hh"

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
// Accepts: true/false (boolean), 1/0 (number), or "true"/"false" (string)
inline bool get_bool_or(const nlohmann::json &j, const std::string &key, bool default_val)
{
    if (j.contains(key) && !j[key].is_null())
    {
        if (j[key].is_boolean())
            return j[key].get<bool>();
        if (j[key].is_number_integer())
            return j[key].get<int64_t>() != 0;
        if (j[key].is_string())
        {
            std::string val = j[key].get<std::string>();
            return val == "true" || val == "1" || val == "yes" || val == "on";
        }
    }
    return default_val;
}

// Helper to safely extract vector<int> with default value
inline std::vector<int16_t> get_int16_vec_or(const nlohmann::json &j, const std::string &key, const std::vector<int16_t> &default_val)
{
    if (j.contains(key) && !j[key].is_null() && j[key].is_array())
        return j[key].get<std::vector<int16_t>>();
    return default_val;
}

/**
 * @brief Load system configuration from env.json file
 *
 * @param json_path Path to the env.json file
 * @return SysConfig& Reference to singleton SysConfig instance
 * @throws std::runtime_error if file cannot be read or JSON is invalid
 */
inline SysConfig &load_system_config(const std::string &json_path)
{
    SysConfig &config = SysConfig::instance();
    try
    {
        // Read file content using optkit's utility
        std::string content = optkit::utils::read_file(json_path);

        // Parse JSON
        nlohmann::json j = nlohmann::json::parse(content);

        // Load CPU configuration
        CPU &cpu = *static_cast<CPU *>(config.get_module(CPU::name));
        if (j.contains("cpu") && j["cpu"].is_object())
        {
            const auto &cpu_json = j["cpu"];
            cpu.governor = get_string_or(cpu_json, "governor", "performance");
            cpu.affinity_cores = get_int16_vec_or(cpu_json, "affinity_cores", {});
            cpu.offline_cores = get_int16_vec_or(cpu_json, "offline_cores", {});
            cpu.core_freq = get_int64_or(cpu_json, "core_freq", 0);
            cpu.uncore_freq = get_int64_or(cpu_json, "uncore_freq", 0);
            cpu.smt_enabled = get_bool_or(cpu_json, "smt_enabled", true);
            cpu.turbo = get_bool_or(cpu_json, "turbo", true);
        }

        // Load Memory configuration
        Memory &mem = *static_cast<Memory *>(config.get_module(Memory::name));
        if (j.contains("memory") && j["memory"].is_object())
        {
            const auto &mem_json = j["memory"];
            mem.thp_mode = mem.from_string_thp_mode(get_string_or(mem_json, "thp_mode", Memory::to_string_thp_mode(Memory::THPMode::MADVISE)));
            mem.malloc_backend = mem.from_string_malloc_backend(get_string_or(mem_json, "malloc_backend", Memory::to_string_malloc_backend(Memory::Backend::GLIBC)));
            mem.hugepages_count = get_int64_or(mem_json, "hugepages_count", 0);
            mem.arena_max = get_int64_or(mem_json, "arena_max", 0);
            mem.swappiness = get_int64_or(mem_json, "swappiness", 60);
            mem.oom_kill_task = get_int64_or(mem_json, "oom_kill_task", 0);
            mem.drop_caches = get_bool_or(mem_json, "drop_caches", false);
            mem.mlock_all = get_bool_or(mem_json, "mlock_all", false);
        }

        // Load GPU configuration
        GPU &gpu = *static_cast<GPU *>(config.get_module(GPU::name));
        if (j.contains("gpu") && j["gpu"].is_object())
        {
            const auto &gpu_json = j["gpu"];
            gpu.device_name = get_string_or(gpu_json, "device_name", "");
            gpu.persistence_mode = get_string_or(gpu_json, "persistence_mode", "off");
            gpu.fan_speed = get_string_or(gpu_json, "fan_speed", "auto");
            gpu.core_freq_mhz = get_int64_or(gpu_json, "core_freq_mhz", 0);
            gpu.mem_freq_mhz = get_int64_or(gpu_json, "mem_freq_mhz", 0);
            gpu.power_limit_watts = get_int64_or(gpu_json, "power_limit_watts", 0);
            gpu.reset_device = get_bool_or(gpu_json, "reset_device", true);
        }

        // Load CGroup configuration
        CGroup &cg = *static_cast<CGroup *>(config.get_module(CGroup::name));
        if (j.contains("cgroup") && j["cgroup"].is_object())
        {
            const auto &cg_json = j["cgroup"];

            // Load cgroup path and name
            cg.cgroup_path = get_string_or(cg_json, "cgroup_path", "/sys/fs/cgroup/optkit");
            cg.cgroup_name = get_string_or(cg_json, "cgroup_name", "optkit");

            // Load CPU controller settings
            if (cg_json.contains("cpu") && cg_json["cpu"].is_object())
            {
                const auto &cpu_json = cg_json["cpu"];
                cg.cpu.quota_us = get_int64_or(cpu_json, "quota_us", -1);
                cg.cpu.period_us = get_int64_or(cpu_json, "period_us", 100000);
                cg.cpu.max_burst_us = get_int64_or(cpu_json, "max_burst_us", 0);
                // cg.cpu.weight = get_int64_or(cpu_json, "weight", 100);
                cg.cpu.weight_nice = get_int64_or(cpu_json, "weight_nice", 0);
                cg.cpu.uclamp_min = get_int64_or(cpu_json, "uclamp_min", 0);
                cg.cpu.uclamp_max = get_int64_or(cpu_json, "uclamp_max", 100);
                cg.cpu.idle = get_bool_or(cpu_json, "idle", false);
            }

            // Load Memory controller settings
            if (cg_json.contains("memory") && cg_json["memory"].is_object())
            {
                const auto &mem_json = cg_json["memory"];
                cg.memory.max = get_string_or(mem_json, "max", "max");
                cg.memory.high = get_string_or(mem_json, "high", "max");
                cg.memory.low = get_string_or(mem_json, "low", "0");
                cg.memory.min = get_string_or(mem_json, "min", "0");
                cg.memory.swap_max = get_string_or(mem_json, "swap_max", "max");
                cg.memory.swap_high = get_string_or(mem_json, "swap_high", "max");
                cg.memory.zswap_max = get_string_or(mem_json, "zswap_max", "max");
                cg.memory.oom_group = get_bool_or(mem_json, "oom_group", false);
                cg.memory.zswap_writeback = get_bool_or(mem_json, "zswap_writeback", true);
            }

            // Load PID controller settings
            if (cg_json.contains("pid") && cg_json["pid"].is_object())
            {
                const auto &pid_json = cg_json["pid"];
                cg.pid.max = get_string_or(pid_json, "max", "max");
            }
        }
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
