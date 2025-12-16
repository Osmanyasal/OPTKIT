#pragma once

#include "sysconfig.hh"
#include <unistd.h>
#include <cstdlib>

/**
 * @brief Set file ownership to the user who invoked sudo
 *
 * @param file_path Path to the file
 */
inline void set_original_user_ownership(const std::string &file_path)
{
    // Check if running under sudo
    const char *sudo_uid_str = std::getenv("SUDO_UID");
    const char *sudo_gid_str = std::getenv("SUDO_GID");

    if (sudo_uid_str && sudo_gid_str)
    {
        uid_t uid = static_cast<uid_t>(std::stoul(sudo_uid_str));
        gid_t gid = static_cast<gid_t>(std::stoul(sudo_gid_str));

        if (chown(file_path.c_str(), uid, gid) != 0)
        {
            OPTKIT_WARN("Failed to change ownership of {}: {}", file_path, strerror(errno));
        }
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
        std::cout << config << "\n";
        nlohmann::json j = config.to_json();
        if (truncate && optkit::utils::is_path_exists(json_path))
            optkit::utils::remove_file(json_path, true);
        optkit::utils::write_file(json_path, j.dump(indent));

        // Set ownership to original user if running under sudo
        set_original_user_ownership(json_path);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to save config to ") + json_path + ": " + e.what());
    }
}

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

        // Set ownership to original user if running under sudo
        set_original_user_ownership(json_path);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to save config to ") + json_path + ": " + e.what());
    }
}