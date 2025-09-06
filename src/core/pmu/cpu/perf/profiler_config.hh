#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT

#include <cstring>
#include <cstdint>
#include <linux/perf_event.h>
#include "utils/base_profiler.hh"

namespace optkit::core::pmu::cpu::perf
{
    /**
     * @brief perf_event_open profiler config.
     * @see perf_event.h for more detail.
     *
     */
    struct PerfProfilerConfig : public optkit::core::ProfilerConfig
    {
        /**
         * @brief Construct a new PerfProfilerConfig object
         *
         * @param block_name Name of the profiling block (base ProfilerConfig)
         * @param measurement_type Type of measurement (base ProfilerConfig)
         * @param is_grouped Indicates if all events in the BlockProfiler should be grouped (see perf_event_open man page)
         * @param pid See perf_event_open man page for meaning
         * @param cpu See perf_event_open man page for meaning
         * @param is_reset_after_read Reset after read (base ProfilerConfig)
         * @param dump_results_to_file Dump results to file (base ProfilerConfig)
         * @param verbose Verbose output (base ProfilerConfig)
         *
         * pid/cpu combinations:
         *  - pid == 0 and cpu == -1: Measures the calling process/thread on any CPU.
         *  - pid == 0 and cpu >= 0: Measures the calling process/thread only when running on the specified CPU.
         *  - pid > 0 and cpu == -1: Measures the specified process/thread on any CPU.
         *  - pid > 0 and cpu >= 0: Measures the specified process/thread only when running on the specified CPU.
         *  - pid == -1 and cpu >= 0: Measures all processes/threads on the specified CPU. Requires CAP_PERFMON (since Linux 5.8) or CAP_SYS_ADMIN capability or /proc/sys/kernel/perf_event_paranoid < 1.
         *  - pid == -1 and cpu == -1: Invalid setting, returns error.
         *
         * @param perf_event_config Optional perf_event_attr configuration (second constructor)
         */

        PerfProfilerConfig(
            const char *block_name,
            bool is_grouped = false,
            int32_t pid = 0,  // current process
            int32_t cpu = -1, // any cpu
            const char *measurement_type = "cpu_pmu",
            bool is_reset_after_read = true,
            bool dump_results_to_file = Query::create_folder,
            bool verbose = !Query::create_folder);

        PerfProfilerConfig(
            const char *block_name,
            const perf_event_attr &perf_event_config,
            bool is_grouped = false,
            int32_t pid = 0,  // current process
            int32_t cpu = -1, // any cpu
            const char *measurement_type = "cpu_pmu",
            bool is_reset_after_read = true,
            bool dump_results_to_file = Query::create_folder,
            bool verbose = !Query::create_folder);

        virtual ~PerfProfilerConfig() {}

        PerfProfilerConfig &set_grouped(bool is_grouped)
        {
            if (is_grouped)
                this->perf_event_config.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
            else
                this->perf_event_config.read_format = 0;
            this->is_grouped = is_grouped;
            return *this;
        }

        bool is_grouped;
        int32_t pid;
        int32_t cpu;
        perf_event_attr perf_event_config;
    };

} // namespace optkit::core::pmu::cpu::perf

#endif