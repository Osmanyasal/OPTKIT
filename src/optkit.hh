#pragma once

#include <string>
#include <vector>
#include <cstdlib>

#include "utils/utils.hh"
#include "utils/deployment/deployment_config.hh"
#include "core/profiling/module.hh"
#include "core/freq/module.hh"
#include "core/event_recepies/module.hh"

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

        /**
         * @brief Returns current perf_event_paranoid value from "/proc/sys/kernel/perf_event_paranoid"<br>
         * Suggested value is -1 but 0 is also okay. cannot gurantee to accuretely measure for values above >0.
         *
         * @return int32_t paranoid value.
         */
        int32_t paranoid();

    private:
        void process_env_variables();

    private:
        const OPTKIT_CONFIG config;
    };
}

#ifdef CONF__OPTKIT__ENABLED
    #define OPTKIT_INIT(...) optkit::core::OptimizerKit optkit{__VA_ARGS__}
#else 
    #define OPTKIT_INIT(...)
#endif