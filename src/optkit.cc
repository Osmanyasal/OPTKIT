#include "optkit.hh"

namespace optkit::core
{
    /**
     * @brief Construct a new Optimizer Kit:: Optimizer Kit object
     *
     *  Creates Execution Directory | use existing/random one
     *  Starts Logger and Query class
     *
     *
     * @param execution_file
     */

    OptimizerKit::OptimizerKit(const OPTKIT_CONFIG config) : config{config}
    {
        optkit::utils::logger::BaseLogger::init();
        int32_t paranoid = this->paranoid();
        if (OPT_UNLIKELY(paranoid > 0))
        {
            OPTKIT_CORE_ERROR("perf_event_paranoid {}(current) must be <=0 for ACCURATE data collection by optimizer toolkit!", paranoid);
            OPTKIT_CORE_WARN("FOR ALL EVENTS: set perf_event_paranoid to -1 (SUGGESTED)");
            OPTKIT_CORE_WARN("FOR EVENTS WITH NO SECURITY IMPLICATIONS: set perf_event_paranoid to 0");
            OPTKIT_CORE_WARN("USE: sudo sysctl kernel.perf_event_paranoid=<parameter>");
            exit(EXIT_FAILURE);
        }
        else if (paranoid <= 0)
        {

            if (OPT_LIKELY(config.create_folder))
            {
                if (this->config.execution_file.size() > 0)
                    EXECUTION_FOLDER_NAME = this->config.execution_file;
                    
                ::create_directory(EXECUTION_FOLDER_NAME);
                OPTKIT_CORE_INFO("Execution file created {}", EXECUTION_FOLDER_NAME);
            }
            else
            {
                OPTKIT_CORE_INFO("File creation skipped!");
            }

            process_env_variables();
        }
        OPTKIT_CORE_GANTT_PROFILE_BEGIN_SESSION("Optimizer Toolkit", "optkit_gui_gantt_instr.json");
    }

    void OptimizerKit::process_env_variables()
    {
        const char *socket0__enabled = std::getenv("OPTKIT_SOCKET0__ENABLED");
        const char *socket0__core_freq = std::getenv("OPTKIT_SOCKET0__CORE_FREQ");
        const char *socket0__uncore_freq = std::getenv("OPTKIT_SOCKET0__UNCORE_FREQ");

        const char *socket1__enabled = std::getenv("OPTKIT_SOCKET1__ENABLED");
        const char *socket1__core_freq = std::getenv("OPTKIT_SOCKET1__CORE_FREQ");
        const char *socket1__uncore_freq = std::getenv("OPTKIT_SOCKET1__UNCORE_FREQ");

        if (socket0__enabled == nullptr && socket1__enabled == nullptr)
        {
            OPTKIT_CORE_INFO("OPTKIT_SOCKET0__ENABLED and OPTKIT_SOCKET1__ENABLED are not specified");
            return;
        }

        if (socket0__enabled != nullptr)
        {
            Query::OPTKIT_SOCKET0__ENABLED = true;

            if (socket0__core_freq != nullptr)
            {
                Query::OPTKIT_SOCKET0__CORE_FREQ = std::atol(socket0__core_freq);
                freq::CPUFrequency::set_core_frequency(Query::OPTKIT_SOCKET0__CORE_FREQ, 0);
                OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET0__CORE_FREQ:{} ", Query::OPTKIT_SOCKET0__CORE_FREQ);
            }
            else
            {
                OPTKIT_CORE_INFO("OPTKIT_SOCKET0__CORE_FREQ is not specified");
            }

            if (socket0__uncore_freq != nullptr)
            {
                Query::OPTKIT_SOCKET0__UNCORE_FREQ = std::atol(socket0__uncore_freq);
                freq::CPUFrequency::set_uncore_frequency(Query::OPTKIT_SOCKET0__UNCORE_FREQ, 0);
                OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET0__UNCORE_FREQ:{} ", Query::OPTKIT_SOCKET0__UNCORE_FREQ);
            }
            else
            {
                OPTKIT_CORE_INFO("OPTKIT_SOCKET0__UNCORE_FREQ is not specified");
            }
        }
        else
        {
            OPTKIT_CORE_INFO("OPTKIT_SOCKET0__ENABLED is not specified");
        }

        if (socket1__enabled != nullptr)
        {
            Query::OPTKIT_SOCKET1__ENABLED = true;

            if (socket1__core_freq != nullptr)
            {
                Query::OPTKIT_SOCKET1__CORE_FREQ = std::atol(socket1__core_freq);
                freq::CPUFrequency::set_core_frequency(Query::OPTKIT_SOCKET1__CORE_FREQ, 1);
                OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET1__CORE_FREQ:{} ", Query::OPTKIT_SOCKET1__CORE_FREQ);
            }
            else
            {
                OPTKIT_CORE_INFO("OPTKIT_SOCKET1__CORE_FREQ is not specified");
            }

            if (socket1__uncore_freq != nullptr)
            {
                Query::OPTKIT_SOCKET1__UNCORE_FREQ = std::atol(socket1__uncore_freq);
                freq::CPUFrequency::set_uncore_frequency(Query::OPTKIT_SOCKET1__UNCORE_FREQ, 1);
                OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET1__UNCORE_FREQ:{} ", Query::OPTKIT_SOCKET1__UNCORE_FREQ);
            }
            else
            {
                OPTKIT_CORE_INFO("OPTKIT_SOCKET1__UNCORE_FREQ is not specified");
            }
        }
        else
        {
            OPTKIT_CORE_INFO("OPTKIT_SOCKET1__ENABLED is not specified");
        }
    }

    /**
     * @brief Destroy the Optimizer Kit:: Optimizer Kit object
     *
     *  Read all files under the execution directory & print them
     *  Destroy Query utility
     *
     */
    OptimizerKit::~OptimizerKit()
    {
        optkit::core::pmu::QueryPMU::destroy();
        OPTKIT_CORE_GANTT_PROFILE_END_SESSION();
    }

    int32_t OptimizerKit::paranoid()
    {
        static std::string value = read_file("/proc/sys/kernel/perf_event_paranoid");
        return std::stoi(value);
    }

} // namespace optkit::core
