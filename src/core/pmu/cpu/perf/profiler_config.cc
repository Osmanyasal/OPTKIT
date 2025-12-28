#include "core/pmu/cpu/perf/profiler_config.hh"

#if OPTKIT_ENV_LIB_PERF_EVENT
namespace optkit::pmu::cpu::perf
{

    PerfProfilerConfig::PerfProfilerConfig(
        const char *block_name,
        bool is_sampling,
        bool is_grouped,
        int32_t pid,
        int32_t cpu,
        const char *measurement_type,
        bool is_reset_after_read,
        bool dump_results_to_file,
        bool verbose)
        : optkit::ProfilerConfig(block_name, measurement_type, is_reset_after_read, is_sampling, dump_results_to_file, verbose),
          is_grouped(is_grouped),
          pid(pid),
          cpu(cpu)
    {
        ::memset(&this->perf_event_config, 0, sizeof(struct perf_event_attr));
        this->perf_event_config.type = PERF_TYPE_RAW;
        this->perf_event_config.size = sizeof(struct perf_event_attr);
        this->perf_event_config.disabled = 1;
        this->perf_event_config.inherit = 1;
        this->perf_event_config.exclude_kernel = 1;
        this->perf_event_config.exclude_hv = 1;
        set_grouped(this->is_grouped);
    }

    PerfProfilerConfig::PerfProfilerConfig(
        const char *block_name,
        const perf_event_attr &perf_event_config,
        bool is_sampling,
        bool is_grouped,
        int32_t pid,
        int32_t cpu,
        const char *measurement_type,
        bool is_reset_after_read,
        bool dump_results_to_file,
        bool verbose)
        : optkit::ProfilerConfig(block_name, measurement_type, is_reset_after_read, is_sampling, dump_results_to_file, verbose),
          is_grouped(is_grouped),
          pid(pid),
          cpu(cpu),
          perf_event_config(perf_event_config)
    {
        set_grouped(perf_event_config.read_format == (PERF_FORMAT_GROUP | PERF_FORMAT_ID));
    }
} // namespace optkit::pmu::cpu::perf

#endif