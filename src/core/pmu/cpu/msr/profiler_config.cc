#include "core/pmu/cpu/msr/profiler_config.hh"
#if OPTKIT_ENV_LIB_MSR_SAFE
namespace optkit::pmu::cpu::msr
{

    MSRProfilerConfig::MSRProfilerConfig(bool dump_results_to_file, bool is_reset_after_read, bool is_grouped, int32_t pid, int32_t cpu)
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

    void MSRProfilerConfig::setGrouped(bool is_grouped)
    {
        if (is_grouped)
            perf_event_config.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
        else
            perf_event_config.read_format = 0;
        this->is_grouped = is_grouped;
    }
} // namespace optkit::pmu::cpu::msr

#endif