
#include <csignal>
#include <cstdint>
#include "utils.hh"
// For rewriting JSON files in-place
#include <fstream>

// Generic JSON key/value injector.
// Adds a top-level key:value (string inserted verbatim) to every JSON file in `dir` unless the key already exists.
// Anchor-based insertion (by default before first "duration") keeps ordering consistent; falls back near the start.
static void inject_json_kv(const std::string &dir,
                           const std::string &key,
                           const std::string &value_literal, // Must be a JSON literal (e.g. 123, "str", true)
                           const std::string &anchor = "\"duration\"")
{
    if (key.empty())
        return;

    const std::string key_pattern = std::string("\"") + key + "\""; // "key"
    const std::string insertion_prefix = key_pattern + ": " + value_literal + ", ";

    std::vector<std::string> files = optkit::utils::get_all_files(dir);
    for (const auto &path : files)
    {
        if (path.size() < 5 || path.substr(path.size() - 5) != ".json")
            continue; // only JSON files

        const std::string file = dir + "/" + path;
        std::string content;
        try
        {
            content = optkit::utils::read_file(file);
        }
        catch (...)
        {
            continue; // unreadable
        }

        if (content.find(key_pattern) != std::string::npos)
            continue; // already annotated

        bool inserted = false;
        size_t anchor_pos = content.find(anchor);
        if (anchor_pos != std::string::npos)
        {
            content.insert(anchor_pos, insertion_prefix);
            inserted = true;
        }
        else
        {
            // Fallback: after first '[' or '{'
            size_t pos = content.find('[');
            if (pos == std::string::npos)
                pos = content.find('{');
            if (pos != std::string::npos)
            {
                ++pos; // after bracket/brace
                while (pos < content.size() && std::isspace(static_cast<unsigned char>(content[pos])))
                    ++pos;
                content.insert(pos, insertion_prefix);
                inserted = true;
            }
        }

        if (!inserted)
            continue;

        std::ofstream out(file.c_str(), std::ios::out | std::ios::trunc);
        if (!out.is_open())
            continue;
        out << content;
    }
}

template <typename T>
static bool apply_freq_window_in_place(std::vector<T> &values, uint32_t start, uint32_t limit)
{
    if (values.empty())
        return true;

    if (start >= values.size())
        return false;

    const size_t s = static_cast<size_t>(start);
    const size_t n = values.size();
    const size_t max_take = n - s;
    const size_t take = (limit == 0) ? max_take : std::min(max_take, static_cast<size_t>(limit));

    std::vector<T> out;
    out.reserve(take);
    for (size_t i = 0; i < take; ++i)
        out.push_back(values[s + i]);
    values.swap(out);
    return true;
}

bool IS_RUNNING = false;
int32_t CHILD_PID = -1;

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        if (CHILD_PID > 0)
            kill(CHILD_PID, SIGINT);
        else
            IS_RUNNING = false;
    }
    else if (signal == SIGTERM)
    {
        if (CHILD_PID > 0)
            kill(CHILD_PID, SIGTERM);
        else
            IS_RUNNING = false;
    }
}

void setup_signal_handlers()
{
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // `kill` default
}

void create_child_process(const CommandArgs &args)
{
    const std::string &program = args.program;
    const std::vector<std::string> &program_args = args.program_args;
    auto events = args.events;
    auto metrics = args.metrics;

    CHILD_PID = fork();
    if (CHILD_PID == 0)
    {
        std::vector<char *> c_args;
        c_args.push_back(const_cast<char *>(program.c_str()));
        for (const auto &arg : program_args)
            c_args.push_back(const_cast<char *>(arg.c_str()));
        c_args.push_back(nullptr);

        execvp(program.c_str(), c_args.data());

        perror("execvp failed");
        _exit(1);
    }
    else if (CHILD_PID > 0) // parent
    {
        std::cout << "[Started child process with PID: " << CHILD_PID << "]\n";
        IS_RUNNING = true;
        int status;
        auto begin_time = std::chrono::high_resolution_clock::now();

        // Initialize metrics
        optkit::metrics::MetricBuilder<uint64_t> _metric;
        for (auto &&i : args.metrics)
            _metric.add(optkit::metrics::performance::cpu_metrics::get_metric(i));

        for (auto &&event_name : args.events)
            _metric.add(event_name, optkit::metrics::performance::cpu_mapper::get(event_name));

        optkit::pmu::cpu::perf::PerfProfilerConfig perf_config{"stat", true /*is_sampling*/};
        if (args.is_screenshot)
            perf_config.is_screenshot = true;
        if (args.system_wide)
        {
            perf_config.pid = -1;
            perf_config.cpu = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS;
        }
        else
        {
            perf_config.pid = CHILD_PID;
            perf_config.cpu = -1;
        }
        optkit::pmu::cpu::perf::BlockProfiler stat_metric_profiler(perf_config, _metric);
        // open with sampling.
        optkit::callstack::Profiler callstack_profiler{{"stat", true, false, CHILD_PID, -1, "callstack"}};

        // optkit::disk::IoDiskProfiler disk_profiler{
        //     {"stat", "disk_io", true, args.is_screenshot, optkit::Query::create_folder, !optkit::Query::create_folder, args.is_screenshot},
        //     optkit::metrics::disk::core_metrics::all_metrics()};

        OPTKIT_CPU_ENERGY_SAMPLING("stat");
        OPTKIT_GPU_ENERGY("stat");
        while (IS_RUNNING)
        {
            pid_t result = waitpid(CHILD_PID, &status, WNOHANG);
            if (OPT_LIKELY(result == 0))         // Child is still running
                usleep(100000);                  // 0.1 sec
            else if (OPT_UNLIKELY(result == -1)) // Error
                std::cerr << "waitpid error: " << strerror(errno) << std::endl;
            else // child is finished!
                IS_RUNNING = false;
        }
        CHILD_PID = -1;
    }
    else
    { // error
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void execute_stat_command(const CommandArgs &args)
{
    if (args.program.empty())
    {
        std::cerr << "Error: No program specified for profiling\n";
        std::cerr << "Usage: optkit stat [OPTIONS] -- <program> [args...]\n";
        return;
    }

    setup_signal_handlers();

    // Get topology to determine socket configuration
    const auto &socket_cpus = optkit::Query::detect_cpu_packages();

    switch (args.bench_type)
    {
    case BenchType::FREQ_SCALING:
    {
        OPTKIT_INIT({true});
        std::cout << "\n[Will execute program multiple times with varying configurations]\n";
        std::cout << "Frequency Scaling Analysis (multiple executions)\n";
        std::unordered_map<uint32_t, std::string> socket_curr_governor;
        for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
            socket_curr_governor[socket] = optkit::frequency::cpu::Query::get_scaling_governor(socket_cpus.at(socket)[0]);

        auto avail_core_freqs = optkit::frequency::cpu::Query::get_scaling_available_core_frequencies(0);         // assuming all sockets are the same and have same available frequencies
        auto avail_uncore_freqs = optkit::frequency::cpu::Frequency::get_scaling_available_uncore_frequencies(0); // assuming all sockets are the same and have same available frequencies
        if (avail_uncore_freqs.empty())
        {
            std::cerr << "Error: No available uncore frequencies found for scaling analysis\n";
            avail_uncore_freqs.push_back(0);
        }

        if (!apply_freq_window_in_place(avail_core_freqs, args.freq_start, args.freq_limit))
        {
            std::cerr << "Error: --freq-start " << args.freq_start << " is out of range for core frequencies (size=" << avail_core_freqs.size() << ")\n";
            return;
        }

        const bool uncore_unavailable = (avail_uncore_freqs.size() == 1 && avail_uncore_freqs[0] == 0);
        if (!uncore_unavailable)
        {
            if (!apply_freq_window_in_place(avail_uncore_freqs, args.freq_start, args.freq_limit))
            {
                std::cerr << "Error: --freq-start " << args.freq_start << " is out of range for uncore frequencies (size=" << avail_uncore_freqs.size() << ")\n";
                return;
            }
        }

        // traverse all combinations of core and uncore frequencies
        for (const auto &core_freq : avail_core_freqs)
            for (const auto &uncore_freq : avail_uncore_freqs)
            {
                std::cout << "\n"
                          << std::string(60, '-') << "\n";
                std::cout << "Setting core frequency to " << core_freq / 1000 << " MHz\n";
                std::cout << "Setting uncore frequency to " << uncore_freq / 1000 << " MHz\n";
                std::cout << std::string(60, '-') << "\n";

                // Set frequency for all sockets and cores
                for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                {
                    optkit::frequency::cpu::Query::set_scaling_governor("performance", socket);
                    optkit::frequency::cpu::Frequency::set_core_frequency(core_freq, socket);
                    if (uncore_freq > 0)
                        optkit::frequency::cpu::Frequency::set_uncore_frequency(uncore_freq, socket);
                }
                // Create modified args with frequency settings

                create_child_process(args);
                inject_json_kv(optkit::utils::EXECUTION_FOLDER_NAME, "core_frequency_khz", std::to_string(core_freq));
                inject_json_kv(optkit::utils::EXECUTION_FOLDER_NAME, "uncore_frequency_khz", std::to_string(uncore_freq));
                optkit::utils::EXECUTION_FOLDER_NAME = {optkit::utils::get_date() + "__" + optkit::utils::get_time() + "__" + optkit::utils::generateGUID().substr(0, CONF_LOG_PRINT_GUID_LENGTH)};
                optkit::utils::create_directory(optkit::utils::EXECUTION_FOLDER_NAME);

                // Small delay between runs
                usleep(500000); // 0.5 second
            }
        optkit::utils::remove_directory(optkit::utils::EXECUTION_FOLDER_NAME);

        // Restore original governors and frequencies
        for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
        {
            optkit::frequency::cpu::Query::set_scaling_governor(socket_curr_governor.at(socket), socket);
            optkit::frequency::cpu::Frequency::reset_core_frequency(socket);
            if (avail_uncore_freqs.size() == 1 && avail_uncore_freqs[0] == 0)
                optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket);
        }
        break;
    }

    case BenchType::CORE_SCALING:
    {
        OPTKIT_INIT({true});
        std::cout << "\n[Will execute program multiple times with varying configurations]\n";
        std::cout << "Core Scaling Analysis (multiple executions)\n";
        // Get number of processors
        if (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS <= 0)
        {
            std::cerr << "Error: Could not determine number of processors\n";
            return;
        }

        std::cout << "System has " << OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS << " processors across " << OPTKIT_ENV_CPU_NUM_SOCKETS << " socket(s)\n";
        std::cout << std::string(60, '=') << "\n";

        // Determine which cores to use based on socket selection
        std::vector<int> cpus_to_use;
        if (args.socket_id == static_cast<uint32_t>(-1))
        {
            // Use all cores from all sockets
            std::cout << "Using all available cores from all sockets\n";
            for (const auto &socket_entry : socket_cpus)
            {
                const std::vector<int> &socket_cpus_list = socket_entry.second;
                cpus_to_use.insert(cpus_to_use.end(), socket_cpus_list.begin(), socket_cpus_list.end());
            }
        }
        else
        {
            // Use cores from specific socket only
            std::cout << "Using cores from socket " << args.socket_id << " only\n";
            auto it = socket_cpus.find(args.socket_id);
            if (it == socket_cpus.end())
            {
                std::cerr << "Error: Socket " << args.socket_id << " not found\n";
                return;
            }
            cpus_to_use = it->second;
        }

        std::sort(cpus_to_use.begin(), cpus_to_use.end());

        int total_cores = cpus_to_use.size();
        std::cout << "Using " << total_cores << " CPUs: [";
        for (size_t i = 0; i < cpus_to_use.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << cpus_to_use[i];
        }
        std::cout << "]\n";
        std::cout << std::string(60, '=') << "\n";

        // Execute with 1, 2, 4, 8, ... cores up to total_cores
        int num_cores = 1;
        for (; num_cores <= total_cores; num_cores *= 2)
        {
            std::cout << "\n"
                      << std::string(60, '-') << "\n";
            std::cout << "Running with " << num_cores << " core(s)\n";
            std::cout << std::string(60, '-') << "\n";

            // Build CPU mask using selected cores
            std::string cpu_list = std::to_string(cpus_to_use[0]);
            for (int i = 1; i < num_cores; ++i)
            {
                cpu_list += "," + std::to_string(cpus_to_use[i]);
            }

            // Create modified args with taskset prepended
            CommandArgs taskset_args = args;
            taskset_args.program = "taskset";
            taskset_args.program_args.clear();
            taskset_args.program_args.push_back("-c");
            taskset_args.program_args.push_back(cpu_list);
            taskset_args.program_args.push_back(args.program);
            taskset_args.program_args.insert(
                taskset_args.program_args.end(),
                args.program_args.begin(),
                args.program_args.end());

            create_child_process(taskset_args);
            // Annotate generated JSON files with the number of logical CPUs used
            inject_json_kv(optkit::utils::EXECUTION_FOLDER_NAME, "cores_used", std::to_string(num_cores));

            optkit::utils::EXECUTION_FOLDER_NAME = {optkit::utils::get_date() + "__" + optkit::utils::get_time() + "__" + optkit::utils::generateGUID().substr(0, CONF_LOG_PRINT_GUID_LENGTH)};
            optkit::utils::create_directory(optkit::utils::EXECUTION_FOLDER_NAME);

            // Small delay between runs
            usleep(500000); // 0.5 second
        }
        if (num_cores > total_cores && num_cores / 2.0 != total_cores)
        {
            std::cout << "\n"
                      << std::string(60, '-') << "\n";
            std::cout << "Running with " << total_cores << " core(s)\n";
            std::cout << std::string(60, '-') << "\n";

            // Build CPU mask using all selected CPUs
            std::string cpu_list;
            for (int i = 0; i < total_cores; ++i)
            {
                if (!cpu_list.empty())
                    cpu_list += ",";
                cpu_list += std::to_string(cpus_to_use[i]);
            }

            // Create modified args with taskset prepended
            CommandArgs taskset_args = args;
            taskset_args.program = "taskset";
            taskset_args.program_args.clear();
            taskset_args.program_args.push_back("-c");
            taskset_args.program_args.push_back(cpu_list);
            taskset_args.program_args.push_back(args.program);
            taskset_args.program_args.insert(
                taskset_args.program_args.end(),
                args.program_args.begin(),
                args.program_args.end());

            create_child_process(taskset_args);
            // Give a brief moment for files to be materialized before annotation
            usleep(100000); // 0.1 second

            // Annotate JSONs for the final run using all selected cores
            inject_json_kv(optkit::utils::EXECUTION_FOLDER_NAME, "cores_used", std::to_string(num_cores));
        }
        else
        {
            optkit::utils::remove_directory(optkit::utils::EXECUTION_FOLDER_NAME);
        }
        std::cout << "\n"
                  << std::string(60, '=') << "\n";
        std::cout << "Core scaling analysis complete\n";
        break;
    }

    case BenchType::DEFAULT:
    {
        // IMPORTANT: OPTKIT_INIT declares a local variable. If constructed inside an if/else block,
        // it will be destroyed at the end of that block, and profiling code below may run after
        // OPTKIT teardown (can cause a crash).
        std::string execution_folder;
        if (args.is_screenshot)
            execution_folder = args.program + "__" + optkit::utils::EXECUTION_FOLDER_NAME + "__screenshot";
        optkit::OPTKIT optkit{optkit::OPTKIT_CONFIG(args.dump_to_file, execution_folder)};
        std::cout << "\n[Will execute program once and collect metrics]\n";
        create_child_process(args);
        break;
    }
    default:
        std::cerr << "Error: Unknown benchmark type\n";
        break;
    }
}