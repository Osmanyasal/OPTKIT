#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT && OPTKIT_ENV_CPU_RISCV

#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/pmu/cpu/perf/riscv/events.hh"

namespace optkit::pmu::cpu::perf::riscv
{
    inline PerfProfilerConfig instructions_profiler_config(
        const char *block_name,
        bool is_sampling = false,
        bool is_grouped = false,
        int32_t pid = 0,
        int32_t cpu = -1,
        const char *measurement_type = "cpu_pmu.instructions",
        bool is_reset_after_read = true,
        bool dump_results_to_file = optkit::Query::create_folder,
        bool verbose = !optkit::Query::create_folder,
        bool is_screenshot = false)
    {
        PerfProfilerConfig config(block_name,
                                  make_instructions_attr(),
                                  is_sampling,
                                  is_grouped,
                                  pid,
                                  cpu,
                                  measurement_type,
                                  is_reset_after_read,
                                  dump_results_to_file,
                                  verbose,
                                  is_screenshot);

        config.set_grouped(is_grouped);
        return config;
    }

    inline PerfProfilerConfig llc_load_misses_profiler_config(
        const char *block_name,
        bool is_sampling = false,
        bool is_grouped = false,
        int32_t pid = 0,
        int32_t cpu = -1,
        const char *measurement_type = "cpu_pmu.llc_load_misses",
        bool is_reset_after_read = true,
        bool dump_results_to_file = optkit::Query::create_folder,
        bool verbose = !optkit::Query::create_folder,
        bool is_screenshot = false)
    {
        PerfProfilerConfig config(block_name,
                                  make_llc_load_misses_attr(),
                                  is_sampling,
                                  is_grouped,
                                  pid,
                                  cpu,
                                  measurement_type,
                                  is_reset_after_read,
                                  dump_results_to_file,
                                  verbose,
                                  is_screenshot);

        config.set_grouped(is_grouped);
        return config;
    }

    inline PerfProfilerConfig llc_store_misses_profiler_config(
        const char *block_name,
        bool is_sampling = false,
        bool is_grouped = false,
        int32_t pid = 0,
        int32_t cpu = -1,
        const char *measurement_type = "cpu_pmu.llc_store_misses",
        bool is_reset_after_read = true,
        bool dump_results_to_file = optkit::Query::create_folder,
        bool verbose = !optkit::Query::create_folder,
        bool is_screenshot = false)
    {
        PerfProfilerConfig config(block_name,
                                  make_llc_store_misses_attr(),
                                  is_sampling,
                                  is_grouped,
                                  pid,
                                  cpu,
                                  measurement_type,
                                  is_reset_after_read,
                                  dump_results_to_file,
                                  verbose,
                                  is_screenshot);

        config.set_grouped(is_grouped);
        return config;
    }
} // namespace optkit::pmu::cpu::perf::riscv

#endif