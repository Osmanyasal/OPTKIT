#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "utils.hh"

Command parse_command(const std::string &cmd)
{
    static const std::unordered_map<std::string, Command> command_map = {
        {"topology", Command::TOPOLOGY},
        {"list", Command::LIST},
        {"stat", Command::STAT},
        {"help", Command::HELP},
        {"--help", Command::HELP},
        {"-h", Command::HELP}};

    auto it = command_map.find(cmd);
    return (it != command_map.end()) ? it->second : Command::UNKNOWN;
}

ListType parse_list_type(const std::string &type)
{
    static const std::unordered_map<std::string, ListType> list_map = {
        {"all", ListType::ALL},
        {"events", ListType::EVENTS},
        {"metrics", ListType::METRICS}};

    auto it = list_map.find(type);
    return (it != list_map.end()) ? it->second : ListType::ALL;
}

Target parse_target(const std::string &target)
{
    static const std::unordered_map<std::string, Target> target_map = {
        {"cpu", Target::CPU},
        {"gpu", Target::GPU}};

    auto it = target_map.find(target);
    return (it != target_map.end()) ? it->second : Target::ALL;
}

BenchType parse_bench_type(const std::string &type)
{
    static const std::unordered_map<std::string, BenchType> bench_map = {
        {"freq-scaling", BenchType::FREQ_SCALING},
        {"core-scaling", BenchType::CORE_SCALING},
        {"affinity", BenchType::AFFINITY}};

    auto it = bench_map.find(type);
    return (it != bench_map.end()) ? it->second : BenchType::DEFAULT;
}

AffinityStrategy parse_affinity_strategy(const std::string &strategy)
{
    static const std::unordered_map<std::string, AffinityStrategy> strategy_map = {
        {"compact", AffinityStrategy::COMPACT},
        {"scatter", AffinityStrategy::SCATTER},
        {"numa", AffinityStrategy::NUMA},
        {"manual", AffinityStrategy::MANUAL}};

    auto it = strategy_map.find(strategy);
    return (it != strategy_map.end()) ? it->second : AffinityStrategy::COMPACT;
}

CommandArgs parse_arguments(int argc, char **argv)
{
    CommandArgs args;
    std::vector<std::string> tokens;

    // Convert to string vector (skip argv[0] which is program name)
    for (int i = 1; i < argc; ++i)
    {
        tokens.push_back(argv[i]);
    }

    if (tokens.empty())
    {
        args.command = Command::HELP;
        return args;
    }

    // Parse main command
    args.command = parse_command(tokens[0]);

    // Find program separator "--"
    auto separator_it = std::find(tokens.begin(), tokens.end(), "--");
    size_t separator_pos = (separator_it != tokens.end()) ? std::distance(tokens.begin(), separator_it) : tokens.size();

    // Extract program and its arguments
    if (separator_it != tokens.end() && separator_it + 1 != tokens.end())
    {
        // First token after "--" is the program
        args.program = *(separator_it + 1);

        // Rest are program arguments
        for (auto it = separator_it + 2; it != tokens.end(); ++it)
        {
            args.program_args.push_back(*it);
        }
    }

    // Process command-specific arguments
    switch (args.command)
    {
    case Command::TOPOLOGY:
        if (tokens.size() > 1)
            args.target = parse_target(tokens[1]);
        break;

    case Command::LIST:
        if (tokens.size() > 1)
        {
            std::cout << "tokens[1]: " << tokens[1] << std::endl;
            args.target = parse_target(tokens[1]);
        }
        if (tokens.size() > 2)
        {
            std::cout << "tokens[2]: " << tokens[2] << std::endl;
            args.list_type = parse_list_type(tokens[2]);
        }
        break;

    case Command::STAT:
        // Parse all options before the separator
        for (size_t i = 1; i < separator_pos; ++i)
        {
            const std::string &token = tokens[i];

            if (token == "-e" || token == "--event")
            {
                // Next token should be the event name
                if (i + 1 < separator_pos && tokens[i + 1][0] != '-')
                {
                    args.events.push_back(tokens[++i]);
                }
            }
            else if (token == "-m" || token == "--metric")
            {
                // Next token should be the metric name
                if (i + 1 < separator_pos && tokens[i + 1][0] != '-')
                {
                    args.metrics.push_back(tokens[++i]);
                }
            }
            else if (token == "--bench")
            {
                // Format: --bench freq-scaling
                if (i + 1 < separator_pos)
                {
                    args.bench_type = parse_bench_type(tokens[++i]);
                }
            }
            else if (token == "--affinity")
            {
                // Format: --affinity compact
                if (i + 1 < separator_pos)
                {
                    args.affinity_strategy = parse_affinity_strategy(tokens[++i]);
                    args.bench_type = BenchType::AFFINITY;
                }
            }
        }
        break;

    default:
        break;
    }

    return args;
}

void execute_stat_command(const CommandArgs &args)
{
    if (args.program_args.empty())
    {
        std::cerr << "Error: No program specified for profiling\n";
        std::cerr << "Usage: optkit stat [OPTIONS] -- <program> [args...]\n";
        return;
    }

    // Determine if this is single-shot profiling or benchmark
    bool is_benchmark = (args.bench_type != BenchType::DEFAULT);

    if (is_benchmark)
    {
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
        std::cout << "Running single-shot profiling (like perf stat)\n";
    }

    // Display events if specified
    if (!args.events.empty())
    {
        std::cout << "Events: ";
        for (size_t i = 0; i < args.events.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << args.events[i];
        }
        std::cout << "\n";
    }

    // Display metrics if specified
    if (!args.metrics.empty())
    {
        std::cout << "Metrics: ";
        for (size_t i = 0; i < args.metrics.size(); ++i)
        {
            if (i > 0)
                std::cout << ", ";
            std::cout << args.metrics[i];
        }
        std::cout << "\n";
    }

    std::cout << "Program: ";
    for (const auto &arg : args.program_args)
    {
        std::cout << arg << " ";
    }
    std::cout << "\n";

    if (is_benchmark)
    {
        std::cout << "\n[Will execute program multiple times with varying configurations]\n";
    }
    else
    {
        std::cout << "\n[Will execute program once and collect metrics]\n";
    }

    // TODO: Execute profiling/benchmark with OPTKIT
}

void execute_command(const CommandArgs &args)
{
    std::cout << "Parsed Arguments:\n"
              << to_string(args) << "\n\n";
    switch (args.command)
    {
    case Command::TOPOLOGY:
        execute_topology_command(args);
        break;

    case Command::LIST:
        execute_list_command(args);
        break;

    case Command::STAT:
        execute_stat_command(args);
        break;

    case Command::HELP:
        print_help();
        break;

    case Command::UNKNOWN:
    default:
        std::cerr << "Error: Unknown command\n";
        print_help();
        break;
    }
}