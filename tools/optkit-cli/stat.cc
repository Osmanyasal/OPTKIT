
#include <csignal>
#include <cstdint>
#include "utils.hh"

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
        while (IS_RUNNING)
        {
            pid_t result = waitpid(CHILD_PID, &status, WNOHANG);
            if (OPT_LIKELY(result == 0)) // Child is still running
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time).count() / 1000.0f;
                if (elapsed >= args.sampling_period_ms) // Sample here.
                {
                    std::cout << "sampling...\n";
                    begin_time = end_time;
                }
                // sleep 100ms to avoid busy waiting
                usleep(100000); // 0.1 sec
            }
            else if (OPT_UNLIKELY(result == -1)) // Error
                std::cerr << "waitpid error: " << strerror(errno) << std::endl;
            else
            { // child is finished!
                IS_RUNNING = false;
                CHILD_PID = -1;
            }
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

    // Determine if this is single-shot profiling or benchmark
    bool is_benchmark = (args.bench_type != BenchType::DEFAULT);

    setup_signal_handlers();

    if (is_benchmark)
    {
        std::cout << "\n[Will execute program multiple times with varying configurations]\n";
        std::cout << "Running benchmark: ";

        // Get topology to determine socket configuration
        const auto &socket_cpus = optkit::Query::detect_cpu_packages();

        switch (args.bench_type)
        {
        case BenchType::FREQ_SCALING:
        {
            std::cout << "Frequency Scaling Analysis (multiple executions)\n";
            std::unordered_map<uint32_t, std::string> socket_curr_governor;
            for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
                socket_curr_governor[socket] = optkit::frequency::cpu::Query::get_scaling_governor(socket_cpus.at(socket)[0]);

            auto avail_core_freqs = optkit::frequency::cpu::Query::get_scaling_available_core_frequencies(0);     // assuming all sockets are the same and have same available frequencies
            auto avail_uncore_freqs = optkit::frequency::cpu::Query::get_scaling_available_uncore_frequencies(0); // assuming all sockets are the same and have same available frequencies
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
                        optkit::frequency::cpu::Frequency::set_uncore_frequency(uncore_freq, socket);
                    }
                    // Create modified args with frequency settings
                    create_child_process(args);

                    // Small delay between runs
                    usleep(500000); // 0.5 second
                }

            for (size_t socket = 0; socket < OPTKIT_ENV_CPU_NUM_SOCKETS; socket++)
            {
                optkit::frequency::cpu::Query::set_scaling_governor(socket_curr_governor.at(socket), socket);
                optkit::frequency::cpu::Frequency::reset_core_frequency(core_freq, socket);
                optkit::frequency::cpu::Frequency::reset_uncore_frequency(socket);
            }
            break;
        }

        case BenchType::CORE_SCALING:
        {
            std::cout << "Core Scaling Analysis (multiple executions)\n";
            // Get number of processors
            if (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS <= 0)
            {
                std::cerr << "Error: Could not determine number of processors\n";
                return;
            }

            std::cout << "System has " << OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS << " processors across " << socket_cpus.size() << " socket(s)\n";
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

                // Small delay between runs
                usleep(500000); // 0.5 second
            }
            if (num_cores > total_cores)
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
            }
            std::cout << "\n"
                      << std::string(60, '=') << "\n";
            std::cout << "Core scaling analysis complete\n";
            break;
        }

        case BenchType::AFFINITY:
        {
            std::cout << "Affinity Analysis (multiple executions, strategy: ";
            switch (args.affinity_strategy)
            {
            case AffinityStrategy::COMPACT:
                std::cout << "compact";
                break;
            case AffinityStrategy::SCATTER:
                std::cout << "scatter";
                break;
            case AffinityStrategy::NUMA:
                std::cout << "numa";
                break;
            case AffinityStrategy::MANUAL:
                std::cout << "manual";
                break;
            }
            std::cout << ")\n";
            break;
        }
        default:
            break;
        }
    }
    else
    {
        std::cout << "\n[Will execute program once and collect metrics]\n";
        create_child_process(args);
    }
}
