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
    optkit <COMMAND> [OPTIONS] [-o | --output <FILE>] [-- <PROGRAM>]

COMMAND:
    topology [cpu|gpu]              Show system topology
    list [cpu|gpu] <TYPE>            List available components
    stat [OPTIONS] -- <PROGRAM>     Run single-shot profiling (like perf stat)
    msrmod [OPTIONS]                Read/write MSRs via /dev/cpu/<cpu>/msr_safe
    train <FOLDER>                  Train a frequency model from screenshot JSON datasets

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
    
    optkit stat -- <program>                                      Default profiling
    optkit stat -e <event> -- <program>                           Profile specific event
    optkit stat -m <metric> -- <program>                          Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>               Profile event + metric
    optkit stat -e <event> -m <metric> -S 0 -- <program>          Profile event + metric + socket 0
    optkit stat -a -e <event> -m <metric> -- <program>            Profile system-wide across all CPUs
    optkit stat -a -e <event> -m <metric> -ss -- <program>        Profile system-wide across all CPUs with screenshot tracing

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench freq-scaling --freq-limit 10 -- <program>   Frequency scaling (cap to at most 10 core freqs and 10 uncore freqs)
    optkit stat --bench freq-scaling --freq-start 2 --freq-limit 5 -- <program>   Frequency scaling (skip top 2, run next 5 freqs)
    optkit stat --bench core-scaling -- <program>            Core scaling analysis 

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program>

VISUALISING REPORT (--report):
    Generate visual report from collected profiling data

    optkit report <report_data_folder(s)>

TRAINING (train):
    Train a model from screenshot JSON datasets produced by `optkit stat -ss`.

    optkit train <folder> [TRAIN_OPTS...]

    TRAIN_OPTS (forwarded to the Python trainer):
        --epochs N           (default: 30)
        --batch-size N       (default: 256)
        --lr LR              (default: 1e-3)
        --hidden-size N      (default: 32)
        --num-layers N       (default: 1)
        --window W           (default: 0 = auto)
        --opset N            (default: 17)
        --device cpu|cuda    (default: cpu)


MSRMOD:
    Read/write a Model-Specific Register (MSR) using msr-safe (msr_safe device).

        optkit msrmod -r -c <cpu> -a <msr_addr>
        optkit msrmod -w -c <cpu> -a <msr_addr> -v <value>

        Notes:
            - Requires msr-safe (and its `msr_safe` devices) to be available.
            - Requires sufficient permissions.
 
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
    MSRMOD,
    TRAIN,
    HELP,
    REPORT,
    UNKNOWN
};

enum class MsrOp
{
    NONE,
    READ,
    WRITE
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
    bool dump_to_file = false;
    bool system_wide = false;                      // System-wide monitoring (-a) for stat
    bool is_screenshot = false;                    // Whether to capture screenshot of events
    uint32_t freq_limit = 0;                       // For --bench freq-scaling: cap available core/uncore frequency list sizes (0 = no limit)
    uint32_t freq_start = 0;                       // For --bench freq-scaling: starting index from top (highest frequency) (0 = start at top)
    std::string parse_error_message;
    std::vector<std::string> events;                // PMU events to profile (-e)
    std::vector<std::string> metrics;               // Metrics to collect (-m)
    std::string program;                            // Program to execute
    std::vector<std::string> program_args;          // Arguments for the program
    uint32_t socket_id = static_cast<uint32_t>(-1); // Socket ID (-1 means all sockets, 0+ means specific socket)

    // msrmod command options
    MsrOp msr_op = MsrOp::NONE;
    uint32_t msr_cpu = static_cast<uint32_t>(-1);
    uint64_t msr_address = static_cast<uint64_t>(-1);
    uint64_t msr_value = 0;

    // train command options
    std::string train_folder;
    std::vector<std::string> train_args;
};

// Parse command line arguments
CommandArgs parse_arguments(int argc, char **argv);

// Execute commands
void execute_command(const CommandArgs &args);
void execute_topology_command(const CommandArgs &args);
void execute_list_command(const CommandArgs &args);
void execute_stat_command(const CommandArgs &args);
void execute_report_command(const CommandArgs &args);
void execute_msrmod_command(const CommandArgs &args);
void execute_train_command(const CommandArgs &args);

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
    case Command::MSRMOD:
        return "MSRMOD";
    case Command::TRAIN:
        return "TRAIN";
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
    oss << "  system_wide: " << (args.system_wide ? "true" : "false") << "\n";
    oss << "  is_screenshot: " << (args.is_screenshot ? "true" : "false") << "\n";
    oss << "  freq_limit: " << args.freq_limit << "\n";
    oss << "  freq_start: " << args.freq_start << "\n";

    if (args.command == Command::TRAIN)
    {
        oss << "  train_folder: " << (args.train_folder.empty() ? "<none>" : args.train_folder) << "\n";
        oss << "  train_args: [";
        for (size_t i = 0; i < args.train_args.size(); ++i)
        {
            oss << "\"" << args.train_args[i] << "\"";
            if (i + 1 < args.train_args.size())
                oss << ", ";
        }
        oss << "]\n";
    }

    if (args.command == Command::MSRMOD)
    {
        oss << "  msr_op: " << (args.msr_op == MsrOp::READ ? "READ" : (args.msr_op == MsrOp::WRITE ? "WRITE" : "NONE")) << "\n";
        oss << "  msr_cpu: " << args.msr_cpu << "\n";
        oss << "  msr_address: 0x" << std::hex << args.msr_address << std::dec << "\n";
        if (args.msr_op == MsrOp::WRITE)
            oss << "  msr_value: 0x" << std::hex << args.msr_value << std::dec << "\n";
    }


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

inline bool parse_u64_base0(const std::string &s, uint64_t &out)
{
    try
    {
        unsigned long long v = std::stoull(s, nullptr, 0); // base 0: accepts 0x.. hex
        out = static_cast<uint64_t>(v);
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}