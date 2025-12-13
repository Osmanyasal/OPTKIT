#pragma once

#include "sysconfig.hh"

/**
 * @brief Write system configuration string to JSON file
 *
 * @param config_str Configuration string in JSON format
 * @param json_path Output file path
 * @param truncate Whether to truncate existing file
 * @throws std::runtime_error if file cannot be written
 */
inline void save_system_config(const std::string &config_str, const std::string &json_path, bool truncate = true)
{
    try
    {
        if (truncate && optkit::utils::is_path_exists(json_path))
            if (optkit::utils::remove_file(json_path, true))
                optkit::utils::write_file(json_path, config_str);
            else
                throw std::runtime_error("Failed to remove existing file: " + json_path);
        else
            optkit::utils::write_file(json_path, config_str);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to save config to ") + json_path + ": " + e.what());
    }
}