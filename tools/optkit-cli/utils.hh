#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "optkit.hh"

inline void print_help()
{
    std::cout << R"(
OPTKIT - Performance and Energy Profiling & Optimization Tool

USAGE:
    optkit <COMMAND> [OPTIONS] [-- <PROGRAM>]

COMMANDS:
    topology [cpu|gpu]              Show system topology
    list <TYPE> [cpu|gpu]           List available components
    stat [OPTIONS] -- <PROGRAM>     Run single-shot profiling (like perf stat)

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology only
    optkit topology gpu             Show GPU topology only

LIST:
    optkit list [all|cpu|gpu]           List all PMU capabilities
    optkit list [all|cpu|gpu] events    List available PMU events
    optkit list [all|cpu|gpu] metrics   List available metrics

PROFILING (stat):
    Single execution profiling - runs program once and collects metrics
    
    optkit stat -- <program>                                 Default profiling
    optkit stat -e <event> -- <program>                      Profile specific event
    optkit stat -m <metric> -- <program>                     Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>          Profile event + metric

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench core-scaling -- <program>            Core scaling analysis
    optkit stat --affinity <STRATEGY> -- <program>           Affinity analysis

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program>

AFFINITY STRATEGIES:
    --affinity compact              Pack threads on fewer cores (cache locality)
    --affinity scatter              Spread threads across cores (avoid contention)
    --affinity numa                 NUMA-aware placement (memory locality)
    --affinity manual               Manual affinity control

EXAMPLES:
    # Topology queries
    optkit topology
    optkit topology cpu
    optkit topology gpu

    # List capabilities
    optkit list all
    optkit list cpu all
    optkit list cpu events
    optkit list cpu metrics
    optkit list gpu all
    optkit list gpu events
    optkit list gpu metrics

    # Single-shot profiling (executes once)
    optkit stat -- ./my_program
    optkit stat -e cycles -e instructions -- ./app
    optkit stat -m ipc -m cache-miss-rate -- ./benchmark

    # Benchmark analysis (executes multiple times)
    optkit stat --bench freq-scaling -- ./compute_heavy
    optkit stat --bench core-scaling -- ./parallel_app
    optkit stat --affinity scatter -- ./threaded_app
    optkit stat --affinity numa -- ./parallel_workload

    # Interleaved options (benchmark + specific profiling)
    optkit stat --bench freq-scaling -e cycles -m ipc -- ./program --input data.txt
    optkit stat -e cache-misses -m energy --bench core-scaling -- ./app
    optkit stat --affinity compact -e instructions -m ipc -- ./multithreaded

NOTE:
    - 'stat' without --bench or --affinity: Single execution, collects specified events/metrics
    - 'stat' with --bench or --affinity: Multiple executions with varying configurations
      (e.g., different frequencies, core counts, affinity patterns)

)";
}

enum class Command
{
    TOPOLOGY,
    LIST,
    STAT,
    HELP,
    UNKNOWN
};

enum class Target
{
    CPU,
    GPU,
    ALL
};

enum class BenchType
{
    DEFAULT,
    FREQ_SCALING,
    CORE_SCALING,
    AFFINITY
};

enum class AffinityStrategy
{
    COMPACT,
    SCATTER,
    NUMA,
    MANUAL
};

enum class ListType
{
    ALL,
    EVENTS,
    METRICS
};

struct CommandArgs
{
    Command command = Command::UNKNOWN;
    Target target = Target::ALL;
    BenchType bench_type = BenchType::DEFAULT;
    AffinityStrategy affinity_strategy = AffinityStrategy::COMPACT;
    ListType list_type = ListType::ALL;
    std::vector<std::string> events;       // PMU events to profile (-e)
    std::vector<std::string> metrics;      // Metrics to collect (-m)
    std::string program;                   // Program to execute
    std::vector<std::string> program_args; // Arguments for the program
};

// Parse command line arguments
CommandArgs parse_arguments(int argc, char **argv);

// Execute commands
void execute_command(const CommandArgs &args);
void execute_topology_command(const CommandArgs &args);
void execute_list_command(const CommandArgs &args);
void execute_stat_command(const CommandArgs &args);

// String conversion helpers
inline std::string to_string(Command cmd)
{
    switch (cmd)
    {
    case Command::TOPOLOGY:
        return "TOPOLOGY";
    case Command::LIST:
        return "LIST";
    case Command::STAT:
        return "STAT";
    case Command::HELP:
        return "HELP";
    case Command::UNKNOWN:
        return "UNKNOWN";
    default:
        return "INVALID";
    }
}

inline std::string to_string(Target target)
{
    switch (target)
    {
    case Target::CPU:
        return "CPU";
    case Target::GPU:
        return "GPU";
    case Target::ALL:
        return "ALL";
    default:
        return "INVALID";
    }
}

inline std::string to_string(BenchType bench)
{
    switch (bench)
    {
    case BenchType::DEFAULT:
        return "DEFAULT";
    case BenchType::FREQ_SCALING:
        return "FREQ_SCALING";
    case BenchType::CORE_SCALING:
        return "CORE_SCALING";
    case BenchType::AFFINITY:
        return "AFFINITY";
    default:
        return "INVALID";
    }
}

inline std::string to_string(AffinityStrategy affinity)
{
    switch (affinity)
    {
    case AffinityStrategy::COMPACT:
        return "COMPACT";
    case AffinityStrategy::SCATTER:
        return "SCATTER";
    case AffinityStrategy::NUMA:
        return "NUMA";
    case AffinityStrategy::MANUAL:
        return "MANUAL";
    default:
        return "INVALID";
    }
}

inline std::string to_string(ListType list_type)
{
    switch (list_type)
    {
    case ListType::ALL:
        return "ALL";
    case ListType::EVENTS:
        return "EVENTS";
    case ListType::METRICS:
        return "METRICS";
    default:
        return "UNKNOWN";
    }
}

inline std::string to_string(const CommandArgs &args)
{
    std::ostringstream oss;
    oss << "CommandArgs {\n";
    oss << "  command: " << to_string(args.command) << "\n";
    oss << "  target: " << to_string(args.target) << "\n";
    oss << "  bench_type: " << to_string(args.bench_type) << "\n";
    oss << "  affinity_strategy: " << to_string(args.affinity_strategy) << "\n";
    oss << "  list_type: " << to_string(args.list_type) << "\n";

    oss << "  events: [";
    for (size_t i = 0; i < args.events.size(); ++i)
    {
        if (i > 0)
            oss << ", ";
        oss << "\"" << args.events[i] << "\"";
    }
    oss << "]\n";

    oss << "  metrics: [";
    for (size_t i = 0; i < args.metrics.size(); ++i)
    {
        if (i > 0)
            oss << ", ";
        oss << "\"" << args.metrics[i] << "\"";
    }
    oss << "]\n";
    oss << "  program: " << (args.program.empty() ? "<none>" : args.program) << "\n";
    oss << "  program_args: [";
    for (size_t i = 0; i < args.program_args.size(); ++i)
    {
        if (i > 0)
            oss << ", ";
        oss << "\"" << args.program_args[i] << "\"";
    }
    oss << "]\n";
    oss << "}";

    return oss.str();
}

inline std::ostream &operator<<(std::ostream &os, const CommandArgs &args)
{
    os << to_string(args);
    return os;
}
