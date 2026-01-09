#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    optkit list [cpu|gpu] pmu                   List available PMU info
    optkit list [cpu|gpu|disk|memory]           List available events + metrics
    optkit list [cpu|gpu|disk|memory] events    List available events
    optkit list [cpu|gpu|disk|memory] metrics   List available metrics

PROFILING (stat):
    Single execution profiling - runs program once and collects metrics
    
    optkit stat -- <program>                                                          Default profiling
    optkit stat -e <event> -- <program>                                               Profile specific event
    optkit stat -m <metric> -- <program>                                              Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>                                   Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -- <program>        Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -S 0 -- <program>   Profile event + metric + socket 0

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench core-scaling -- <program>            Core scaling analysis 

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program>

VISUALISING REPORT (--report):
    Generate visual report from collected profiling data

    optkit report -- <report_data_folder(s)>

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

    # Interleaved options (benchmark + specific profiling)
    optkit stat --bench freq-scaling -e cycles -m ipc -- ./program --input data.txt
    optkit stat -e cache-misses -m energy --bench core-scaling -- ./app 

NOTE:
    - 'stat' without --bench <...>: Single execution, collects specified events/metrics
    - 'stat' with --bench <...>: Multiple executions with varying configurations
      (e.g., different frequencies or core counts)

)";
}

enum class Command
{
    TOPOLOGY,
    LIST,
    STAT,
    HELP,
    REPORT,
    UNKNOWN
};

enum class Target
{
    CPU,
    GPU,
    DISK,
    MEMORY,
    ALL
};

enum class BenchType
{
    DEFAULT,
    FREQ_SCALING,
    CORE_SCALING
};

enum class ListType
{
    ALL,
    EVENTS,
    METRICS,
    PMU
};

struct CommandArgs
{
    Command command = Command::UNKNOWN;
    Target target = Target::ALL;
    BenchType bench_type = BenchType::DEFAULT;
    ListType list_type = ListType::ALL;
    bool parse_error = false;
    std::string parse_error_message;
    std::vector<std::string> events;                // PMU events to profile (-e)
    std::vector<std::string> metrics;               // Metrics to collect (-m)
    std::string program;                            // Program to execute
    std::vector<std::string> program_args;          // Arguments for the program
    uint32_t socket_id = static_cast<uint32_t>(-1); // Socket ID (-1 means all sockets, 0+ means specific socket)
    uint32_t sampling_period_ms = 1000;             // Sampling period in milliseconds (1second) for stat command
};

// Parse command line arguments
CommandArgs parse_arguments(int argc, char **argv);

// Execute commands
void execute_command(const CommandArgs &args);
void execute_topology_command(const CommandArgs &args);
void execute_list_command(const CommandArgs &args);
void execute_stat_command(const CommandArgs &args);
void execute_report_command(const CommandArgs &args);

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
    case Command::REPORT:
        return "REPORT";
    case Command::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNKNOWN";
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
    case Target::DISK:
        return "DISK";
    case Target::MEMORY:
        return "MEMORY";
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
    case ListType::PMU:
        return "PMU";
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

inline bool report_debug_enabled()
{
    const char *v = std::getenv("OPTKIT_CLI_DEBUG");
    return (v != nullptr) && (v[0] == '1' || v[0] == 'y' || v[0] == 'Y' || v[0] == 't' || v[0] == 'T');
}

inline bool run_system_checked(const std::string &cmd, const std::string &what)
{
    int ret = std::system(cmd.c_str());
    if (ret != 0)
    {
        std::cerr << "Error: failed to run " << what << " (exit=" << ret << ")\n";
        if (report_debug_enabled())
            std::cerr << "Command: " << cmd << "\n";
        return false;
    }
    return true;
}

inline bool cli_debug_enabled()
{
    const char *v = std::getenv("OPTKIT_CLI_DEBUG");
    return (v != nullptr) && (v[0] == '1' || v[0] == 'y' || v[0] == 'Y' || v[0] == 't' || v[0] == 'T');
}

inline bool parse_u32(const std::string &s, uint32_t &out)
{
    try
    {
        unsigned long v = std::stoul(s);
        out = static_cast<uint32_t>(v);
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}