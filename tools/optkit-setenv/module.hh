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
    virtual bool apply() = 0;
    virtual void load_current_settings(pid_t pid) = 0;
    virtual std::string possible_values() const = 0;
    virtual nlohmann::json to_json() const = 0;
};
