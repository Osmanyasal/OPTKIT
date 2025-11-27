#pragma once
#include "helper.hh"

class Module
{
public:
    virtual ~Module() {}
    virtual std::string to_string() const = 0;
    virtual bool is_valid() const = 0;
    virtual bool apply() = 0;
};
