#include "optkit.hh"

namespace optkit
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
        int32_t paranoid = optkit::Query::paranoid();
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

#if OPTKIT_CONF_RAPL_MACROS_ENABLED
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_RAPL_MACROS_ENABLED: OK");
#else
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_RAPL_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_FREQ_MACROS_ENABLED
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_FREQ_MACROS_ENABLED: OK");
#else
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_FREQ_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_MACROS_ENABLED
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_MACROS_ENABLED: OK");
#else
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_MACROS_ENABLED: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_USE_PERF
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_USE_PERF: OK");
#else
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_USE_PERF: NOT ENABLED");
#endif

#if OPTKIT_CONF_PMU_USE_MSR
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_USE_MSR: OK");
#else
            OPTKIT_CORE_DEBUG("OPTKIT_CONF_PMU_USE_MSR: NOT ENABLED");
#endif

            Query::create_folder = config.create_folder;
            if (OPT_LIKELY(Query::create_folder))
            {
                if (this->config.execution_file.size() > 0)
                    optkit::utils::EXECUTION_FOLDER_NAME = this->config.execution_file;

                optkit::utils::create_directory(optkit::utils::EXECUTION_FOLDER_NAME);
                OPTKIT_CORE_DEBUG("Execution file created {}", optkit::utils::EXECUTION_FOLDER_NAME);
            }
            else
            {
                OPTKIT_CORE_DEBUG("File creation skipped!");
            }

            // pmf init
            optkit::pmu::cpu::Query::init();

            // try to init all gpu vendors
            for (optkit::gpu::GpuVendor vendor = optkit::gpu::GpuVendor::BEGIN; vendor < optkit::gpu::GpuVendor::END; vendor = static_cast<optkit::gpu::GpuVendor>(static_cast<int>(vendor) + 1))
            {
                if (optkit::gpu::Query::init(vendor))
                {
                    if (!optkit::gpu::Query::is_device_exists(vendor))
                        optkit::gpu::Query::shutdown(vendor);
                }
            }

            // discover hwmon temperatures.
            optkit::temperature::hwmon::Profiler::init();

            if (config.init_cpu_frequency)
                for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                    optkit::frequency::cpu::Frequency::get_uncore_min_max(socket); // cache default uncore freq on init
        }
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
        // try to shutdown all gpu vendors
        for (optkit::gpu::GpuVendor vendor = optkit::gpu::GpuVendor::BEGIN; vendor < optkit::gpu::GpuVendor::END; vendor = static_cast<optkit::gpu::GpuVendor>(static_cast<int>(vendor) + 1))
        {
            if (optkit::gpu::Query::is_init(vendor))
                optkit::gpu::Query::shutdown(vendor);
        }

        if (config.init_cpu_frequency)
            for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket); // restore default uncore freq on exit.

        optkit::pmu::cpu::Query::destroy();
        optkit::utils::logger::BaseLogger::shutdown(); // logger shutdown.
    }

} // namespace optkit
