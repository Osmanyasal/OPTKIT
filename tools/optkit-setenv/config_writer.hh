#pragma once

#include "sysconfig.hh"

/**
 * @brief Write system configuration to JSON file
 *
 * @param config_str Configuration string in JSON format
 * @param json_path Output file path
 * @param indent Number of spaces for JSON indentation (default: 4)
 * @throws std::runtime_error if file cannot be written
 */
inline void save_system_config(const std::string &config_str, const std::string &json_path, bool truncate = true, int indent = 4)
{
    try
    {
        if (truncate && optkit::utils::is_path_exists(json_path))
            optkit::utils::remove_file(json_path);
        optkit::utils::write_file(json_path, config_str);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to save config to ") + json_path + ": " + e.what());
    }
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
        nlohmann::json j = config.to_json();
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
    config.add_module(CPU::name, std::make_unique<CPU>())
        .add_module(GPU::name, std::make_unique<GPU>())
        .add_module(Memory::name, std::make_unique<Memory>())
        .add_module(CGroup::name, std::make_unique<CGroup>());
    return config;
}