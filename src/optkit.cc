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

    OPTKIT::OPTKIT(const OPTKIT_CONFIG config) : config{config}
    {
        optkit::utils::logger::BaseLogger::init(); // logger init
        int32_t paranoid = optkit::core::Query::paranoid();
        if (OPT_UNLIKELY(paranoid > 0))
        {
            OPTKIT_CORE_ERROR("perf_event_paranoid {}(current) must be <=0 for ACCURATE data collection by optimizer toolkit!", paranoid);
            OPTKIT_CORE_WARN("FOR ALL EVENTS: set perf_event_paranoid to -1 (SUGGESTED)");
            OPTKIT_CORE_WARN("FOR EVENTS WITH X SECURITY IMPLICATIONS: set perf_event_paranoid to 0");
            OPTKIT_CORE_WARN("USE: sudo sysctl kernel.perf_event_paranoid=<parameter>");
            std::exit(EXIT_FAILURE);
        }
        else
        {
            optkit::core::pmu::cpu::QueryPMU::init();                  // pmf init
            optkit::core::temperature::CPUTemperatureProfiler::init(); // discover hwmon temperatures.

            Query::create_folder = config.create_folder;
            if (OPT_LIKELY(Query::create_folder))
            {
                if (this->config.execution_file.size() > 0)
                    optkit::utils::EXECUTION_FOLDER_NAME = this->config.execution_file;

                optkit::utils::create_directory(optkit::utils::EXECUTION_FOLDER_NAME);
                OPTKIT_CORE_INFO("Execution file created {}", optkit::utils::EXECUTION_FOLDER_NAME);
            }
            else
            {
                OPTKIT_CORE_INFO("File creation skipped!");
            }

            process_env_variables();

            // reverse the scaling governor to the default one
            if (OPT_LIKELY(config.init_cpu_frequency))
            {
                // set cpufreq governor based on the available drivers. if it is new, set it to performance and warn the user.
                for (int i = 0; i < OPTKIT_ENV_CPU_NUM_SOCKETS; i++)
                {
                    std::string driver = frequency::QueryCPUFrequency::get_scaling_driver(i);
                    // if driver is old like acpi-cpufreq, then set governor to userspace else set governor to performance
                    if (driver == "acpi-cpufreq")
                    {
                        OPTKIT_CORE_INFO("Detected old cpufreq driver '{}' for socket {}. Setting governor to userspace", driver, i);
                        frequency::QueryCPUFrequency::set_scaling_governor("userspace", i);
                        OPTKIT_CORE_INFO("Current cpufreq governor for socket {}: {}", i, frequency::QueryCPUFrequency::get_scaling_governor(i));
                    }
                    else
                    {
                        OPTKIT_CORE_INFO("Detected new cpufreq driver '{}' for socket {}. Setting governor to performance", driver, i);
                        frequency::QueryCPUFrequency::set_scaling_governor("performance", i);
                        OPTKIT_CORE_INFO("Current cpufreq governor for socket {}: {}", i, frequency::QueryCPUFrequency::get_scaling_governor(i));
                    }
                }
            }
        }
    }

    void OPTKIT::process_env_variables()
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
        }
        else
        {

            if (socket0__enabled != nullptr)
            {
                Query::OPTKIT_SOCKET0__ENABLED = true;

                if (socket0__core_freq != nullptr)
                {
                    Query::OPTKIT_SOCKET0__CORE_FREQ = std::atol(socket0__core_freq);
                    core::frequency::CPUFrequency::set_core_frequency(Query::OPTKIT_SOCKET0__CORE_FREQ, 0);
                    OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET0__CORE_FREQ:{} ", Query::OPTKIT_SOCKET0__CORE_FREQ);
                }
                else
                {
                    OPTKIT_CORE_INFO("OPTKIT_SOCKET0__CORE_FREQ is not specified");
                }

                if (socket0__uncore_freq != nullptr)
                {

#if OPTKIT_ENV_CPU_INTEL
                    Query::OPTKIT_SOCKET0__UNCORE_FREQ = std::atol(socket0__uncore_freq);
                    core::frequency::CPUFrequency::set_uncore_frequency(Query::OPTKIT_SOCKET0__UNCORE_FREQ, 0);
                    OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET0__UNCORE_FREQ:{} ", Query::OPTKIT_SOCKET0__UNCORE_FREQ);
#endif
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
                    core::frequency::CPUFrequency::set_core_frequency(Query::OPTKIT_SOCKET1__CORE_FREQ, 1);
                    OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET1__CORE_FREQ:{} ", Query::OPTKIT_SOCKET1__CORE_FREQ);
                }
                else
                {
                    OPTKIT_CORE_INFO("OPTKIT_SOCKET1__CORE_FREQ is not specified");
                }

                if (socket1__uncore_freq != nullptr)
                {

#if OPTKIT_ENV_CPU_INTEL
                    Query::OPTKIT_SOCKET1__UNCORE_FREQ = std::atol(socket1__uncore_freq);
                    core::frequency::CPUFrequency::set_uncore_frequency(Query::OPTKIT_SOCKET1__UNCORE_FREQ, 1);
                    OPTKIT_CORE_INFO("---env read--- OPTKIT_SOCKET1__UNCORE_FREQ:{} ", Query::OPTKIT_SOCKET1__UNCORE_FREQ);
#endif
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
#if OPTKIT_CONF_RAPL_MACROS_ENABLED
        OPTKIT_CORE_INFO("OPTKIT_CONF_RAPL_MACROS_ENABLED: OK");
#else
        OPTKIT_CORE_WARN("OPTKIT_CONF_RAPL_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_FREQ_MACROS_ENABLED
        OPTKIT_CORE_INFO("OPTKIT_CONF_FREQ_MACROS_ENABLED: OK");
#else
        OPTKIT_CORE_WARN("OPTKIT_CONF_FREQ_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_MACROS_ENABLED
        OPTKIT_CORE_INFO("OPTKIT_CONF_PMU_MACROS_ENABLED: OK");
#else
        OPTKIT_CORE_WARN("OPTKIT_CONF_PMU_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_USE_PERF
        OPTKIT_CORE_INFO("OPTKIT_CONF_PMU_USE_PERF: OK");
#else
        OPTKIT_CORE_WARN("OPTKIT_CONF_PMU_USE_PERF: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_USE_MSR
        OPTKIT_CORE_INFO("OPTKIT_CONF_PMU_USE_MSR: OK");
#else
        OPTKIT_CORE_WARN("OPTKIT_CONF_PMU_USE_MSR: NOT ENABLED");
#endif
    }

    /**
     * @brief Destroy the Optimizer Kit:: Optimizer Kit object
     *
     *  Read all files under the execution directory & print them
     *  Destroy Query utility
     *
     */
    OPTKIT::~OPTKIT()
    {
        // reverse the scaling governor to the default one
        if (OPT_LIKELY(config.init_cpu_frequency))
        {
            for (int i = 0; i < OPTKIT_ENV_CPU_NUM_SOCKETS; i++)
            {
                std::string driver = frequency::QueryCPUFrequency::get_scaling_driver(i);
                if (driver == "acpi-cpufreq")
                {
                    OPTKIT_CORE_INFO("Reverting cpufreq driver '{}' for socket {} to ondemand", driver, i);
                    frequency::QueryCPUFrequency::set_scaling_governor("ondemand", i);
                    OPTKIT_CORE_INFO("Current cpufreq governor for socket {}: {}", i, frequency::QueryCPUFrequency::get_scaling_governor(i));

                    OPTKIT_CORE_INFO("Resetting CPU core and uncore frequencies for socket {}", i);
                    frequency::CPUFrequency::reset_core_frequency(i);
                    frequency::CPUFrequency::reset_uncore_frequency(i);
                }
                else
                {
                    OPTKIT_CORE_INFO("Reverting cpufreq driver '{}' for socket {} to powersave", driver, i);
                    frequency::QueryCPUFrequency::set_scaling_governor("powersave", i);
                    OPTKIT_CORE_INFO("Current cpufreq governor for socket {}: {}", i, frequency::QueryCPUFrequency::get_scaling_governor(i));

                    OPTKIT_CORE_INFO("Resetting CPU core and uncore frequencies for socket {}", i);
                    frequency::CPUFrequency::reset_core_frequency(i);
                    frequency::CPUFrequency::reset_uncore_frequency(i);
                }
            }
        }
        optkit::core::pmu::cpu::QueryPMU::destroy();
    }

} // namespace optkit::core
