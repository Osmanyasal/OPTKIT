
#include <csignal>
#include <cstdint>
#include "utils.hh"

bool IS_RUNNING = false;
int32_t CHILD_PID = -1;

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        // write(STDOUT_FILENO, "Interrupt signal (Ctrl+C) received\n", 36);
        if (CHILD_PID > 0)
        {
            // Forward signal to child process
            // write(STDOUT_FILENO, "Signal forwarded to the child process\n", 39);
            kill(CHILD_PID, SIGINT);
        }
        else
        {
            // write(STDOUT_FILENO, "Child not exist, terminating.\n", 31);
            IS_RUNNING = false;
            // _exit(0);
        }
    }
    else if (signal == SIGTERM)
    {
        // write(STDOUT_FILENO, "Termination signal received.\n", 30);
        if (CHILD_PID > 0)
        {
            // Forward signal to child process
            // write(STDOUT_FILENO, "Signal forwarded to the child process\n", 39);
            kill(CHILD_PID, SIGTERM);
        }
        else
        {
            // write(STDOUT_FILENO, "Child not exist, terminating.\n", 31);
            IS_RUNNING = false;
            // _exit(0);
        }
    }
}

void setup_signal_handlers()
{
    std::signal(SIGINT, signal_handler);  // Ctrl+C
    std::signal(SIGTERM, signal_handler); // `kill` default
}

void create_child_process(const std::string &program, const std::vector<std::string> &args)
{
    CHILD_PID = fork();
    if (CHILD_PID == 0)
    {
        std::vector<char *> c_args;
        c_args.push_back(const_cast<char *>(program.c_str()));
        for (const auto &arg : args)
            c_args.push_back(const_cast<char *>(arg.c_str()));
        c_args.push_back(nullptr);

        execvp(program.c_str(), c_args.data());

        perror("execvp failed");
        _exit(1);
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
        switch (args.bench_type)
        {
        case BenchType::FREQ_SCALING:
            std::cout << "Frequency Scaling Analysis (multiple executions)\n";
            break;
        case BenchType::CORE_SCALING:
            std::cout << "Core Scaling Analysis (multiple executions)\n";
            break;
        case BenchType::AFFINITY:
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
        default:
            break;
        }
    }
    else
    {
        std::cout << "\n[Will execute program once and collect metrics]\n";
        create_child_process(args.program, args.program_args);
    }
}
