#pragma once

#include <cstring>
#include <cstdint>
#include <linux/perf_event.h>          // for pmu monitoring

namespace optkit::core::pmu::cpu::perf
{
    /**
     * @brief perf_event_open profiler config.
     * @see perf_event.h for more detail.
     *
     */
    struct PerfProfilerConfig
    {
        /**
         * @brief Construct a new Profiler Config object
         *
         * @param is_reset_after_read Reset the counter after any read operations or not.
         * @param is_grouped indicates all events in the BlockProfiler should be groupped or not @see perf_event_open man page
         * @param pid
         * @param cpu
         *
         *  pid == 0 and cpu == -1
         *        This measures the calling process/thread on any CPU.
         *
         *  pid == 0 and cpu >= 0
         *         This measures the calling process/thread only when running
         *         on the specified CPU.
         *
         * pid > 0 and cpu == -1
         *         This measures the specified process/thread on any CPU.
         *
         *  pid > 0 and cpu >= 0
         *         This measures the specified process/thread only when
         *         running on the specified CPU.
         *
         *  pid == -1 and cpu >= 0
         *         This measures all processes/threads on the specified CPU.
         *         This requires CAP_PERFMON (since Linux 5.8) or
         *         CAP_SYS_ADMIN capability or a
         *         /proc/sys/kernel/perf_event_paranoid value of less than 1.
         *
         *  pid == -1 and cpu == -1
         *         This setting is invalid and will return an error.
         *
         */
        PerfProfilerConfig(bool dump_results_to_file = true, bool is_reset_after_read = true, bool is_grouped = false, int32_t pid = 0, int32_t cpu = -1);
        PerfProfilerConfig(perf_event_attr perf_event_config, bool dump_results_to_file = true, bool is_reset_after_read = true, int32_t pid = 0, int32_t cpu = -1);
        void setGrouped(bool is_grouped);

        bool dump_results_to_file;
        bool is_reset_after_read;
        bool is_grouped;
        int32_t pid;
        int32_t cpu;
        perf_event_attr perf_event_config;
    };

} // namespace optkit::core::pmu::cpu::perf
