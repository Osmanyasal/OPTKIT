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
#include "optkit.hh"

class Module
{
public:
    virtual ~Module() {}
    virtual std::string to_string() const = 0;
    virtual bool is_valid() const = 0;
    virtual bool apply() = 0;
    virtual void load_current_settings(pid_t pid) = 0;
};
