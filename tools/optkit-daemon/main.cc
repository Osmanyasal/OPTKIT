#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <cstdlib>
#include <csignal>
#include <cstdint>

#include <thread>
#include <chrono>
#include "onnx_api.hh"
#include "optkit.hh"

struct DaemonArgs
{
    std::string model_path;
    std::vector<std::string> metrics;
    std::vector<std::string> events;
    bool help = false;
    bool parse_error = false;
    std::string parse_error_message;
};

static std::vector<std::string> split_csv(const std::string &s)
{
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        // trim whitespace
        const auto first = item.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
            continue;
        const auto last = item.find_last_not_of(" \t\n\r");
        out.emplace_back(item.substr(first, last - first + 1));
    }
    return out;
}

static bool parse_opt_with_value(const char *arg, const char *name, std::string &value_out)
{
    // Accept: --name=value
    const size_t name_len = std::strlen(name);
    if (std::strncmp(arg, name, name_len) != 0)
        return false;
    if (arg[name_len] != '=')
        return false;
    value_out = std::string(arg + name_len + 1);
    return true;
}

static DaemonArgs parse_daemon_args(int argc, char **argv)
{
    DaemonArgs args;

    for (int i = 1; i < argc; i++)
    {
        const char *a = argv[i];
        if (std::strcmp(a, "--help") == 0 || std::strcmp(a, "-h") == 0)
        {
            args.help = true;
            continue;
        }

        if (std::strcmp(a, "-m") == 0)
        {
            if (i + 1 >= argc)
            {
                args.parse_error = true;
                args.parse_error_message = "Missing value for -m";
                return args;
            }
            args.metrics.emplace_back(argv[++i]);
            continue;
        }
        if (std::strcmp(a, "-e") == 0)
        {
            if (i + 1 >= argc)
            {
                args.parse_error = true;
                args.parse_error_message = "Missing value for -e";
                return args;
            }
            args.events.emplace_back(argv[++i]);
            continue;
        }

        std::string opt_value;
        if (parse_opt_with_value(a, "--metrics", opt_value))
        {
            auto items = split_csv(opt_value);
            args.metrics.insert(args.metrics.end(), items.begin(), items.end());
            continue;
        }
        if (parse_opt_with_value(a, "--events", opt_value))
        {
            auto items = split_csv(opt_value);
            args.events.insert(args.events.end(), items.begin(), items.end());
            continue;
        }

        if (std::strcmp(a, "--metrics") == 0 || std::strcmp(a, "--metric") == 0)
        {
            if (i + 1 >= argc)
            {
                args.parse_error = true;
                args.parse_error_message = std::string("Missing value for ") + a;
                return args;
            }
            auto items = split_csv(argv[++i]);
            args.metrics.insert(args.metrics.end(), items.begin(), items.end());
            continue;
        }
        if (std::strcmp(a, "--events") == 0 || std::strcmp(a, "--event") == 0)
        {
            if (i + 1 >= argc)
            {
                args.parse_error = true;
                args.parse_error_message = std::string("Missing value for ") + a;
                return args;
            }
            auto items = split_csv(argv[++i]);
            args.events.insert(args.events.end(), items.begin(), items.end());
            continue;
        }

        if (a[0] == '-')
        {
            args.parse_error = true;
            args.parse_error_message = std::string("Unknown option: ") + a;
            return args;
        }

        // positional: model path
        if (args.model_path.empty())
        {
            args.model_path = std::string(a);
            continue;
        }

        args.parse_error = true;
        args.parse_error_message = std::string("Unexpected extra argument: ") + a;
        return args;
    }

    return args;
}

static void print_usage(const char *argv0)
{
    std::cerr << "Usage: " << argv0 << " [-m <metric>]... [-e <event>]... [<model.onnx>]\n";
    std::cerr << "       " << argv0 << " [--metrics M1,M2] [--events E1,E2] [<model.onnx>]\n";
    std::cerr << "  Or set OPTKIT_ONNX_MODEL=<model.onnx>\n";
    std::cerr << "Examples:\n";
    std::cerr << "  " << argv0 << " -m cycles -m instructions -e cycles model.onnx\n";
    std::cerr << "  " << argv0 << " --metrics cycles,instructions --events cpu/event=0x3c,umask=0x00/ model.onnx\n";
}

static std::unordered_map<std::string, uint64_t> event_counts_from_values(
    const std::vector<std::string> &event_names,
    const std::vector<uint64_t> &values,
    double duration_ms)
{
    std::unordered_map<std::string, uint64_t> counts;

    if (!event_names.empty() && !values.empty())
    {
        for (size_t j = 0; j < values.size(); ++j)
            counts[event_names[j % event_names.size()]] += values[j];
    }

    counts["duration_microsec"] = static_cast<uint64_t>(duration_ms * 1000.0);
    return counts;
}

std::string resolve_model_path(int argc, char **argv)
{
    if (argc > 1)
        return argv[1];

    const char *env_model = std::getenv("OPTKIT_ONNX_MODEL");
    if (env_model != nullptr && env_model[0] != '\0')
        return std::string(env_model);

    return {};
}

bool IS_RUNNING = true;
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "Received SIGINT, shutting down...\n";
        IS_RUNNING = false;
    }
    else if (signal == SIGTERM)
    {
        std::cout << "Received SIGTERM, shutting down...\n";
        IS_RUNNING = false;
    }
}
void setup_signal_handlers()
{
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // `kill` default
}

int main(int argc, char **argv)
{
    OPTKIT_INIT(false);

    DaemonArgs args = parse_daemon_args(argc, argv);
    if (args.help)
    {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (args.parse_error)
    {
        std::cerr << "Error: " << args.parse_error_message << "\n";
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (args.model_path.empty())
    {
        const char *env_model = std::getenv("OPTKIT_ONNX_MODEL");
        if (env_model != nullptr && env_model[0] != '\0')
            args.model_path = std::string(env_model);
    }

    if (args.model_path.empty())
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    try
    {
        setup_signal_handlers();

        // Initialize metrics
        optkit::metrics::MetricBuilder<uint64_t> _metric;
        for (auto &&i : args.metrics)
            _metric.add(optkit::metrics::performance::cpu_metrics::get_metric(i));

        for (auto &&event_name : args.events)
            _metric.add(event_name, optkit::metrics::performance::cpu_mapper::get(event_name));

        optkit::pmu::cpu::perf::PerfProfilerConfig perf_config{"stat"};
        // system wide
        perf_config.pid = -1;
        perf_config.cpu = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS;
        optkit::pmu::cpu::perf::BlockProfiler stat_metric_profiler(perf_config, _metric);
        optkit::disk::IoDiskProfiler disk_profiler{
            {"stat", "disk_io", true, false, optkit::Query::create_folder, !optkit::Query::create_folder, false},
            optkit::metrics::disk::core_metrics::all_metrics()};

        auto last = std::chrono::steady_clock::now();
        while (IS_RUNNING)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            const auto now = std::chrono::steady_clock::now();
            const double duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - last).count();
            last = now;

            const std::vector<uint64_t> values = stat_metric_profiler.read();
            const auto counts = event_counts_from_values(_metric.event_names(), values, duration_ms);
            const auto mapped = _metric.calculate(counts);
            
            for (const auto &kv : counts)
                std::cout << kv.first << ": " << kv.second << ",";
            for (const auto &kv : mapped)
                std::cout << kv.first << ": " << kv.second << "\n";
            // std::unique_ptr<OnnxApi> onnx_api = create_onnx_api();
            // onnx_api->load_model(model_path);
            // InferenceSummary summary = onnx_api->infer();

            // std::cout << "Model loaded: " << model_path << "\n";
            // std::cout << "Input: " << summary.input.name << " elements=" << summary.input.elements << "\n";
            // std::cout << "Output: " << summary.output.name << " elements=" << summary.output.elements << " shape=[";
            // for (size_t i = 0; i < summary.output.shape.size(); ++i)
            // {
            //     if (i)
            //         std::cout << ",";
            //     std::cout << summary.output.shape[i];
            // }
            // std::cout << "]\n";
            // std::cout << "Inference completed successfully\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}