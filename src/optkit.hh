#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include "core/module.hh"
#include <utils/gpu.hh>

namespace optkit
{
    class OPTKIT_CONFIG
    {
    public:
        OPTKIT_CONFIG(bool create_folder = true,
                      const std::string &execution_folder_name = "",
                      const bool restore_cpu_frequency = true) : create_folder{create_folder}, execution_folder_name{execution_folder_name}, restore_cpu_frequency{restore_cpu_frequency} {}

        const bool create_folder;
        const std::string execution_folder_name;
        const bool restore_cpu_frequency; // set governor on init, reset frequencies and governor on exit
    };
    class OPTKIT
    {

    public:
        OPTKIT(const OPTKIT_CONFIG config = {});
        ~OPTKIT();

    private:
        void process_env_variables();

    private:
        const OPTKIT_CONFIG config;
    };
}
#ifndef OPTKIT_INIT
#define OPTKIT_INIT(...) \
    optkit::OPTKIT optkit { __VA_ARGS__ }
#endif