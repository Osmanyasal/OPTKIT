#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "utils.hh"

void print_help()
{
    std::cout << R"(
OPTKIT - Performance and Energy Profiling Tool

USAGE:
    optkit <COMMAND> [OPTIONS] [-- <PROGRAM>]

COMMANDS:
    topology [cpu|gpu]              Show system topology
    list <TYPE> [cpu|gpu]           List available components
    bench [TYPE] [OPTIONS] -- <PROGRAM>  Run benchmark

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology
    optkit topology gpu             Show GPU topology

LIST:
    optkit list pmu                 List PMU capabilities
    optkit list events [cpu|gpu]    List available PMU events
    optkit list metrics [cpu|gpu]   List available metrics

BENCHMARK:
    optkit bench -- <program>                              Default benchmark
    optkit bench freq-scaling -- <program>                 Frequency scaling analysis
    optkit bench core-scaling -- <program>                 Core scaling analysis
    optkit bench affinity [--strategy=STRATEGY] -- <program>  Affinity analysis

AFFINITY STRATEGIES:
    --strategy=compact              Pack threads on fewer cores
    --strategy=scatter              Spread threads across cores
    --strategy=numa                 NUMA-aware placement
    --strategy=manual               Manual affinity control

EXAMPLES:
    optkit topology cpu
    optkit list events cpu
    optkit bench -- ./my_program
    optkit bench freq-scaling -- ./benchmark
    optkit bench affinity --strategy=scatter -- ./parallel_app

)";
}

Command parse_command(const std::string &cmd)
{
    static const std::unordered_map<std::string, Command> command_map = {
        {"topology", Command::TOPOLOGY},
        {"list", Command::LIST},
        {"bench", Command::BENCH},
        {"help", Command::HELP},
        {"--help", Command::HELP},
        {"-h", Command::HELP}};

    auto it = command_map.find(cmd);
    return (it != command_map.end()) ? it->second : Command::UNKNOWN;
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

    // Convert to string vector
    for (int i = 1; i < argc; ++i)
    {
        tokens.emplace_back(argv[i]);
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

    // Extract program arguments
    if (separator_it != tokens.end())
    {
        args.program_args.assign(separator_it + 1, tokens.end());
    }

    // Process command-specific arguments
    switch (args.command)
    {
    case Command::TOPOLOGY:
        if (separator_pos > 1)
        {
            args.target = parse_target(tokens[1]);
        }
        break;

    case Command::LIST:
        if (separator_pos > 1)
        {
            args.list_type = tokens[1];
            if (separator_pos > 2)
            {
                args.target = parse_target(tokens[2]);
            }
        }
        break;

    case Command::BENCH:
        // Parse benchmark type
        if (separator_pos > 1)
        {
            args.bench_type = parse_bench_type(tokens[1]);
        }

        // Parse affinity strategy if present
        for (size_t i = 1; i < separator_pos; ++i)
        {
            if (tokens[i].find("--strategy=") == 0)
            {
                std::string strategy = tokens[i].substr(11); // Skip "--strategy="
                args.affinity_strategy = parse_affinity_strategy(strategy);
            }
        }
        break;

    default:
        break;
    }

    return args;
}

void execute_topology_command(const CommandArgs &args)
{
    std::cout << "Executing topology command for ";
    switch (args.target)
    {
    case Target::CPU:
        std::cout << "CPU\n";
        // TODO: Call OPTKIT CPU topology function
        break;
    case Target::GPU:
        std::cout << "GPU\n";
        // TODO: Call OPTKIT GPU topology function
        break;
    case Target::ALL:
    default:
        std::cout << "ALL devices\n";
        // TODO: Call OPTKIT complete topology function
        break;
    }
}

void execute_list_command(const CommandArgs &args)
{
    std::cout << "Listing " << args.list_type;

    if (args.target != Target::ALL)
    {
        std::cout << " for " << (args.target == Target::CPU ? "CPU" : "GPU");
    }
    std::cout << "\n";

    // TODO: Implement list functionality
    if (args.list_type == "pmu")
    {
        std::cout << "  - PMU capabilities\n";
    }
    else if (args.list_type == "events")
    {
        std::cout << "  - Available PMU events\n";
    }
    else if (args.list_type == "metrics")
    {
        std::cout << "  - Available metrics\n";
    }
}

void execute_bench_command(const CommandArgs &args)
{
    if (args.program_args.empty())
    {
        std::cerr << "Error: No program specified for benchmark\n";
        std::cerr << "Usage: optkit bench [TYPE] -- <program> [args...]\n";
        return;
    }

    std::cout << "Running benchmark: ";
    switch (args.bench_type)
    {
    case BenchType::FREQ_SCALING:
        std::cout << "Frequency Scaling Analysis\n";
        break;
    case BenchType::CORE_SCALING:
        std::cout << "Core Scaling Analysis\n";
        break;
    case BenchType::AFFINITY:
        std::cout << "Affinity Analysis (strategy: ";
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
    case BenchType::DEFAULT:
    default:
        std::cout << "Default Benchmark\n";
        break;
    }

    std::cout << "Program: ";
    for (const auto &arg : args.program_args)
    {
        std::cout << arg << " ";
    }
    std::cout << "\n";

    // TODO: Execute benchmark with OPTKIT
}

void execute_command(const CommandArgs &args)
{
    switch (args.command)
    {
    case Command::TOPOLOGY:
        execute_topology_command(args);
        break;

    case Command::LIST:
        if (args.list_type.empty())
        {
            std::cerr << "Error: Missing list type\n";
            std::cerr << "Usage: optkit list <TYPE> [cpu|gpu]\n";
            return;
        }
        execute_list_command(args);
        break;

    case Command::BENCH:
        execute_bench_command(args);
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