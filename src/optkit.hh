#pragma once

#include <string>
#include <vector>
#include <cstdlib>

#include "utils/utils.hh"
// =============================================
// Include subsystem headers
// =============================================

// based on the configuration, each module is responsible including "ways" for the module.
#include "core/pmu/cpu/module.hh"   // if use_msr is enabled, include defaults + use_msr module likewise for perf. same macro but different classes is defined for them.
#include "core/energy/cpu/rapl/module.hh"
#include "core/metrics/module.hh"
#include "core/frequency/module.hh"

namespace optkit::core
{
    struct OPTKIT_CONFIG
    {
        OPTKIT_CONFIG(bool create_folder = true, const std::string &execution_file = "") : create_folder{create_folder}, execution_file{execution_file} {}

        const bool create_folder;
        const std::string execution_file;
    };
    class OptimizerKit
    { 

    public:
        OptimizerKit(const OPTKIT_CONFIG config = {});
        ~OptimizerKit();

    private:
        void process_env_variables();

    private:
        const OPTKIT_CONFIG config;
    };
}

#define OPTKIT_INIT(...) optkit::core::OptimizerKit optkit{__VA_ARGS__}