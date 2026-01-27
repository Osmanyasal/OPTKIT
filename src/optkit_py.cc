#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <memory>
#include <stack>

#include "optkit.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
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
/**
 * @brief Wrapper to ensure BlockProfiler's config name strings persist.
 *
 * The core BlockProfiler stores a config which stores a `const char*`.
 * If that pointer comes from a temporary standard string, it dangles.
 * This wrapper owns the string.
 */
struct SafeBlockProfiler
{
    std::string name_storage;
    std::unique_ptr<optkit::pmu::cpu::perf::BlockProfiler> profiler;

    // Constructor for convenience (name only)
    SafeBlockProfiler(std::string name) : name_storage(std::move(name))
    {
        optkit::pmu::cpu::perf::PerfProfilerConfig config(name_storage.c_str());
        // Default metric builder
        optkit::metrics::MetricBuilder<uint64_t> mb(true, false);
        profiler = make_unique<optkit::pmu::cpu::perf::BlockProfiler>(config, mb);
    }

    // Constructor for explicit config
    SafeBlockProfiler(const optkit::pmu::cpu::perf::PerfProfilerConfig &config,
                      const optkit::metrics::MetricBuilder<uint64_t> &mb)
    {
        if (config.block_name)
        {
            name_storage = config.block_name;
        }
        else
        {
            name_storage = "unknown";
        }

        // Create a safe copy of config pointing to our persistent string
        optkit::pmu::cpu::perf::PerfProfilerConfig safe_config = config;
        safe_config.block_name = name_storage.c_str();

        profiler = make_unique<optkit::pmu::cpu::perf::BlockProfiler>(safe_config, mb);
    }
};

// -----------------------------------------------------------------------------
// Python Wrapper Module
// -----------------------------------------------------------------------------

PYBIND11_MODULE(optkit_py, m)
{
    m.doc() = "Python bindings for OPTKIT with start/stop semantics";

    // -------------------------------------------------------------------------
    // Top-Level Initialization & Convenience
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
        if (!global_optkit_instance) {
             py::print("Warning: OPTKIT not initialized! Call optkit_py.init() first.");
        }
        
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
}
