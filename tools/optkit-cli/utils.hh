#pragma once

#include <string>
#include <vector>

enum class Command
{
    TOPOLOGY,
    LIST,
    BENCH,
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

struct CommandArgs
{
    Command command = Command::UNKNOWN;
    Target target = Target::ALL;
    BenchType bench_type = BenchType::DEFAULT;
    AffinityStrategy affinity_strategy = AffinityStrategy::COMPACT;
    std::string list_type;
    std::vector<std::string> program_args;
};

// Parse command line arguments
CommandArgs parse_arguments(int argc, char **argv);

// Execute commands
void execute_command(const CommandArgs &args);
void print_help();
