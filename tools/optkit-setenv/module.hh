#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <cerrno>
#include <sched.h>
#include <memory> // For std::unique_ptr
#include "optkit.hh"

// C++11 compatible make_unique implementation (C++14 feature)
#if __cplusplus < 201402L
namespace std
{
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif

class Module
{
public:
    virtual ~Module() = default;
    virtual std::string to_string() const = 0;
    virtual bool is_valid() const = 0;
    virtual bool apply(pid_t pid) = 0;
    virtual void load_current_settings(pid_t pid) = 0;
    virtual std::string possible_values() const = 0;
    virtual nlohmann::json to_json() const = 0;
};

#define OPTKIT_EXECUTEME "optkit_execute_me.sh"
#define OPTKIT_EXECUTEME_header "#!/bin/bash\n"
#define OPTKIT_EXECUTEME_footer "# End of script\n"

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
