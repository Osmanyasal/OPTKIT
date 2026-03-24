#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <cstdlib>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <cmath>

#include "utils/json.hh"

#include <thread>
#include <chrono>
#include <deque>
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
    bool print_data = false;
    bool verbose = false;
    bool check_features = false;
};

#if OPTKIT_DAEMON_WITH_ONNX
struct ModelMeta
{
    std::vector<std::string> feature_names;
    std::vector<std::string> targets;
    size_t window = 0;
};

static std::string dirname_of(const std::string &path)
{
    const auto pos = path.find_last_of('/');
    if (pos == std::string::npos)
        return ".";
    if (pos == 0)
        return "/";
    return path.substr(0, pos);
}

static bool load_model_meta(const std::string &model_path, ModelMeta &meta_out)
{
    const std::string meta_path = dirname_of(model_path) + "/meta.json";
    std::ifstream f(meta_path.c_str());
    if (!f)
        return false;

    nlohmann::json j;
    try
    {
        f >> j;
    }
    catch (...)
    {
        return false;
    }

    try
    {
        if (j.contains("window"))
        {
            // meta.json window is an integer.
            const auto w = j["window"];
            if (w.is_number_integer())
                meta_out.window = static_cast<size_t>(w.get<int>());
        }

        if (j.contains("feature_names") && j["feature_names"].is_array())
        {
            meta_out.feature_names.clear();
            for (const auto &v : j["feature_names"])
            {
                if (v.is_string())
                    meta_out.feature_names.emplace_back(v.get<std::string>());
            }
        }

        if (j.contains("targets") && j["targets"].is_array())
        {
            meta_out.targets.clear();
            for (const auto &v : j["targets"])
            {
                if (v.is_string())
                    meta_out.targets.emplace_back(v.get<std::string>());
            }
        }
    }
    catch (...)
    {
        return false;
    }

    return !meta_out.feature_names.empty();
}

static void add_prefixed_features(std::unordered_map<std::string, float> &dst,
                                  const std::string &prefix,
                                  const std::unordered_map<std::string, uint64_t> &counts,
                                  const std::vector<std::pair<std::string, double>> &mapped,
                                  double duration_ms)
{
    auto strip_unit_suffix = [](const std::string &name) -> std::string
    {
        const auto pos = name.find("__");
        if (pos == std::string::npos)
            return name;
        return name.substr(0, pos);
    };

    dst[prefix + "duration_ms"] = static_cast<float>(duration_ms);
    dst[prefix + "duration_microsec"] = static_cast<float>(duration_ms * 1000.0);

    for (const auto &kv : counts)
        dst[prefix + kv.first] = static_cast<float>(kv.second);

    for (const auto &kv : mapped)
    {
        if (!std::isfinite(kv.second))
            continue;
        dst[prefix + kv.first] = static_cast<float>(kv.second);

        const std::string base = strip_unit_suffix(kv.first);
        if (base != kv.first)
            dst[prefix + base] = static_cast<float>(kv.second);
    }
}
#endif

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
        std::string opt_value;
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
        if (std::strcmp(a, "-o") == 0)
        {
            args.print_data = true;
            continue;
        }

        if (std::strcmp(a, "--verbose") == 0 || std::strcmp(a, "-v") == 0)
        {
            args.verbose = true;
            continue;
        }

        if (std::strcmp(a, "--check-features") == 0)
        {
            args.check_features = true;
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
    std::cerr << "Options:\n";
    std::cerr << "  -o                 Print raw and derived metrics each tick\n";
    std::cerr << "  -v, --verbose       Print infer() timing (ms) each tick\n";
    std::cerr << "  --check-features    Print meta.json feature coverage + input stats\n";
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
    // reset frequency
    for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
    {
        optkit::frequency::cpu::Frequency::reset_core_frequency(socket);
        optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket);
    }
    exit(EXIT_SUCCESS);
}
void setup_signal_handlers()
{
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // `kill` default
}

int main(int argc, char **argv)
{
    setup_signal_handlers();
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
#if !OPTKIT_DAEMON_WITH_ONNX
        std::cerr << "Error: optkit-daemon was built without ONNX Runtime support (build with WITH_ONNX=1).\n";
        return EXIT_FAILURE;
#else
        std::unique_ptr<OnnxApi> onnx_api = create_onnx_api();
        onnx_api->load_model(args.model_path);
        ModelMeta meta;
        const bool have_meta = load_model_meta(args.model_path, meta);
        if (!have_meta)
        {
            std::cerr << "WARNING: could not read meta.json next to model; will run inference with zero-filled input (likely meaningless).\n";
        }
        bool warned_meta_mismatch = false;

        const size_t model_input_elems = onnx_api->input_summary().elements;
        const size_t input_elems = have_meta ? meta.feature_names.size() : model_input_elems;
        if (input_elems == 0)
            throw std::runtime_error("Model input has zero elements");

        const size_t window = (have_meta && meta.window > 0) ? meta.window : 1;
        std::deque<std::vector<float>> history;

        // Only apply frequency changes if the predicted value differs from the last applied
        // value by at least 1 GHz. Frequencies are in kHz here.
        // We intentionally track a single last-applied value because we apply the same
        // target frequency to all sockets.
        constexpr int64_t FREQ_CHANGE_THRESHOLD_KHZ = 500000; // 0.5 GHz
        int64_t last_applied_core_khz = -1;
        int64_t last_applied_uncore_khz = -1;

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

        auto last = std::chrono::steady_clock::now();
        while (IS_RUNNING)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            const auto now = std::chrono::steady_clock::now();
            const double duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - last).count();
            last = now;

            const std::vector<uint64_t> values = stat_metric_profiler.read();
            const auto pmc_counts = event_counts_from_values(_metric.event_names(), values, duration_ms);
            const auto pmc_mapped = _metric.calculate(pmc_counts);

            if (args.print_data)
            {
                for (const auto &kv : pmc_counts)
                    std::cout << kv.first << ": " << kv.second << ",";
                for (const auto &kv : pmc_mapped)
                    std::cout << kv.first << ": " << kv.second << "\n";
                std::cout << "==============================\n";
            }

            std::vector<float> frame;
            frame.assign(input_elems, 0.0f);

            if (have_meta && meta.feature_names.size() == frame.size())
            {
                std::unordered_map<std::string, float> feat;
                add_prefixed_features(feat, "cpu_pmu.", pmc_counts, pmc_mapped, duration_ms);

                for (size_t k = 0; k < meta.feature_names.size(); ++k)
                {
                    const auto it = feat.find(meta.feature_names[k]);
                    if (it != feat.end())
                        frame[k] = it->second;
                }
            }
            else if (have_meta && !warned_meta_mismatch)
            {
                warned_meta_mismatch = true;
                std::cerr << "WARNING: meta.json feature_names size (" << meta.feature_names.size()
                          << ") does not match inferred input elements (" << frame.size() << ")\n";
                if (model_input_elems != 0 && model_input_elems != frame.size())
                    std::cerr << "WARNING: ONNX reported input elements (" << model_input_elems << ") differ from inferred (" << frame.size() << ")\n";
            }

            history.push_back(std::move(frame));
            while (history.size() > window)
                history.pop_front();

            // Flatten to (1, window, features). Left-pad with zeros until history is full.
            std::vector<float> input_seq;
            input_seq.assign(window * input_elems, 0.0f);
            const size_t have = history.size();
            const size_t offset = window - have;
            for (size_t i = 0; i < have; ++i)
            {
                const auto &v = history[i];
                std::copy(v.begin(), v.end(), input_seq.begin() + (offset + i) * input_elems);
            }

            if (args.check_features && have_meta)
            {
                // Rebuild the current feature map (same as we used to fill the frame)
                // to report coverage.
                std::unordered_map<std::string, float> feat;
                add_prefixed_features(feat, "cpu_pmu.", pmc_counts, pmc_mapped, duration_ms);

                size_t found = 0;
                std::vector<std::string> missing;
                missing.reserve(meta.feature_names.size());
                for (const auto &name : meta.feature_names)
                {
                    if (feat.find(name) != feat.end())
                        ++found;
                    else
                        missing.emplace_back(name);
                }

                float vmin = 0.0f, vmax = 0.0f;
                bool have_any = false;
                for (float v : input_seq)
                {
                    if (!have_any)
                    {
                        vmin = vmax = v;
                        have_any = true;
                    }
                    else
                    {
                        if (v < vmin)
                            vmin = v;
                        if (v > vmax)
                            vmax = v;
                    }
                }

                std::cerr << "features_found=" << found << "/" << meta.feature_names.size()
                          << " window=" << window << " input_min=" << vmin << " input_max=" << vmax << "\n";
                if (!missing.empty())
                {
                    std::cerr << "missing_features(" << missing.size() << "):";
                    const size_t show = std::min<size_t>(missing.size(), 10);
                    for (size_t i = 0; i < show; ++i)
                        std::cerr << " " << missing[i];
                    if (missing.size() > show)
                        std::cerr << " ...";
                    std::cerr << "\n";
                }
            }

            const auto infer_t0 = std::chrono::steady_clock::now();
            InferenceSummary summary = onnx_api->infer(input_seq, {1, static_cast<int64_t>(window), static_cast<int64_t>(input_elems)});
            const auto infer_t1 = std::chrono::steady_clock::now();
            const double infer_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(infer_t1 - infer_t0).count();

            if (args.verbose)
                std::cerr << "infer_ms=" << infer_ms << "\n";

            if (!summary.output_values.empty())
            {
                if (summary.output_values.size() == 1)
                {
                    if (args.verbose)
                        std::cout << "pred_core_ghz=" << summary.output_values[0] << "\n";
                    const double pred_core_ghz = summary.output_values[0];
                    if (std::isfinite(pred_core_ghz) && pred_core_ghz > 0.0)
                    {
                        const int64_t target_core_khz = static_cast<int64_t>(std::llround(pred_core_ghz * 1000000.0));
                        const int64_t diff = (last_applied_core_khz < 0) ? FREQ_CHANGE_THRESHOLD_KHZ : std::llabs(target_core_khz - last_applied_core_khz);
                        if (diff >= FREQ_CHANGE_THRESHOLD_KHZ)
                        {
                            for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                            {
                                optkit::frequency::cpu::Frequency::set_core_frequency(target_core_khz, socket);
                            }
                            last_applied_core_khz = target_core_khz;
                        }
                    }
                }
                else if (summary.output_values.size() >= 2)
                {
                    if (args.verbose)
                        std::cout << "pred_core_ghz=" << summary.output_values[0]
                                  << " pred_uncore_ghz=" << summary.output_values[1] << "\n";
                    const double pred_core_ghz = summary.output_values[0];
                    const double pred_uncore_ghz = summary.output_values[1];
                    const bool core_ok = (std::isfinite(pred_core_ghz) && pred_core_ghz > 0.0);
                    const bool uncore_ok = (std::isfinite(pred_uncore_ghz) && pred_uncore_ghz > 0.0);

                    const int64_t target_core_khz = core_ok ? static_cast<int64_t>(std::llround(pred_core_ghz * 1000000.0)) : -1;
                    const int64_t target_uncore_khz = uncore_ok ? static_cast<int64_t>(std::llround(pred_uncore_ghz * 1000000.0)) : -1;

                    if (core_ok)
                    {
                        const int64_t diff = (last_applied_core_khz < 0) ? FREQ_CHANGE_THRESHOLD_KHZ : std::llabs(target_core_khz - last_applied_core_khz);
                        if (diff >= FREQ_CHANGE_THRESHOLD_KHZ)
                        {
                            for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                            {
                                optkit::frequency::cpu::Frequency::set_core_frequency(target_core_khz, socket);
                            }
                            last_applied_core_khz = target_core_khz;
                        }
                    }

                    if (uncore_ok)
                    {
                        const int64_t diff = (last_applied_uncore_khz < 0) ? FREQ_CHANGE_THRESHOLD_KHZ : std::llabs(target_uncore_khz - last_applied_uncore_khz);
                        if (diff >= FREQ_CHANGE_THRESHOLD_KHZ)
                        {
                            for (int socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                            {
                                optkit::frequency::cpu::Frequency::set_uncore_frequency(target_uncore_khz, socket);
                            }
                            last_applied_uncore_khz = target_uncore_khz;
                        }
                    }
                }
                else
                {
                    std::cout << "pred=[empty]\n";
                }
            }
            else
            {
                std::cout << "pred=[no_output]\n";
            }
        }
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}