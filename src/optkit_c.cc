#include "optkit_c.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <unistd.h>

#include "optkit.hh"

namespace
{

    thread_local std::string g_last_error;

    void set_error(const std::string &msg)
    {
        g_last_error = msg;
    }

    optkit_status_t set_error_status(optkit_status_t status, const char *msg)
    {
        set_error(msg ? std::string(msg) : std::string("(null)"));
        return status;
    }

    void clear_error()
    {
        g_last_error.clear();
    }

    optkit_status_t require_initialized()
    {
        if (!::optkit_is_initialized())
            return set_error_status(OPTKIT_STATUS_NOT_INITIALIZED, "OPTKIT not initialized. Call optkit_init() first.");
        return OPTKIT_STATUS_OK;
    }

    char *dup_cstr(const std::string &s)
    {
        const size_t n = s.size();
        char *p = static_cast<char *>(std::malloc(n + 1));
        if (!p)
            return nullptr;
        std::memcpy(p, s.data(), n);
        p[n] = '\0';
        return p;
    }

    optkit_status_t set_out_string(char **out_str, const std::string &value)
    {
        if (!out_str)
            return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_str is null");
        *out_str = nullptr;

        char *p = dup_cstr(value);
        if (!p)
            return set_error_status(OPTKIT_STATUS_ERROR, "Out of memory");

        *out_str = p;
        return OPTKIT_STATUS_OK;
    }

    optkit::gpu::GpuVendor to_cpp_vendor(optkit_gpu_vendor_t v)
    {
        switch (v)
        {
        case OPTKIT_GPU_VENDOR_NVIDIA:
            return optkit::gpu::GpuVendor::NVIDIA;
        case OPTKIT_GPU_VENDOR_AMD:
            return optkit::gpu::GpuVendor::AMD;
        case OPTKIT_GPU_VENDOR_INTEL:
            return optkit::gpu::GpuVendor::INTEL;
        case OPTKIT_GPU_VENDOR_ARM_MALI:
            return optkit::gpu::GpuVendor::ARM_MALI;
        case OPTKIT_GPU_VENDOR_QUALCOMM_ADRENO:
            return optkit::gpu::GpuVendor::QUALCOMM_ADRENO;
        case OPTKIT_GPU_VENDOR_IMAGINATION_POWERVR:
            return optkit::gpu::GpuVendor::IMAGINATION_POWERVR;
        default:
            return optkit::gpu::GpuVendor::UNKNOWN;
        }
    }

    optkit::frequency::Unit to_cpp_unit(optkit_frequency_unit_t u)
    {
        switch (u)
        {
        case OPTKIT_FREQUENCY_UNIT_HZ:
            return optkit::frequency::Unit::Hz;
        case OPTKIT_FREQUENCY_UNIT_KHZ:
            return optkit::frequency::Unit::KHz;
        case OPTKIT_FREQUENCY_UNIT_MHZ:
            return optkit::frequency::Unit::MHz;
        case OPTKIT_FREQUENCY_UNIT_GHZ:
            return optkit::frequency::Unit::GHz;
        default:
            return optkit::frequency::Unit::Hz;
        }
    }

    // -----------------------------------------------------------------------------
    // Engine and profiler managers
    // -----------------------------------------------------------------------------

    static std::unique_ptr<optkit::OPTKIT> g_optkit;

    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    static void ensure_pmu_query_initialized()
    {
        static bool initialized = false;
        if (!initialized)
        {
            optkit::pmu::cpu::Query::init();
            initialized = true;
        }
    }

    static void ensure_gpu_query_initialized(optkit::gpu::GpuVendor vendor)
    {
        if (!optkit::gpu::Query::is_init(vendor))
            optkit::gpu::Query::init(vendor);
    }

    static optkit::ProfilerConfig make_profiler_config(const std::string &block_name,
                                                       const char *measurement_type,
                                                       bool is_reset_after_read = true,
                                                       bool is_sampling = true)
    {
        return {block_name.c_str(),
                measurement_type,
                is_reset_after_read,
                is_sampling,
                optkit::Query::create_folder,
                !optkit::Query::create_folder};
    }

    template <typename ProfilerT>
    struct SafePerfEventProfiler
    {
        std::string name_storage;
        std::unique_ptr<ProfilerT> profiler;

        SafePerfEventProfiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config)
        {
            name_storage = config.block_name ? config.block_name : "unknown";
            optkit::pmu::cpu::perf::PerfProfilerConfig safe_config = config;
            safe_config.block_name = name_storage.c_str();
            profiler = make_unique<ProfilerT>(safe_config);
        }

        SafePerfEventProfiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config,
                              const optkit::metrics::MetricBuilder<uint64_t> &mb)
        {
            name_storage = config.block_name ? config.block_name : "unknown";
            optkit::pmu::cpu::perf::PerfProfilerConfig safe_config = config;
            safe_config.block_name = name_storage.c_str();
            profiler = make_unique<ProfilerT>(safe_config, mb);
        }
    };

    template <typename ProfilerT, typename builderT>
    struct SafeProfiler
    {
        std::string name_storage;
        std::unique_ptr<ProfilerT> profiler;

        SafeProfiler(const optkit::ProfilerConfig &config,
                     const optkit::metrics::MetricBuilder<builderT> &mb)
        {
            name_storage = config.block_name ? config.block_name : "unknown";

            optkit::ProfilerConfig safe_config{name_storage.c_str(),
                                               config.measurement_type,
                                               config.is_reset_after_read,
                                               config.is_sampling,
                                               config.dump_results_to_file,
                                               config.verbose};

            profiler = make_unique<ProfilerT>(safe_config, mb);
        }
    };

    using SafePerfProfiler = SafePerfEventProfiler<optkit::pmu::cpu::perf::BlockProfiler>;
    using SafeCallstackProfiler = SafePerfEventProfiler<optkit::callstack::Profiler>;

    using SafeRaplProfiler = SafeProfiler<optkit::energy::rapl::Profiler, double>;
    using SafeNvidiaProfiler = SafeProfiler<optkit::energy::gpu::nvidia::Profiler, double>;
    using SafeAmdProfiler = SafeProfiler<optkit::energy::gpu::amd::Profiler, double>;
    using SafeDiskProfiler = SafeProfiler<optkit::disk::IoDiskProfiler, uint64_t>;
    using SafeHwmonTempProfiler = SafeProfiler<optkit::temperature::hwmon::Profiler, double>;
    using SafeGpuTempProfiler = SafeProfiler<optkit::temperature::gpu::Profiler, std::pair<double, double>>;

    template <typename T>
    class ProfilerManager
    {
    public:
        static void push(std::unique_ptr<T> profiler)
        {
            get_stack().push_back(std::move(profiler));
        }

        static void pop()
        {
            auto &s = get_stack();
            if (!s.empty())
                s.pop_back();
        }

        static void clear()
        {
            get_stack().clear();
        }

        static size_t size()
        {
            return get_stack().size();
        }

    private:
        static std::vector<std::unique_ptr<T>> &get_stack()
        {
            static std::vector<std::unique_ptr<T>> stack;
            return stack;
        }
    };

} // namespace

// -----------------------------------------------------------------------------
// Common utilities
// -----------------------------------------------------------------------------

const char *optkit_last_error_message(void)
{
    return g_last_error.c_str();
}

void optkit_clear_error(void)
{
    clear_error();
}

void optkit_free(void *p)
{
    std::free(p);
}

// -----------------------------------------------------------------------------
// Engine lifecycle
// -----------------------------------------------------------------------------

int optkit_is_initialized(void)
{
    return g_optkit ? 1 : 0;
}

optkit_status_t optkit_init(int create_folder, const char *execution_file)
{
    clear_error();
    try
    {
        if (g_optkit)
            return OPTKIT_STATUS_OK;

        const std::string exec = execution_file ? std::string(execution_file) : std::string("");
        optkit::OPTKIT_CONFIG config(static_cast<bool>(create_folder), exec);
        g_optkit = make_unique<optkit::OPTKIT>(config);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
    catch (...)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, "Unknown error in optkit_init()");
    }
}

optkit_status_t optkit_finalize(void)
{
    clear_error();
    try
    {
        g_optkit.reset();

        ProfilerManager<SafePerfProfiler>::clear();
        ProfilerManager<SafeCallstackProfiler>::clear();
        ProfilerManager<SafeRaplProfiler>::clear();
        ProfilerManager<SafeNvidiaProfiler>::clear();
        ProfilerManager<SafeAmdProfiler>::clear();
        ProfilerManager<SafeDiskProfiler>::clear();
        ProfilerManager<SafeHwmonTempProfiler>::clear();
        ProfilerManager<SafeGpuTempProfiler>::clear();

        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
    catch (...)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, "Unknown error in optkit_finalize()");
    }
}

// -----------------------------------------------------------------------------
// Query: system / CPU
// -----------------------------------------------------------------------------

int16_t optkit_query_system_num_sockets(void)
{
    return optkit::Query::num_sockets;
}

int16_t optkit_query_system_num_logical_cores(void)
{
    return optkit::Query::num_logical_cores;
}

int optkit_query_system_is_root_priv_enabled(void)
{
    return optkit::Query::is_root_priv_enabled ? 1 : 0;
}

optkit_status_t optkit_query_system_paranoid(int32_t *out_paranoid)
{
    clear_error();
    if (!out_paranoid)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_paranoid is null");

    try
    {
        *out_paranoid = optkit::Query::paranoid();
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_system_is_smt_enabled(int *out_enabled)
{
    clear_error();
    if (!out_enabled)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_enabled is null");

    try
    {
        *out_enabled = optkit::Query::is_smt_enabled() ? 1 : 0;
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_system_is_turbo_enabled(int *out_enabled)
{
    clear_error();
    if (!out_enabled)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_enabled is null");

    try
    {
        *out_enabled = optkit::Query::is_turbo_enabled() ? 1 : 0;
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_system_detect_cpu_packages_str(char **out_str)
{
    clear_error();
    try
    {
        const auto &packages = optkit::Query::detect_cpu_packages();
        return set_out_string(out_str, optkit::to_string(packages));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

// -----------------------------------------------------------------------------
// Query: PMU
// -----------------------------------------------------------------------------

optkit_status_t optkit_query_pmu_list_avail_pmus(void)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        optkit::pmu::cpu::Query::list_avail_pmus();
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_list_avail_events(int32_t pmu_id)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        optkit::pmu::cpu::Query::list_avail_events(pmu_id);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_avail_pmu_ids(int32_t *out_ids, size_t capacity, size_t *out_count)
{
    clear_error();
    if (!out_count)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_count is null");

    try
    {
        ensure_pmu_query_initialized();
        const auto ids = optkit::pmu::cpu::Query::avail_pmu_ids();

        *out_count = ids.size();
        if (!out_ids)
            return OPTKIT_STATUS_OK;

        const size_t n = (capacity < ids.size()) ? capacity : ids.size();
        for (size_t i = 0; i < n; ++i)
            out_ids[i] = ids[i];

        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_get_avail_events_str(int32_t pmu_id, char **out_str)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        const auto events = optkit::pmu::cpu::Query::get_avail_events(pmu_id);
        std::string joined;
        for (size_t i = 0; i < events.size(); ++i)
        {
            joined += events[i];
            joined += '\n';
        }
        return set_out_string(out_str, joined);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_pmu_info_str(int32_t pmu_id, char **out_str)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        return set_out_string(out_str, optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::pmu_info(pmu_id)));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_default_pmu_info_str(char **out_str)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        return set_out_string(out_str, optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::default_pmu_info()));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_pmu_event_detail_str(int32_t pmu_id, uint32_t event_code, char **out_str)
{
    clear_error();
    try
    {
        ensure_pmu_query_initialized();
        return set_out_string(out_str, optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::event_detail(pmu_id, event_code)));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

// -----------------------------------------------------------------------------
// Query: RAPL
// -----------------------------------------------------------------------------

int32_t optkit_query_rapl_avail_read_methods(void)
{
    clear_error();
    try
    {
        return optkit::energy::rapl::Query::avail_rapl_read_methods();
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

int optkit_query_rapl_is_perf_avail(void)
{
    clear_error();
    try
    {
        return optkit::energy::rapl::Query::is_rapl_perf_avail() ? 1 : 0;
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

int optkit_query_rapl_is_sysfs_avail(void)
{
    clear_error();
    try
    {
        return optkit::energy::rapl::Query::is_rapl_sysfs_avail() ? 1 : 0;
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

int optkit_query_rapl_is_msr_avail(void)
{
    clear_error();
    try
    {
        return optkit::energy::rapl::Query::is_rapl_msr_avail() ? 1 : 0;
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

optkit_status_t optkit_query_rapl_domain_info_str(char **out_str)
{
    clear_error();
    try
    {
        const auto &domains = optkit::energy::rapl::Query::rapl_domain_info();
        std::string joined;
        for (size_t i = 0; i < domains.size(); ++i)
        {
            joined += optkit::energy::rapl::to_string(domains[i]);
            joined += '\n';
        }
        return set_out_string(out_str, joined);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

// -----------------------------------------------------------------------------
// Query: GPU
// -----------------------------------------------------------------------------

optkit_status_t optkit_query_gpu_init(optkit_gpu_vendor_t vendor)
{
    clear_error();
    try
    {
        const auto v = to_cpp_vendor(vendor);
        (void)optkit::gpu::Query::init(v);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_shutdown(optkit_gpu_vendor_t vendor)
{
    clear_error();
    try
    {
        const auto v = to_cpp_vendor(vendor);
        const bool ok = optkit::gpu::Query::shutdown(v);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "GPU shutdown failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

int optkit_query_gpu_is_init(optkit_gpu_vendor_t vendor)
{
    clear_error();
    try
    {
        return optkit::gpu::Query::is_init(to_cpp_vendor(vendor)) ? 1 : 0;
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

int optkit_query_gpu_is_device_exists(optkit_gpu_vendor_t vendor)
{
    clear_error();
    try
    {
        ensure_gpu_query_initialized(to_cpp_vendor(vendor));
        return optkit::gpu::Query::is_device_exists(to_cpp_vendor(vendor)) ? 1 : 0;
    }
    catch (const std::exception &e)
    {
        set_error(e.what());
        return 0;
    }
}

optkit_status_t optkit_query_gpu_get_device_count(optkit_gpu_vendor_t vendor, uint32_t *out_count)
{
    clear_error();
    if (!out_count)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_count is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        uint32_t count = 0;
        const bool ok = optkit::gpu::Query::get_device_count(v, count);
        *out_count = count;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_device_count failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_device_query_str(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str)
{
    clear_error();
    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        optkit::gpu::GpuDeviceInfo info{};
        const bool ok = optkit::gpu::Query::device_query(v, device_index, info);
        if (!ok)
            return set_error_status(OPTKIT_STATUS_ERROR, "device_query failed");
        return set_out_string(out_str, optkit::gpu::to_string(info));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

#define OPTKIT_GPU_INFO_STR_WRAPPER(NAME, TYPE, CALL_EXPR)                                  \
    optkit_status_t NAME(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str) \
    {                                                                                       \
        clear_error();                                                                      \
        try                                                                                 \
        {                                                                                   \
            const auto v = to_cpp_vendor(vendor);                                           \
            ensure_gpu_query_initialized(v);                                                \
            TYPE info{};                                                                    \
            const bool ok = (CALL_EXPR);                                                    \
            if (!ok)                                                                        \
                return set_error_status(OPTKIT_STATUS_ERROR, "GPU query failed");           \
            return set_out_string(out_str, optkit::gpu::to_string(info));                   \
        }                                                                                   \
        catch (const std::exception &e)                                                     \
        {                                                                                   \
            return set_error_status(OPTKIT_STATUS_ERROR, e.what());                         \
        }                                                                                   \
    }

OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_basic_info_str, optkit::gpu::GpuBasicInfo,
                            optkit::gpu::Query::get_basic_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_version_info_str, optkit::gpu::GpuVersionInfo,
                            optkit::gpu::Query::get_version_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_memory_info_str, optkit::gpu::GpuMemoryInfo,
                            optkit::gpu::Query::get_memory_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_clock_info_str, optkit::gpu::GpuClockInfo,
                            optkit::gpu::Query::get_clock_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_temperature_info_str, optkit::gpu::GpuTemperatureInfo,
                            optkit::gpu::Query::get_temperature_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_compute_info_str, optkit::gpu::GpuComputeInfo,
                            optkit::gpu::Query::get_compute_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_power_info_str, optkit::gpu::GpuPowerInfo,
                            optkit::gpu::Query::get_power_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_utilization_info_str, optkit::gpu::GpuUtilizationInfo,
                            optkit::gpu::Query::get_utilization_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_hardware_info_str, optkit::gpu::GpuHardwareInfo,
                            optkit::gpu::Query::get_hardware_info(v, device_index, info))
OPTKIT_GPU_INFO_STR_WRAPPER(optkit_query_gpu_get_capabilities_info_str, optkit::gpu::GpuCapabilitiesInfo,
                            optkit::gpu::Query::get_capabilities_info(v, device_index, info))

#undef OPTKIT_GPU_INFO_STR_WRAPPER

optkit_status_t optkit_query_gpu_set_clock(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t mem_clk_mhz, uint32_t graphics_clk_mhz)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::set_clock(to_cpp_vendor(vendor), device_index, mem_clk_mhz, graphics_clk_mhz);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_clock failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_reset_clock(optkit_gpu_vendor_t vendor, uint32_t device_index)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::reset_clock(to_cpp_vendor(vendor), device_index);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "reset_clock failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_reset_device(optkit_gpu_vendor_t vendor, uint32_t device_index)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::reset_device(to_cpp_vendor(vendor), device_index);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "reset_device failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_set_persistence_mode(optkit_gpu_vendor_t vendor, uint32_t device_index, int enable)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::set_persistence_mode(to_cpp_vendor(vendor), device_index, static_cast<bool>(enable));
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_persistence_mode failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_set_fan_speed(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t fan_speed_percent)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::set_fan_speed(to_cpp_vendor(vendor), device_index, std::to_string(fan_speed_percent));
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_fan_speed failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_reset_fan_speed(optkit_gpu_vendor_t vendor, uint32_t device_index)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::reset_fan_speed(to_cpp_vendor(vendor), device_index);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "reset_fan_speed failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_set_power_limit(optkit_gpu_vendor_t vendor, uint32_t device_index, double power_limit_watts)
{
    clear_error();
    try
    {
        const bool ok = optkit::gpu::Query::set_power_limit(to_cpp_vendor(vendor), device_index, power_limit_watts);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_power_limit failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_fan_count(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_count)
{
    clear_error();
    if (!out_count)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_count is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        uint32_t count = 0;
        const bool ok = optkit::gpu::Query::get_fan_count(v, device_index, count);
        *out_count = count;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_fan_count failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_warp_size(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_warp_size)
{
    clear_error();
    if (!out_warp_size)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_warp_size is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        uint32_t warp = 0;
        const bool ok = optkit::gpu::Query::get_warp_size(v, device_index, warp);
        *out_warp_size = warp;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_warp_size failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_driver_version(optkit_gpu_vendor_t vendor, double *out_version)
{
    clear_error();
    if (!out_version)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_version is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        double ver = 0.0;
        const bool ok = optkit::gpu::Query::get_driver_version(v, ver);
        *out_version = ver;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_driver_version failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_library_version(optkit_gpu_vendor_t vendor, char **out_str)
{
    clear_error();
    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        std::string ver;
        const bool ok = optkit::gpu::Query::get_library_version(v, ver);
        if (!ok)
            return set_error_status(OPTKIT_STATUS_ERROR, "get_library_version failed");
        return set_out_string(out_str, ver);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_device_power(optkit_gpu_vendor_t vendor, uint32_t device_index, double *out_watts)
{
    clear_error();
    if (!out_watts)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_watts is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        double power = 0.0;
        const bool ok = optkit::gpu::Query::get_device_power(v, device_index, power);
        *out_watts = power;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_device_power failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_architecture(optkit_gpu_vendor_t vendor, uint32_t device_index, uint32_t *out_arch)
{
    clear_error();
    if (!out_arch)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_arch is null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        uint32_t arch = 0;
        const bool ok = optkit::gpu::Query::get_architecture(v, device_index, arch);
        *out_arch = arch;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_architecture failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_device_power_limits(optkit_gpu_vendor_t vendor, uint32_t device_index,
                                                         double *out_limit_watts, double *out_default_power, double *out_min_limit_watts,
                                                         double *out_max_limit_watts, int *out_is_configurable)
{
    clear_error();
    if (!out_limit_watts || !out_default_power || !out_min_limit_watts || !out_max_limit_watts || !out_is_configurable)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "one or more output pointers are null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        double limit = 0.0, defp = 0.0, minp = 0.0, maxp = 0.0;
        bool is_cfg = false;
        const bool ok = optkit::gpu::Query::get_device_power_limits(v, device_index, limit, defp, minp, maxp, is_cfg);
        *out_limit_watts = limit;
        *out_default_power = defp;
        *out_min_limit_watts = minp;
        *out_max_limit_watts = maxp;
        *out_is_configurable = is_cfg ? 1 : 0;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_device_power_limits failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_device_temperature(optkit_gpu_vendor_t vendor, uint32_t device_index, double *out_device_temp_c, double *out_mem_temp_c)
{
    clear_error();
    if (!out_device_temp_c || !out_mem_temp_c)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "one or more output pointers are null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        double dev = 0.0, mem = 0.0;
        const bool ok = optkit::gpu::Query::get_device_temperature(v, device_index, dev, mem);
        *out_device_temp_c = dev;
        *out_mem_temp_c = mem;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_device_temperature failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_device_name(optkit_gpu_vendor_t vendor, uint32_t device_index, char **out_str)
{
    clear_error();
    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        std::string name;
        const bool ok = optkit::gpu::Query::get_device_name(v, device_index, name);
        if (!ok)
            return set_error_status(OPTKIT_STATUS_ERROR, "get_device_name failed");
        return set_out_string(out_str, name);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_query_gpu_get_device_temperature_thresholds(optkit_gpu_vendor_t vendor, uint32_t device_index,
                                                                   double *out_max_gpu_temp_c, double *out_max_mem_temp_c,
                                                                   double *out_min_gpu_temp_c, double *out_min_mem_temp_c)
{
    clear_error();
    if (!out_max_gpu_temp_c || !out_max_mem_temp_c || !out_min_gpu_temp_c || !out_min_mem_temp_c)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "one or more output pointers are null");

    try
    {
        const auto v = to_cpp_vendor(vendor);
        ensure_gpu_query_initialized(v);
        double max_gpu = 0.0, max_mem = 0.0, min_gpu = 0.0, min_mem = 0.0;
        const bool ok = optkit::gpu::Query::get_device_temperature_thresholds(v, device_index, max_gpu, max_mem, min_gpu, min_mem);
        *out_max_gpu_temp_c = max_gpu;
        *out_max_mem_temp_c = max_mem;
        *out_min_gpu_temp_c = min_gpu;
        *out_min_mem_temp_c = min_mem;
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "get_device_temperature_thresholds failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

// -----------------------------------------------------------------------------
// Profilers
// -----------------------------------------------------------------------------

optkit_status_t optkit_perf_start(const char *block_name,
                                  const char *const *metrics, size_t metrics_count,
                                  const char *const *events, size_t events_count)
{
    clear_error();

    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::pmu::cpu::perf::PerfProfilerConfig default_config(block_name, true /*is_sampling*/);
        optkit::metrics::MetricBuilder<uint64_t> mb;

        for (size_t i = 0; i < metrics_count; ++i)
        {
            if (!metrics || !metrics[i])
                continue;
            mb.add(optkit::metrics::performance::cpu_metrics::get_metric(std::string(metrics[i])));
        }

        for (size_t i = 0; i < events_count; ++i)
        {
            if (!events || !events[i])
                continue;
            const std::string ev(events[i]);
            mb.add(ev, optkit::metrics::performance::cpu_mapper::get(ev));
        }

        auto safe_profiler = make_unique<SafePerfProfiler>(default_config, mb);
        ProfilerManager<SafePerfProfiler>::push(std::move(safe_profiler));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_perf_stop(void)
{
    clear_error();
    ProfilerManager<SafePerfProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_energy_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig cpu_cfg = make_profiler_config(block_name, "cpu_energy");
        auto cpu_prof = make_unique<SafeRaplProfiler>(cpu_cfg, optkit::metrics::energy::cpu_metrics::all_metrics());
        ProfilerManager<SafeRaplProfiler>::push(std::move(cpu_prof));

        const auto gpu_mb = optkit::metrics::energy::gpu_metrics::all_metrics();
        try
        {
            optkit::ProfilerConfig nvidia_cfg = make_profiler_config(block_name, "nvidia_gpu_energy");
            auto nvidia_prof = make_unique<SafeNvidiaProfiler>(nvidia_cfg, gpu_mb);
            ProfilerManager<SafeNvidiaProfiler>::push(std::move(nvidia_prof));
        }
        catch (...)
        {
            // optional
        }

        try
        {
            optkit::ProfilerConfig amd_cfg = make_profiler_config(block_name, "amd_gpu_energy");
            auto amd_prof = make_unique<SafeAmdProfiler>(amd_cfg, gpu_mb);
            ProfilerManager<SafeAmdProfiler>::push(std::move(amd_prof));
        }
        catch (...)
        {
            // optional
        }

        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_energy_stop(void)
{
    clear_error();
    ProfilerManager<SafeRaplProfiler>::pop();
    ProfilerManager<SafeNvidiaProfiler>::pop();
    ProfilerManager<SafeAmdProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_energy_cpu_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "cpu_energy");
        auto prof = make_unique<SafeRaplProfiler>(cfg, optkit::metrics::energy::cpu_metrics::all_metrics());
        ProfilerManager<SafeRaplProfiler>::push(std::move(prof));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_energy_cpu_stop(void)
{
    clear_error();
    ProfilerManager<SafeRaplProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_energy_gpu_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        const auto gpu_mb = optkit::metrics::energy::gpu_metrics::all_metrics();
        try
        {
            optkit::ProfilerConfig nvidia_cfg = make_profiler_config(block_name, "nvidia_gpu_energy");
            auto nvidia_prof = make_unique<SafeNvidiaProfiler>(nvidia_cfg, gpu_mb);
            ProfilerManager<SafeNvidiaProfiler>::push(std::move(nvidia_prof));
        }
        catch (...)
        {
            // optional
        }

        try
        {
            optkit::ProfilerConfig amd_cfg = make_profiler_config(block_name, "amd_gpu_energy");
            auto amd_prof = make_unique<SafeAmdProfiler>(amd_cfg, gpu_mb);
            ProfilerManager<SafeAmdProfiler>::push(std::move(amd_prof));
        }
        catch (...)
        {
            // optional
        }

        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_energy_gpu_stop(void)
{
    clear_error();
    ProfilerManager<SafeAmdProfiler>::pop();
    ProfilerManager<SafeNvidiaProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_callstack_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::pmu::cpu::perf::PerfProfilerConfig cfg{block_name, true, false, ::getpid(), -1, "callstack"};
        auto prof = make_unique<SafeCallstackProfiler>(cfg);
        ProfilerManager<SafeCallstackProfiler>::push(std::move(prof));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_callstack_stop(void)
{
    clear_error();
    ProfilerManager<SafeCallstackProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_disk_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "disk_io");
        auto prof = make_unique<SafeDiskProfiler>(cfg, optkit::metrics::disk::core_metrics::all_metrics());
        ProfilerManager<SafeDiskProfiler>::push(std::move(prof));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_disk_stop(void)
{
    clear_error();
    ProfilerManager<SafeDiskProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_temperature_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig hwmon_cfg = make_profiler_config(block_name, "hwmon_temperature", true, false);
        auto hwmon_prof = make_unique<SafeHwmonTempProfiler>(hwmon_cfg, optkit::metrics::MetricBuilder<double>{});
        ProfilerManager<SafeHwmonTempProfiler>::push(std::move(hwmon_prof));

        try
        {
            optkit::ProfilerConfig gpu_cfg = make_profiler_config(block_name, "gpu_temperature", true, false);
            auto gpu_prof = make_unique<SafeGpuTempProfiler>(gpu_cfg, optkit::metrics::MetricBuilder<std::pair<double, double>>{});
            ProfilerManager<SafeGpuTempProfiler>::push(std::move(gpu_prof));
        }
        catch (...)
        {
            // optional
        }

        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_temperature_stop(void)
{
    clear_error();
    ProfilerManager<SafeHwmonTempProfiler>::pop();
    ProfilerManager<SafeGpuTempProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_temperature_hwmon_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "hwmon_temperature", true, false);
        auto prof = make_unique<SafeHwmonTempProfiler>(cfg, optkit::metrics::MetricBuilder<double>{});
        ProfilerManager<SafeHwmonTempProfiler>::push(std::move(prof));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_temperature_hwmon_stop(void)
{
    clear_error();
    ProfilerManager<SafeHwmonTempProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_temperature_gpu_start(const char *block_name)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!block_name)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "block_name is null");

    try
    {
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "gpu_temperature", true, false);
        auto prof = make_unique<SafeGpuTempProfiler>(cfg, optkit::metrics::MetricBuilder<std::pair<double, double>>{});
        ProfilerManager<SafeGpuTempProfiler>::push(std::move(prof));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_temperature_gpu_stop(void)
{
    clear_error();
    ProfilerManager<SafeGpuTempProfiler>::pop();
    return OPTKIT_STATUS_OK;
}

// -----------------------------------------------------------------------------
// Frequency
// -----------------------------------------------------------------------------

optkit_status_t optkit_frequency_convert(const char *freq_str, optkit_frequency_unit_t target_unit, double *out_value)
{
    clear_error();
    if (!freq_str || !out_value)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "freq_str or out_value is null");

    try
    {
        *out_value = optkit::frequency::convert_frequency_with_unit(std::string(freq_str), to_cpp_unit(target_unit));
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_set_core_frequency(int64_t freq_khz, int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_core_frequency failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_set_core_frequency_core(int64_t freq_khz, int16_t cpu, int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, cpu, socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_core_frequency_core failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_set_core_frequency_range(int64_t freq_khz, int16_t cpu_start, int16_t cpu_end, int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, cpu_start, cpu_end, socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_core_frequency_range failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_get_core_frequency(int16_t cpu, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Frequency::get_core_frequency(cpu);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

static optkit_status_t fill_int64_vector(const std::vector<int64_t> &vals, int64_t *out, size_t capacity, size_t *out_count)
{
    if (!out_count)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_count is null");

    *out_count = vals.size();
    if (!out)
        return OPTKIT_STATUS_OK;

    const size_t n = (capacity < vals.size()) ? capacity : vals.size();
    for (size_t i = 0; i < n; ++i)
        out[i] = vals[i];

    return OPTKIT_STATUS_OK;
}

optkit_status_t optkit_frequency_cpu_get_core_frequencies(int16_t socket, int64_t *out_freqs_khz, size_t capacity, size_t *out_count)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const auto vals = optkit::frequency::cpu::Frequency::get_core_frequencies(socket);
        return fill_int64_vector(vals, out_freqs_khz, capacity, out_count);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_reset_core_frequency(int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::reset_core_frequency(socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "reset_core_frequency failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_get_uncore_frequency(int16_t socket, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Frequency::get_uncore_frequency(socket);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_set_uncore_frequency(int64_t freq_khz, int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::set_uncore_frequency(freq_khz, socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_uncore_frequency failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_reset_uncore_frequency(int16_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const bool ok = optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "reset_uncore_frequency failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_get_uncore_min_max(int16_t socket, int64_t *out_min_khz, int64_t *out_max_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_min_khz || !out_max_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_min_khz or out_max_khz is null");

    try
    {
        const auto mm = optkit::frequency::cpu::Frequency::get_uncore_min_max(socket);
        *out_min_khz = mm.first;
        *out_max_khz = mm.second;
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_get_scaling_available_uncore_frequencies(int16_t socket, int64_t step_khz,
                                                                              int64_t *out_freqs_khz, size_t capacity, size_t *out_count)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const auto vals = optkit::frequency::cpu::Frequency::get_scaling_available_uncore_frequencies(socket, step_khz);
        return fill_int64_vector(vals, out_freqs_khz, capacity, out_count);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_available_core_frequencies(int32_t core, int64_t step_khz,
                                                                      int64_t *out_freqs_khz, size_t capacity, size_t *out_count)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const auto vals = optkit::frequency::cpu::Query::get_scaling_available_core_frequencies(core, step_khz);
        return fill_int64_vector(vals, out_freqs_khz, capacity, out_count);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_available_governors_str(int32_t core, char **out_str)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        const auto vals = optkit::frequency::cpu::Query::get_available_governors(core);
        std::string joined;
        for (size_t i = 0; i < vals.size(); ++i)
        {
            joined += vals[i];
            joined += '\n';
        }
        return set_out_string(out_str, joined);
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_governor(int32_t core, char **out_str)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        return set_out_string(out_str, optkit::frequency::cpu::Query::get_scaling_governor(core));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_set_governor(const char *governor, int32_t socket)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!governor)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "governor is null");

    try
    {
        const bool ok = optkit::frequency::cpu::Query::set_scaling_governor(std::string(governor), socket);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_governor failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_cpuinfo_max_freq(int32_t core, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Query::get_cpuinfo_max_freq(core);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_cpuinfo_min_freq(int32_t core, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Query::get_cpuinfo_min_freq(core);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_bios_limit(int32_t core, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Query::get_bios_limit(core);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_scaling_driver(int32_t core, char **out_str)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;

    try
    {
        return set_out_string(out_str, optkit::frequency::cpu::Query::get_scaling_driver(core));
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_set_governor_percore(const char *governor, int32_t core)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!governor)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "governor is null");

    try
    {
        const bool ok = optkit::frequency::cpu::Query::set_scaling_governor_percore(std::string(governor), core);
        return ok ? OPTKIT_STATUS_OK : set_error_status(OPTKIT_STATUS_ERROR, "set_governor_percore failed");
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_scaling_max_limit(int32_t core, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Query::get_scaling_max_limit(core);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}

optkit_status_t optkit_frequency_cpu_query_get_scaling_min_limit(int32_t core, int64_t *out_freq_khz)
{
    clear_error();
    const auto init_status = require_initialized();
    if (init_status != OPTKIT_STATUS_OK)
        return init_status;
    if (!out_freq_khz)
        return set_error_status(OPTKIT_STATUS_INVALID_ARGUMENT, "out_freq_khz is null");

    try
    {
        *out_freq_khz = optkit::frequency::cpu::Query::get_scaling_min_limit(core);
        return OPTKIT_STATUS_OK;
    }
    catch (const std::exception &e)
    {
        return set_error_status(OPTKIT_STATUS_ERROR, e.what());
    }
}
