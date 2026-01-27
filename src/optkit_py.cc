#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <memory>
#include <stack>

#include "optkit.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "core/energy/cpu/rapl/profiler.hh"
#include "core/energy/gpu/nvidia/profiler.hh"
#include "core/energy/gpu/amd/profiler.hh"
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

static optkit::ProfilerConfig make_profiler_config(const std::string &block_name,
                                                   const char *measurement_type)
{
    return {block_name.c_str(),
            measurement_type,
            true, // is_reset_after_read
            true, // is_sampling
            optkit::Query::create_folder,
            !optkit::Query::create_folder};
}

/**
 * @brief Wrapper to ensure BlockProfiler's config name strings persist.
 */
struct SafeBlockProfiler
{
    std::string name_storage;
    std::unique_ptr<optkit::pmu::cpu::perf::BlockProfiler> profiler;

    SafeBlockProfiler(std::string name) : name_storage(std::move(name))
    {
        optkit::pmu::cpu::perf::PerfProfilerConfig config(name_storage.c_str());
        optkit::metrics::MetricBuilder<uint64_t> mb(true, false);
        profiler = make_unique<optkit::pmu::cpu::perf::BlockProfiler>(config, mb);
    }

    SafeBlockProfiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config,
                      const optkit::metrics::MetricBuilder<uint64_t> &mb)
    {
        name_storage = config.block_name ? config.block_name : "unknown";

        optkit::pmu::cpu::perf::PerfProfilerConfig safe_config = config;
        safe_config.block_name = name_storage.c_str();

        profiler = make_unique<optkit::pmu::cpu::perf::BlockProfiler>(safe_config, mb);
    }
};

template <typename ProfilerT>
struct SafeEnergyProfiler
{
    std::string name_storage;
    std::unique_ptr<ProfilerT> profiler;

    SafeEnergyProfiler(const optkit::ProfilerConfig &config,
                       const optkit::metrics::MetricBuilder<double> &mb)
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

using SafeRaplProfiler = SafeEnergyProfiler<optkit::energy::rapl::Profiler>;
using SafeNvidiaProfiler = SafeEnergyProfiler<optkit::energy::gpu::nvidia::Profiler>;
using SafeAmdProfiler = SafeEnergyProfiler<optkit::energy::gpu::amd::Profiler>;

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

private:
    static std::vector<std::unique_ptr<T>> &get_stack()
    {
        static std::vector<std::unique_ptr<T>> stack;
        return stack;
    }
};

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
        } }, "Finalize/Destroy OPTKIT engine");

    // -------------------------------------------------------------------------
    // PERF Module
    // -------------------------------------------------------------------------
    auto perf = m.def_submodule("perf");

    // The Global start() function: String-based API
    perf.def("start", [](std::string block_name, std::vector<std::string> metrics, std::vector<std::string> events)
             {
           require_initialized();
        
        // Create temporary default config. SafeBlockProfiler will make a deep copy.
        optkit::pmu::cpu::perf::PerfProfilerConfig default_config(block_name.c_str(), true /*is_sampling*/);
        optkit::metrics::MetricBuilder<uint64_t> _metric;
        for (auto &&i : metrics)
            _metric.add(optkit::metrics::performance::cpu_metrics::get_metric(i));

        for (auto &&event_name : events)
            _metric.add(event_name, optkit::metrics::performance::cpu_mapper::get(event_name));

        auto safe_profiler = make_unique<SafeBlockProfiler>(default_config, _metric);
        ProfilerManager<SafeBlockProfiler>::push(std::move(safe_profiler)); }, "Start a new performance profiling block", py::arg("block_name"), py::arg("metrics"), py::arg("events") = std::vector<std::string>{});

    // The Global stop() function
    perf.def("stop", []()
             { ProfilerManager<SafeBlockProfiler>::pop(); }, "Stop the most recent profiling block.");

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
        auto safe_profiler = make_unique<SafeRaplProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
        ProfilerManager<SafeRaplProfiler>::push(std::move(safe_profiler));
        
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
               { ProfilerManager<SafeRaplProfiler>::pop(); ProfilerManager<SafeNvidiaProfiler>::pop(); ProfilerManager<SafeAmdProfiler>::pop(); }, "Stop CPU & GPU energy profiling");

    cpu.def("start", [](std::string block_name)
            {
        require_initialized();

        optkit::ProfilerConfig profiler_config = make_profiler_config(block_name, "cpu_energy");
        auto safe_profiler = make_unique<SafeRaplProfiler>(profiler_config, optkit::metrics::energy::cpu_metrics::all_metrics());
        ProfilerManager<SafeRaplProfiler>::push(std::move(safe_profiler)); }, "Start CPU energy profiling (RAPL based)", py::arg("block_name"));

    cpu.def("stop", []()
            { ProfilerManager<SafeRaplProfiler>::pop(); }, "Stop CPU energy profiling (RAPL based)");

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
}
