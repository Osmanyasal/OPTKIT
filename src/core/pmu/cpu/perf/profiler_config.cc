
#include "core/pmu/cpu/perf/profiler_config.hh"

namespace optkit::core::pmu::cpu::perf
{
    PerfProfilerConfig::PerfProfilerConfig(bool dump_results_to_file, bool is_reset_after_read, bool is_grouped, int32_t pid, int32_t cpu)
        : dump_results_to_file{dump_results_to_file}, is_reset_after_read{is_reset_after_read}, is_grouped{is_grouped}, pid{pid}, cpu{cpu}
    {
        ::memset(&perf_event_config, 0, sizeof(struct perf_event_attr));
        perf_event_config.type = PERF_TYPE_RAW;
        perf_event_config.size = sizeof(struct perf_event_attr);
        perf_event_config.disabled = 1;
        perf_event_config.inherit = 1;
        perf_event_config.exclude_kernel = 1;
        perf_event_config.exclude_hv = 1;
        setGrouped(this->is_grouped);
    }

    PerfProfilerConfig::PerfProfilerConfig(perf_event_attr perf_event_config, bool dump_results_to_file, bool is_reset_after_read, int32_t pid, int32_t cpu) : dump_results_to_file{dump_results_to_file}, is_reset_after_read{is_reset_after_read}, pid{pid}, cpu{cpu}, perf_event_config{perf_event_config}
    {
        setGrouped(perf_event_config.read_format == (PERF_FORMAT_GROUP | PERF_FORMAT_ID));
    }

    void PerfProfilerConfig::setGrouped(bool is_grouped)
    {
        if (is_grouped)
            perf_event_config.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
        else
            perf_event_config.read_format = 0;

        this->is_grouped = is_grouped;
    }
} // namespace optkit::core::pmu::cpu::perf