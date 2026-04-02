#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <memory>
#include <stack>
#include <unistd.h>

#include "optkit.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/energy/cpu/pdu/profiler.hh"
#include "core/energy/cpu/rapl/profiler.hh"
#include "core/energy/cpu/hwmon/profiler.hh"
#include "core/energy/gpu/nvidia/profiler.hh"
#include "core/energy/gpu/amd/profiler.hh"
#include "core/temperature/hwmon/profiler.hh"
#include "core/temperature/gpu/profiler.hh"
#include "core/metrics/temperature/module.hh"
#include "core/frequency/cpu/frequency.hh"
#include "core/frequency/cpu/query.hh"
#include "core/frequency/utils.hh"
#include "core/query.hh"
#include "core/pmu/cpu/query_pmu.hh"
#include "core/energy/cpu/rapl/query.hh"
#include "core/gpu_query.hh"
#include "utils/gpu.hh"
#include "core/energy/cpu/rapl/rapl.hh"
#include "utils/metric_builder.hh"

namespace py = pybind11;

// -----------------------------------------------------------------------------
// Manager Logic (The "Static Holder" Pattern)
// -----------------------------------------------------------------------------

// Simple make_unique implementation for C++11
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Global OPTKIT Instance
static std::unique_ptr<optkit::OPTKIT> global_optkit_instance;

static void require_initialized()
{
    if (!global_optkit_instance)
    {
        throw py::value_error("OPTKIT not initialized. Call optkit_py.init() first.");
    }
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
    {
        optkit::gpu::Query::init(vendor);
    }
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

/**
 * @brief Wrapper to ensure BlockProfiler's config name strings persist.
 */
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

using SafePduProfiler = SafeProfiler<optkit::energy::pdu::Profiler, double>;
using SafeRaplProfiler = SafeProfiler<optkit::energy::rapl::Profiler, double>;
using SafeCpuHwmonProfiler = SafeProfiler<optkit::energy::hwmon::Profiler, double>;
using SafeNvidiaProfiler = SafeProfiler<optkit::energy::gpu::nvidia::Profiler, double>;
using SafeAmdProfiler = SafeProfiler<optkit::energy::gpu::amd::Profiler, double>;
using SafeDiskProfiler = SafeProfiler<optkit::disk::IoDiskProfiler, uint64_t>;
using SafeHwmonTempProfiler = SafeProfiler<optkit::temperature::hwmon::Profiler, double>;
using SafeGpuTempProfiler = SafeProfiler<optkit::temperature::gpu::Profiler, std::pair<double, double>>;

static bool should_use_pdu_cpu_energy()
{
#if OPTKIT_ENV_LIB_NET_SNMP
    return optkit::energy::pdu::Query::is_pdu_snmp_avail();
#else
    return false;
#endif
}

/**
 * @brief Manages a stack of RAII profilers for a specific type T.
 *
 * This allows "start()" to push a new instance, and "stop()" to pop (destroy) it.
 */
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
        {
            s.pop_back(); // Destructor fires (RAII stop)
        }
        else
        {
            py::print("Warning: stop() called with no active profiler running.");
        }
    }

    static void clear()
    {
        get_stack().clear();
    }

    static T *top()
    {
        auto &s = get_stack();
        return s.empty() ? nullptr : s.back().get();
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

template class ProfilerManager<SafePerfProfiler>;
template class ProfilerManager<SafeCallstackProfiler>;
template class ProfilerManager<SafePduProfiler>;
template class ProfilerManager<SafeRaplProfiler>;
template class ProfilerManager<SafeCpuHwmonProfiler>;
template class ProfilerManager<SafeNvidiaProfiler>;
template class ProfilerManager<SafeAmdProfiler>;
template class ProfilerManager<SafeDiskProfiler>;
template class ProfilerManager<SafeHwmonTempProfiler>;
template class ProfilerManager<SafeGpuTempProfiler>;

// -----------------------------------------------------------------------------
// Python Wrapper Module
// -----------------------------------------------------------------------------

PYBIND11_MODULE(optkit_py, m)
{
    m.doc() = "Python bindings for OPTKIT with start/stop semantics";

    // -------------------------------------------------------------------------
    // Top-Level Initialization
    // -------------------------------------------------------------------------

    m.def("init", [](bool create_folder, std::string execution_file)
          {
        if (!global_optkit_instance) {
             optkit::OPTKIT_CONFIG config(create_folder, execution_file);
             global_optkit_instance = make_unique<optkit::OPTKIT>(config);
        } else {
             py::print("OPTKIT already initialized.");
        } }, "Initialize OPTKIT engine", py::arg("create_folder") = true, py::arg("execution_file") = "");

    m.def("finalize", []()
          {
        if (global_optkit_instance) {
            global_optkit_instance.reset(); // Destroy
            
            if(ProfilerManager<SafePerfProfiler>::size() > 0) {
                py::print("Warning: There are still active performance profilers. They will be stopped.");
                ProfilerManager<SafePerfProfiler>::clear();
            }
            if (ProfilerManager<SafeCallstackProfiler>::size() > 0)
            {
                py::print("Warning: There are still active callstack profilers. They will be stopped.");
                ProfilerManager<SafeCallstackProfiler>::clear();
            }
            if(ProfilerManager<SafeRaplProfiler>::size() > 0) {
                py::print("Warning: There are still active RAPL profilers. They will be stopped.");
                ProfilerManager<SafeRaplProfiler>::clear();
            }
            if(ProfilerManager<SafePduProfiler>::size() > 0) {
                py::print("Warning: There are still active PDU profilers. They will be stopped.");
                ProfilerManager<SafePduProfiler>::clear();
            }
            if(ProfilerManager<SafeCpuHwmonProfiler>::size() > 0) {
                py::print("Warning: There are still active HWMON CPU energy profilers. They will be stopped.");
                ProfilerManager<SafeCpuHwmonProfiler>::clear();
            }
            if(ProfilerManager<SafeNvidiaProfiler>::size() > 0) {
                py::print("Warning: There are still active NVIDIA GPU profilers. They will be stopped.");
                ProfilerManager<SafeNvidiaProfiler>::clear();
            }
            if(ProfilerManager<SafeAmdProfiler>::size() > 0) {
                py::print("Warning: There are still active AMD GPU profilers. They will be stopped.");
                ProfilerManager<SafeAmdProfiler>::clear();
            }
            if(ProfilerManager<SafeDiskProfiler>::size() > 0) {
                py::print("Warning: There are still active Disk I/O profilers. They will be stopped.");
                ProfilerManager<SafeDiskProfiler>::clear();
            }
            if (ProfilerManager<SafeHwmonTempProfiler>::size() > 0)
            {
                py::print("Warning: There are still active HWMON temperature profilers. They will be stopped.");
                ProfilerManager<SafeHwmonTempProfiler>::clear();
            }
            if (ProfilerManager<SafeGpuTempProfiler>::size() > 0)
            {
                py::print("Warning: There are still active GPU temperature profilers. They will be stopped.");
                ProfilerManager<SafeGpuTempProfiler>::clear();
            }
        } else {
            py::print("OPTKIT not initialized.");
            
        } }, "Finalize/Destroy OPTKIT engine");

    // -------------------------------------------------------------------------
    // Query Modules (System / PMU / RAPL / GPU)
    // -------------------------------------------------------------------------

    auto query = m.def_submodule("query");

    // System/CPU queries (optkit::Query)
    {
        auto system = query.def_submodule("system");
        py::class_<optkit::Query, std::unique_ptr<optkit::Query, py::nodelete>>(system, "Query")
            .def_static("detect_cpu_packages", &optkit::Query::detect_cpu_packages)
            .def_static("is_smt_enabled", &optkit::Query::is_smt_enabled)
            .def_static("is_turbo_enabled", &optkit::Query::is_turbo_enabled)
            .def_static("paranoid", &optkit::Query::paranoid)
            .def_property_readonly_static(
                "num_sockets",
                [](py::object)
                { return optkit::Query::num_sockets; })
            .def_property_readonly_static(
                "num_logical_cores",
                [](py::object)
                { return optkit::Query::num_logical_cores; })
            .def_property_readonly_static(
                "is_root_priv_enabled",
                [](py::object)
                { return optkit::Query::is_root_priv_enabled; });
    }

    // PMU queries (libpfm4) - exposed with string helpers to avoid binding pfm_* structs
    {
        auto pmu = query.def_submodule("pmu");
        py::class_<optkit::pmu::cpu::Query, std::unique_ptr<optkit::pmu::cpu::Query, py::nodelete>>(pmu, "Query")
            .def_static(
                "list_avail_events",
                [](int32_t pmu_id)
                {
                    ensure_pmu_query_initialized();
                    optkit::pmu::cpu::Query::list_avail_events(pmu_id);
                },
                py::arg("pmu_id"))
            .def_static(
                "get_avail_events",
                [](int32_t pmu_id)
                {
                    ensure_pmu_query_initialized();
                    return optkit::pmu::cpu::Query::get_avail_events(pmu_id);
                },
                py::arg("pmu_id"))
            .def_static(
                "list_avail_pmus",
                []()
                {
                    ensure_pmu_query_initialized();
                    optkit::pmu::cpu::Query::list_avail_pmus();
                })
            .def_static(
                "avail_pmu_ids",
                []()
                {
                    ensure_pmu_query_initialized();
                    return optkit::pmu::cpu::Query::avail_pmu_ids();
                })
            .def_static(
                "pmu_info_str",
                [](int32_t pmu_id)
                {
                    ensure_pmu_query_initialized();
                    return optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::pmu_info(pmu_id));
                },
                py::arg("pmu_id"))
            .def_static(
                "default_pmu_info_str",
                []()
                {
                    ensure_pmu_query_initialized();
                    return optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::default_pmu_info());
                })
            .def_static(
                "event_detail_str",
                [](int32_t pmu_id, uint32_t event_code)
                {
                    ensure_pmu_query_initialized();
                    return optkit::pmu::cpu::to_string(optkit::pmu::cpu::Query::event_detail(pmu_id, event_code));
                },
                py::arg("pmu_id"), py::arg("event_code"));
    }

    // RAPL queries
    {
        auto rapl = query.def_submodule("rapl");
        py::enum_<optkit::energy::rapl::RaplDomain>(rapl, "RaplDomain")
            .value("PP0", optkit::energy::rapl::RaplDomain::PP0)
            .value("PP1", optkit::energy::rapl::RaplDomain::PP1)
            .value("PACKAGE", optkit::energy::rapl::RaplDomain::PACKAGE)
            .value("PSYS", optkit::energy::rapl::RaplDomain::PSYS)
            .value("DRAM", optkit::energy::rapl::RaplDomain::DRAM)
            .value("ALL", optkit::energy::rapl::RaplDomain::ALL);

        py::enum_<optkit::energy::rapl::RaplReadMethods>(rapl, "RaplReadMethods")
            .value("PERF", optkit::energy::rapl::RaplReadMethods::PERF)
            .value("MSR", optkit::energy::rapl::RaplReadMethods::MSR)
            .value("SYSFS", optkit::energy::rapl::RaplReadMethods::SYSFS);

        py::class_<optkit::energy::rapl::RaplDomainInfo>(rapl, "RaplDomainInfo")
            .def(py::init<>())
            .def_readwrite("domain", &optkit::energy::rapl::RaplDomainInfo::domain)
            .def_readwrite("event", &optkit::energy::rapl::RaplDomainInfo::event)
            .def_readwrite("config", &optkit::energy::rapl::RaplDomainInfo::config)
            .def_readwrite("scale", &optkit::energy::rapl::RaplDomainInfo::scale)
            .def_readwrite("units", &optkit::energy::rapl::RaplDomainInfo::units);

        py::class_<optkit::energy::rapl::Query, std::unique_ptr<optkit::energy::rapl::Query, py::nodelete>>(rapl, "Query")
            .def_static("avail_rapl_read_methods", &optkit::energy::rapl::Query::avail_rapl_read_methods)
            .def_static("is_rapl_perf_avail", &optkit::energy::rapl::Query::is_rapl_perf_avail)
            .def_static("is_rapl_sysfs_avail", &optkit::energy::rapl::Query::is_rapl_sysfs_avail)
            .def_static("is_rapl_msr_avail", &optkit::energy::rapl::Query::is_rapl_msr_avail)
            .def_static("rapl_domain_info", &optkit::energy::rapl::Query::rapl_domain_info);
    }

    // GPU queries
    {
        auto gpu_query = query.def_submodule("gpu");

        py::enum_<optkit::gpu::GpuVendor>(gpu_query, "GpuVendor")
            .value("NVIDIA", optkit::gpu::GpuVendor::NVIDIA)
            .value("AMD", optkit::gpu::GpuVendor::AMD)
            .value("INTEL", optkit::gpu::GpuVendor::INTEL)
            .value("ARM_MALI", optkit::gpu::GpuVendor::ARM_MALI)
            .value("QUALCOMM_ADRENO", optkit::gpu::GpuVendor::QUALCOMM_ADRENO)
            .value("IMAGINATION_POWERVR", optkit::gpu::GpuVendor::IMAGINATION_POWERVR)
            .value("UNKNOWN", optkit::gpu::GpuVendor::UNKNOWN);

        py::class_<optkit::gpu::GpuBasicInfo>(gpu_query, "GpuBasicInfo")
            .def(py::init<>())
            .def_readwrite("id", &optkit::gpu::GpuBasicInfo::id)
            .def_readwrite("device_name", &optkit::gpu::GpuBasicInfo::device_name)
            .def_readwrite("vendor", &optkit::gpu::GpuBasicInfo::vendor)
            .def_readwrite("architecture", &optkit::gpu::GpuBasicInfo::architecture)
            .def_readwrite("vendor_string", &optkit::gpu::GpuBasicInfo::vendor_string)
            .def_readwrite("is_integrated", &optkit::gpu::GpuBasicInfo::is_integrated);

        py::class_<optkit::gpu::GpuVersionInfo>(gpu_query, "GpuVersionInfo")
            .def(py::init<>())
            .def_readwrite("driver_major_minor", &optkit::gpu::GpuVersionInfo::driver_major_minor)
            .def_readwrite("driver_version_string", &optkit::gpu::GpuVersionInfo::driver_version_string)
            .def_readwrite("library_version_string", &optkit::gpu::GpuVersionInfo::library_version_string);

        py::class_<optkit::gpu::GpuComputeInfo>(gpu_query, "GpuComputeInfo")
            .def(py::init<>())
            .def_readwrite("compute_capability_major", &optkit::gpu::GpuComputeInfo::compute_capability_major)
            .def_readwrite("compute_capability_minor", &optkit::gpu::GpuComputeInfo::compute_capability_minor)
            .def_readwrite("multiprocessor_count", &optkit::gpu::GpuComputeInfo::multiprocessor_count)
            .def_readwrite("cores_per_mp", &optkit::gpu::GpuComputeInfo::cores_per_mp)
            .def_readwrite("total_cores", &optkit::gpu::GpuComputeInfo::total_cores)
            .def_readwrite("warp_size", &optkit::gpu::GpuComputeInfo::warp_size);

        py::class_<optkit::gpu::GpuMemoryInfo>(gpu_query, "GpuMemoryInfo")
            .def(py::init<>())
            .def_readwrite("total_global_memory_MBytes", &optkit::gpu::GpuMemoryInfo::total_global_memory_MBytes)
            .def_readwrite("free_memory_MBytes", &optkit::gpu::GpuMemoryInfo::free_memory_MBytes)
            .def_readwrite("used_memory_MBytes", &optkit::gpu::GpuMemoryInfo::used_memory_MBytes)
            .def_readwrite("memory_bus_width_bits", &optkit::gpu::GpuMemoryInfo::memory_bus_width_bits)
            .def_readwrite("memory_utilization_percent", &optkit::gpu::GpuMemoryInfo::memory_utilization_percent);

        py::class_<optkit::gpu::GpuClockInfo>(gpu_query, "GpuClockInfo")
            .def(py::init<>())
            .def_readwrite("current_sm_clock_MHz", &optkit::gpu::GpuClockInfo::current_sm_clock_MHz)
            .def_readwrite("current_video_clock_MHz", &optkit::gpu::GpuClockInfo::current_video_clock_MHz)
            .def_readwrite("current_graphics_clock_MHz", &optkit::gpu::GpuClockInfo::current_graphics_clock_MHz)
            .def_readwrite("current_memory_clock_MHz", &optkit::gpu::GpuClockInfo::current_memory_clock_MHz)
            .def_readwrite("max_sm_clock_MHz", &optkit::gpu::GpuClockInfo::max_sm_clock_MHz)
            .def_readwrite("max_video_clock_MHz", &optkit::gpu::GpuClockInfo::max_video_clock_MHz)
            .def_readwrite("max_graphics_clock_MHz", &optkit::gpu::GpuClockInfo::max_graphics_clock_MHz)
            .def_readwrite("max_memory_clock_MHz", &optkit::gpu::GpuClockInfo::max_memory_clock_MHz)
            .def_readwrite("min_sm_clock_MHz", &optkit::gpu::GpuClockInfo::min_sm_clock_MHz)
            .def_readwrite("min_video_clock_MHz", &optkit::gpu::GpuClockInfo::min_video_clock_MHz)
            .def_readwrite("min_graphics_clock_MHz", &optkit::gpu::GpuClockInfo::min_graphics_clock_MHz)
            .def_readwrite("min_memory_clock_MHz", &optkit::gpu::GpuClockInfo::min_memory_clock_MHz)
            .def_readwrite("memory_supported_clock_rates_MHz", &optkit::gpu::GpuClockInfo::memory_supported_clock_rates_MHz)
            .def_readwrite("graphics_supported_clock_rates_MHz", &optkit::gpu::GpuClockInfo::graphics_supported_clock_rates_MHz)
            .def_readwrite("has_frequency_control", &optkit::gpu::GpuClockInfo::has_frequency_control);

        py::class_<optkit::gpu::GpuPowerInfo>(gpu_query, "GpuPowerInfo")
            .def(py::init<>())
            .def_readwrite("current_power_watts", &optkit::gpu::GpuPowerInfo::current_power_watts)
            .def_readwrite("power_limit_watts", &optkit::gpu::GpuPowerInfo::power_limit_watts)
            .def_readwrite("min_power_watts", &optkit::gpu::GpuPowerInfo::min_power_watts)
            .def_readwrite("max_power_watts", &optkit::gpu::GpuPowerInfo::max_power_watts)
            .def_readwrite("default_power_watts", &optkit::gpu::GpuPowerInfo::default_power_watts)
            .def_readwrite("has_power_monitoring", &optkit::gpu::GpuPowerInfo::has_power_monitoring)
            .def_readwrite("is_configurable", &optkit::gpu::GpuPowerInfo::is_configurable);

        py::class_<optkit::gpu::GpuTemperatureInfo>(gpu_query, "GpuTemperatureInfo")
            .def(py::init<>())
            .def_readwrite("current_device_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::current_device_temperature_celsius)
            .def_readwrite("current_memory_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::current_memory_temperature_celsius)
            .def_readwrite("max_device_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::max_device_temperature_celsius)
            .def_readwrite("max_memory_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::max_memory_temperature_celsius)
            .def_readwrite("min_device_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::min_device_temperature_celsius)
            .def_readwrite("min_memory_temperature_celsius", &optkit::gpu::GpuTemperatureInfo::min_memory_temperature_celsius)
            .def_readwrite("has_temperature_monitoring", &optkit::gpu::GpuTemperatureInfo::has_temperature_monitoring);

        py::class_<optkit::gpu::GpuUtilizationInfo>(gpu_query, "GpuUtilizationInfo")
            .def(py::init<>())
            .def_readwrite("gpu_utilization_percent", &optkit::gpu::GpuUtilizationInfo::gpu_utilization_percent)
            .def_readwrite("memory_utilization_percent", &optkit::gpu::GpuUtilizationInfo::memory_utilization_percent)
            .def_readwrite("has_utilization_monitoring", &optkit::gpu::GpuUtilizationInfo::has_utilization_monitoring);

        py::class_<optkit::gpu::GpuHardwareInfo>(gpu_query, "GpuHardwareInfo")
            .def(py::init<>())
            .def_readwrite("pci_bus_id", &optkit::gpu::GpuHardwareInfo::pci_bus_id)
            .def_readwrite("pci_device_id", &optkit::gpu::GpuHardwareInfo::pci_device_id)
            .def_readwrite("pci_subsystem_id", &optkit::gpu::GpuHardwareInfo::pci_subsystem_id)
            .def_readwrite("board_id", &optkit::gpu::GpuHardwareInfo::board_id)
            .def_readwrite("multi_gpu_board", &optkit::gpu::GpuHardwareInfo::multi_gpu_board);

        py::class_<optkit::gpu::GpuCapabilitiesInfo>(gpu_query, "GpuCapabilitiesInfo")
            .def(py::init<>())
            .def_readwrite("ecc_enabled", &optkit::gpu::GpuCapabilitiesInfo::ecc_enabled)
            .def_readwrite("supports_unified_memory", &optkit::gpu::GpuCapabilitiesInfo::supports_unified_memory)
            .def_readwrite("persistence_mode_enabled", &optkit::gpu::GpuCapabilitiesInfo::persistence_mode_enabled);

        py::class_<optkit::gpu::GpuDeviceInfo>(gpu_query, "GpuDeviceInfo")
            .def(py::init<>())
            .def_readwrite("basic", &optkit::gpu::GpuDeviceInfo::basic)
            .def_readwrite("version", &optkit::gpu::GpuDeviceInfo::version)
            .def_readwrite("compute", &optkit::gpu::GpuDeviceInfo::compute)
            .def_readwrite("memory", &optkit::gpu::GpuDeviceInfo::memory)
            .def_readwrite("clocks", &optkit::gpu::GpuDeviceInfo::clocks)
            .def_readwrite("power", &optkit::gpu::GpuDeviceInfo::power)
            .def_readwrite("temperature", &optkit::gpu::GpuDeviceInfo::temperature)
            .def_readwrite("utilization", &optkit::gpu::GpuDeviceInfo::utilization)
            .def_readwrite("hardware", &optkit::gpu::GpuDeviceInfo::hardware)
            .def_readwrite("capabilities", &optkit::gpu::GpuDeviceInfo::capabilities);

        py::class_<optkit::gpu::Query, std::unique_ptr<optkit::gpu::Query, py::nodelete>>(gpu_query, "Query")
            .def_static("is_device_exists", &optkit::gpu::Query::is_device_exists, py::arg("vendor"))
            .def_static(
                "get_device_count",
                [](optkit::gpu::GpuVendor vendor)
                {
                    ensure_gpu_query_initialized(vendor);
                    uint32_t count = 0;
                    bool ok = optkit::gpu::Query::get_device_count(vendor, count);
                    return py::make_tuple(ok, count);
                },
                py::arg("vendor"))
            .def_static(
                "device_query",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuDeviceInfo info{};
                    bool ok = optkit::gpu::Query::device_query(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_basic_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuBasicInfo info{};
                    bool ok = optkit::gpu::Query::get_basic_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_version_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuVersionInfo info{};
                    bool ok = optkit::gpu::Query::get_version_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_memory_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuMemoryInfo info{};
                    bool ok = optkit::gpu::Query::get_memory_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_clock_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuClockInfo info{};
                    bool ok = optkit::gpu::Query::get_clock_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_temperature_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuTemperatureInfo info{};
                    bool ok = optkit::gpu::Query::get_temperature_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_compute_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuComputeInfo info{};
                    bool ok = optkit::gpu::Query::get_compute_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_power_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuPowerInfo info{};
                    bool ok = optkit::gpu::Query::get_power_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_utilization_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuUtilizationInfo info{};
                    bool ok = optkit::gpu::Query::get_utilization_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_hardware_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuHardwareInfo info{};
                    bool ok = optkit::gpu::Query::get_hardware_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_capabilities_info",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    optkit::gpu::GpuCapabilitiesInfo info{};
                    bool ok = optkit::gpu::Query::get_capabilities_info(vendor, device_index, info);
                    return py::make_tuple(ok, info);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static("set_clock", &optkit::gpu::Query::set_clock, py::arg("vendor"), py::arg("device_index"), py::arg("mem_clk_mhz"), py::arg("graphics_clk_mhz"))
            .def_static("reset_clock", &optkit::gpu::Query::reset_clock, py::arg("vendor"), py::arg("device_index"))
            .def_static("reset_device", &optkit::gpu::Query::reset_device, py::arg("vendor"), py::arg("device_index"))
            .def_static("set_persistence_mode", &optkit::gpu::Query::set_persistence_mode, py::arg("vendor"), py::arg("device_index"), py::arg("enable"))
            .def_static("set_fan_speed", &optkit::gpu::Query::set_fan_speed, py::arg("vendor"), py::arg("device_index"), py::arg("fan_speed_percent"))
            .def_static(
                "get_fan_count",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    uint32_t count = 0;
                    bool ok = optkit::gpu::Query::get_fan_count(vendor, device_index, count);
                    return py::make_tuple(ok, count);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static("set_power_limit", &optkit::gpu::Query::set_power_limit, py::arg("vendor"), py::arg("device_index"), py::arg("power_limit_watts"))
            .def_static("reset_fan_speed", &optkit::gpu::Query::reset_fan_speed, py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_warp_size",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    uint32_t warp = 0;
                    bool ok = optkit::gpu::Query::get_warp_size(vendor, device_index, warp);
                    return py::make_tuple(ok, warp);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_driver_version",
                [](optkit::gpu::GpuVendor vendor)
                {
                    ensure_gpu_query_initialized(vendor);
                    double ver = 0.0;
                    bool ok = optkit::gpu::Query::get_driver_version(vendor, ver);
                    return py::make_tuple(ok, ver);
                },
                py::arg("vendor"))
            .def_static(
                "get_library_version",
                [](optkit::gpu::GpuVendor vendor)
                {
                    ensure_gpu_query_initialized(vendor);
                    std::string ver;
                    bool ok = optkit::gpu::Query::get_library_version(vendor, ver);
                    return py::make_tuple(ok, ver);
                },
                py::arg("vendor"))
            .def_static(
                "get_device_power",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    double power = 0.0;
                    bool ok = optkit::gpu::Query::get_device_power(vendor, device_index, power);
                    return py::make_tuple(ok, power);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_architecture",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    uint32_t arch = 0;
                    bool ok = optkit::gpu::Query::get_architecture(vendor, device_index, arch);
                    return py::make_tuple(ok, arch);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_device_power_limits",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    double limit = 0.0;
                    double defp = 0.0;
                    double minp = 0.0;
                    double maxp = 0.0;
                    bool is_cfg = false;
                    bool ok = optkit::gpu::Query::get_device_power_limits(vendor, device_index, limit, defp, minp, maxp, is_cfg);
                    return py::make_tuple(ok, limit, defp, minp, maxp, is_cfg);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_device_temperature",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    double dev = 0.0;
                    double mem = 0.0;
                    bool ok = optkit::gpu::Query::get_device_temperature(vendor, device_index, dev, mem);
                    return py::make_tuple(ok, dev, mem);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_device_name",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    std::string name;
                    bool ok = optkit::gpu::Query::get_device_name(vendor, device_index, name);
                    return py::make_tuple(ok, name);
                },
                py::arg("vendor"), py::arg("device_index"))
            .def_static(
                "get_device_temperature_thresholds",
                [](optkit::gpu::GpuVendor vendor, uint32_t device_index)
                {
                    ensure_gpu_query_initialized(vendor);
                    double max_gpu = 0.0;
                    double max_mem = 0.0;
                    double min_gpu = 0.0;
                    double min_mem = 0.0;
                    bool ok = optkit::gpu::Query::get_device_temperature_thresholds(vendor, device_index, max_gpu, max_mem, min_gpu, min_mem);
                    return py::make_tuple(ok, max_gpu, max_mem, min_gpu, min_mem);
                },
                py::arg("vendor"), py::arg("device_index"));
    }

    // -------------------------------------------------------------------------
    // PERF Module
    // -------------------------------------------------------------------------
    auto perf = m.def_submodule("perf");

    // The Global start() function: String-based API
    perf.def("start", [](std::string block_name, std::vector<std::string> metrics, std::vector<std::string> events)
             {
           require_initialized();
        
        // Create temporary default config. SafePerfProfiler will make a deep copy.
        optkit::pmu::cpu::perf::PerfProfilerConfig default_config(block_name.c_str(), true /*is_sampling*/);
        optkit::metrics::MetricBuilder<uint64_t> _metric;
        for (auto &&i : metrics)
            _metric.add(optkit::metrics::performance::cpu_metrics::get_metric(i));

        for (auto &&event_name : events)
            _metric.add(event_name, optkit::metrics::performance::cpu_mapper::get(event_name));

        auto safe_profiler = make_unique<SafePerfProfiler>(default_config, _metric);
        ProfilerManager<SafePerfProfiler>::push(std::move(safe_profiler)); }, "Start a new performance profiling block", py::arg("block_name"), py::arg("metrics"), py::arg("events") = std::vector<std::string>{});

    // The Global stop() function
    perf.def("stop", []()
             { ProfilerManager<SafePerfProfiler>::pop(); }, "Stop the most recent profiling block.");

    // -------------------------------------------------------------------------
    // Energy Module
    // -------------------------------------------------------------------------

    auto energy = m.def_submodule("energy");
    auto cpu = energy.def_submodule("cpu");
    auto gpu = energy.def_submodule("gpu");

    energy.def("start", [](std::string block_name)
               {
        require_initialized();

        optkit::ProfilerConfig profiler_config = make_profiler_config(block_name, "cpu_energy");
        if (should_use_pdu_cpu_energy())
        {
            auto safe_profiler = make_unique<SafePduProfiler>(profiler_config, optkit::energy::pdu::default_metrics());
            ProfilerManager<SafePduProfiler>::push(std::move(safe_profiler));
        }
#if OPTKIT_ENV_CPU_ARM
        else
        {
            auto safe_profiler = make_unique<SafeCpuHwmonProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
            ProfilerManager<SafeCpuHwmonProfiler>::push(std::move(safe_profiler));
        }
#else
        else
        {
            auto safe_profiler = make_unique<SafeRaplProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
            ProfilerManager<SafeRaplProfiler>::push(std::move(safe_profiler));
        }
#endif
        
        const auto gpu_mb = optkit::metrics::energy::gpu_metrics::all_metrics();
        try
        {
            optkit::ProfilerConfig nvidia_profiler_config = make_profiler_config(block_name, "nvidia_gpu_energy");
            auto nvidia_safe_profiler = make_unique<SafeNvidiaProfiler>(nvidia_profiler_config, gpu_mb);
            ProfilerManager<SafeNvidiaProfiler>::push(std::move(nvidia_safe_profiler));
        }
        catch (const std::exception &e)
        {
            py::print(std::string("NVIDIA GPU energy profiler not started: ") + e.what());
        }

        try
        {
            optkit::ProfilerConfig amd_profiler_config = make_profiler_config(block_name, "amd_gpu_energy");
            auto amd_safe_profiler = make_unique<SafeAmdProfiler>(amd_profiler_config, gpu_mb);
            ProfilerManager<SafeAmdProfiler>::push(std::move(amd_safe_profiler));
        }
        catch (const std::exception &e)
        {
            py::print(std::string("AMD GPU energy profiler not started: ") + e.what());
        } }, "Start CPU & GPU energy profiling", py::arg("block_name"));

    energy.def("stop", []()
               { ProfilerManager<SafePduProfiler>::pop(); ProfilerManager<SafeRaplProfiler>::pop(); ProfilerManager<SafeCpuHwmonProfiler>::pop(); ProfilerManager<SafeNvidiaProfiler>::pop(); ProfilerManager<SafeAmdProfiler>::pop(); }, "Stop CPU & GPU energy profiling");

    cpu.def("start", [](std::string block_name)
            {
        require_initialized();

        optkit::ProfilerConfig profiler_config = make_profiler_config(block_name, "cpu_energy");
        if (should_use_pdu_cpu_energy())
        {
            auto safe_profiler = make_unique<SafePduProfiler>(profiler_config, optkit::energy::pdu::default_metrics());
            ProfilerManager<SafePduProfiler>::push(std::move(safe_profiler));
        }
#if OPTKIT_ENV_CPU_ARM
        else
        {
            auto safe_profiler = make_unique<SafeCpuHwmonProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
            ProfilerManager<SafeCpuHwmonProfiler>::push(std::move(safe_profiler));
        }
#else
        else
        {
            auto safe_profiler = make_unique<SafeRaplProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
            ProfilerManager<SafeRaplProfiler>::push(std::move(safe_profiler));
        }
#endif
        }, "Start CPU energy profiling (PDU, HWMON, or RAPL backend)", py::arg("block_name"));

    cpu.def("stop", []()
            { ProfilerManager<SafePduProfiler>::pop(); ProfilerManager<SafeRaplProfiler>::pop(); ProfilerManager<SafeCpuHwmonProfiler>::pop(); }, "Stop CPU energy profiling");

    gpu.def("start", [](std::string block_name)
            {
        require_initialized();

        const auto gpu_mb = optkit::metrics::energy::gpu_metrics::all_metrics();
        try
        {
            optkit::ProfilerConfig nvidia_profiler_config = make_profiler_config(block_name, "nvidia_gpu_energy");
            auto nvidia_safe_profiler = make_unique<SafeNvidiaProfiler>(nvidia_profiler_config, gpu_mb);
            ProfilerManager<SafeNvidiaProfiler>::push(std::move(nvidia_safe_profiler));
        }
        catch (const std::exception &e)
        {
            py::print(std::string("NVIDIA GPU energy profiler not started: ") + e.what());
        }

        try
        {
            optkit::ProfilerConfig amd_profiler_config = make_profiler_config(block_name, "amd_gpu_energy");
            auto amd_safe_profiler = make_unique<SafeAmdProfiler>(amd_profiler_config, gpu_mb);
            ProfilerManager<SafeAmdProfiler>::push(std::move(amd_safe_profiler));
        }
        catch (const std::exception &e)
        {
            py::print(std::string("AMD GPU energy profiler not started: ") + e.what());
        } }, "Start GPU energy profiling", py::arg("block_name"));

    gpu.def("stop", []()
            { ProfilerManager<SafeAmdProfiler>::pop(); ProfilerManager<SafeNvidiaProfiler>::pop(); }, "Stop GPU energy profiling");

    // -------------------------------------------------------------------------
    // Callstack Module
    // -------------------------------------------------------------------------

    auto callstack = m.def_submodule("callstack");
    callstack.def("start", [](std::string block_name)
                  {
            require_initialized();

        optkit::pmu::cpu::perf::PerfProfilerConfig default_config {block_name.c_str(), true, false, ::getpid(), -1, "callstack"};
        auto safe_callstack_profiler = make_unique<SafeCallstackProfiler>(default_config);
        ProfilerManager<SafeCallstackProfiler>::push(std::move(safe_callstack_profiler)); }, "Start callstack profiling");

    callstack.def("stop", []()
                  { ProfilerManager<SafeCallstackProfiler>::pop(); }, "Stop callstack profiling");

    // -------------------------------------------------------------------------
    // Disk Module
    // -------------------------------------------------------------------------

    auto disk = m.def_submodule("disk");
    disk.def("start", [](std::string block_name)
             {
        require_initialized();

        optkit::ProfilerConfig profiler_config = make_profiler_config(block_name, "disk_io");
        auto safe_profiler = make_unique<SafeDiskProfiler>(profiler_config, optkit::metrics::disk::core_metrics::all_metrics());
        ProfilerManager<SafeDiskProfiler>::push(std::move(safe_profiler)); }, "Start disk I/O profiling", py::arg("block_name"));

    disk.def("stop", []()
             { ProfilerManager<SafeDiskProfiler>::pop(); }, "Stop disk I/O profiling");
    // -------------------------------------------------------------------------
    // Frequency Module
    // -------------------------------------------------------------------------

    auto frequency = m.def_submodule("frequency");

    py::enum_<optkit::frequency::Unit>(frequency, "Unit")
        .value("Hz", optkit::frequency::Unit::Hz)
        .value("KHz", optkit::frequency::Unit::KHz)
        .value("MHz", optkit::frequency::Unit::MHz)
        .value("GHz", optkit::frequency::Unit::GHz);

    frequency.def("convert", &optkit::frequency::convert_frequency_with_unit,
                  "Convert a frequency string (e.g. '2400 MHz') to target unit",
                  py::arg("freq_str"), py::arg("target_unit") = optkit::frequency::Unit::Hz);

    auto cpu_freq = frequency.def_submodule("cpu");
    cpu_freq.def("set_core_frequency", [](int64_t freq_khz, int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, socket); }, "Set all cores in socket to frequency (kHz)", py::arg("freq_khz"), py::arg("socket"));

    cpu_freq.def("set_core_frequency_core", [](int64_t freq_khz, int16_t cpu, int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, cpu, socket); }, "Set a single CPU core frequency (kHz)", py::arg("freq_khz"), py::arg("cpu"), py::arg("socket"));

    cpu_freq.def("set_core_frequency_range", [](int64_t freq_khz, int16_t cpu_start, int16_t cpu_end, int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::set_core_frequency(freq_khz, cpu_start, cpu_end, socket); }, "Set a range of CPU cores frequency (kHz)", py::arg("freq_khz"), py::arg("cpu_start"), py::arg("cpu_end"), py::arg("socket"));

    cpu_freq.def("get_core_frequency", [](int16_t cpu)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::get_core_frequency(cpu); }, "Get a single CPU core frequency (kHz)", py::arg("cpu"));

    cpu_freq.def("get_core_frequencies", [](int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::get_core_frequencies(socket); }, "Get all core frequencies for socket (kHz)", py::arg("socket"));

    cpu_freq.def("reset_core_frequency", [](int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::reset_core_frequency(socket); }, "Reset core frequency limits for socket", py::arg("socket"));

    cpu_freq.def("get_uncore_frequency", [](int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::get_uncore_frequency(socket); }, "Get uncore frequency for socket (kHz)", py::arg("socket"));

    cpu_freq.def("set_uncore_frequency", [](int64_t freq_khz, int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::set_uncore_frequency(freq_khz, socket); }, "Set uncore frequency for socket (kHz)", py::arg("freq_khz"), py::arg("socket"));

    cpu_freq.def("reset_uncore_frequency", [](int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket); }, "Reset uncore frequency for socket", py::arg("socket"));

    cpu_freq.def("get_uncore_min_max", [](int16_t socket)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::get_uncore_min_max(socket); }, "Get uncore min/max for socket (kHz)", py::arg("socket"));

    cpu_freq.def("get_scaling_available_uncore_frequencies", [](int16_t socket, int64_t step_khz)
                 { require_initialized(); return optkit::frequency::cpu::Frequency::get_scaling_available_uncore_frequencies(socket, step_khz); }, "Enumerate available uncore freqs for socket (kHz)", py::arg("socket"), py::arg("step_khz") = 200000);

    auto cpu_query = cpu_freq.def_submodule("query");
    cpu_query.def("available_core_frequencies", [](int32_t core, int64_t step_khz)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_scaling_available_core_frequencies(core, step_khz); }, py::arg("core") = 0, py::arg("step_khz") = 200000);
    cpu_query.def("available_governors", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_available_governors(core); }, py::arg("core") = 0);
    cpu_query.def("get_governor", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_scaling_governor(core); }, py::arg("core") = 0);
    cpu_query.def("set_governor", [](const std::string &gov, int32_t socket)
                  { require_initialized(); return optkit::frequency::cpu::Query::set_scaling_governor(gov, socket); }, py::arg("governor"), py::arg("socket") = 0);
    cpu_query.def("get_cpuinfo_max_freq", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_cpuinfo_max_freq(core); }, py::arg("core") = 0);
    cpu_query.def("get_cpuinfo_min_freq", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_cpuinfo_min_freq(core); }, py::arg("core") = 0);

    cpu_query.def("get_bios_limit", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_bios_limit(core); }, py::arg("core") = 0);

    cpu_query.def("get_scaling_driver", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_scaling_driver(core); }, py::arg("core") = 0);

    cpu_query.def("set_governor_percore", [](const std::string &gov, int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::set_scaling_governor_percore(gov, core); }, py::arg("governor"), py::arg("core") = 0);

    cpu_query.def("get_scaling_max_limit", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_scaling_max_limit(core); }, py::arg("core") = 0);

    cpu_query.def("get_scaling_min_limit", [](int32_t core)
                  { require_initialized(); return optkit::frequency::cpu::Query::get_scaling_min_limit(core); }, py::arg("core") = 0);

    // -------------------------------------------------------------------------
    // Temperature Module
    // -------------------------------------------------------------------------

    auto temperature = m.def_submodule("temperature");

    auto hwmon = temperature.def_submodule("hwmon");
    auto gpu_temp = temperature.def_submodule("gpu");

    temperature.def("start", [](std::string block_name)
                    {
        require_initialized();

        optkit::ProfilerConfig hwmon_cfg = make_profiler_config(block_name, "hwmon_temperature", true, false);
        auto hwmon_prof = make_unique<SafeHwmonTempProfiler>(hwmon_cfg, optkit::metrics::MetricBuilder<double>{});
        ProfilerManager<SafeHwmonTempProfiler>::push(std::move(hwmon_prof));

        try
        {
            optkit::ProfilerConfig gpu_cfg = make_profiler_config(block_name, "gpu_temperature", true, false);
            auto gpu_prof = make_unique<SafeGpuTempProfiler>(gpu_cfg, optkit::metrics::MetricBuilder<std::pair<double, double>>{});
            ProfilerManager<SafeGpuTempProfiler>::push(std::move(gpu_prof));
        }
        catch (const std::exception &e)
        {
            py::print(std::string("GPU temperature profiler not started: ") + e.what());
        } }, "Start HWMON + GPU temperature profiling", py::arg("block_name"));

    temperature.def("stop", []()
                    {
        ProfilerManager<SafeHwmonTempProfiler>::pop();
        ProfilerManager<SafeGpuTempProfiler>::pop(); }, "Stop HWMON + GPU temperature profiling");

    hwmon.def("start", [](std::string block_name)
              {
        require_initialized();
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "hwmon_temperature", true, false);
        auto prof = make_unique<SafeHwmonTempProfiler>(cfg, optkit::metrics::MetricBuilder<double>{});
        ProfilerManager<SafeHwmonTempProfiler>::push(std::move(prof)); }, "Start HWMON temperature profiling", py::arg("block_name"));

    hwmon.def("stop", []()
              { ProfilerManager<SafeHwmonTempProfiler>::pop(); }, "Stop HWMON temperature profiling");

    gpu_temp.def("start", [](std::string block_name)
                 {
        require_initialized();
        optkit::ProfilerConfig cfg = make_profiler_config(block_name, "gpu_temperature", true, false);
        auto prof = make_unique<SafeGpuTempProfiler>(cfg, optkit::metrics::MetricBuilder<std::pair<double, double>>{});
        ProfilerManager<SafeGpuTempProfiler>::push(std::move(prof)); }, "Start GPU temperature profiling", py::arg("block_name"));

    gpu_temp.def("stop", []()
                 { ProfilerManager<SafeGpuTempProfiler>::pop(); }, "Stop GPU temperature profiling");
}
