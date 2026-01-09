#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include "utils.hh"

Command parse_command(const std::string &cmd)
{
    static const std::unordered_map<std::string, Command> command_map = {
        {"topology", Command::TOPOLOGY},
        {"list", Command::LIST},
        {"stat", Command::STAT},
        {"report", Command::REPORT},
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
        {"metrics", ListType::METRICS},
        {"pmu", ListType::PMU},
    };

    auto it = list_map.find(type);
    return (it != list_map.end()) ? it->second : ListType::ALL;
}

Target parse_target(const std::string &target)
{
    static const std::unordered_map<std::string, Target> target_map = {
        {"cpu", Target::CPU},
        {"gpu", Target::GPU},
        {"disk", Target::DISK},
        {"memory", Target::MEMORY},
        {"all", Target::ALL}};

    auto it = target_map.find(target);
    return (it != target_map.end()) ? it->second : Target::ALL;
}

BenchType parse_bench_type(const std::string &type)
{
    static const std::unordered_map<std::string, BenchType> bench_map = {
        {"freq-scaling", BenchType::FREQ_SCALING},
        {"core-scaling", BenchType::CORE_SCALING}};

    auto it = bench_map.find(type);
    return (it != bench_map.end()) ? it->second : BenchType::DEFAULT;
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
    size_t separator_pos = (separator_it != tokens.end()) ? std::distance(tokens.begin(), separator_it) : 0;

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
    else if (separator_it == tokens.end())
    {
        for (size_t i = 1; i < tokens.size(); i++)
            args.program_args.push_back(tokens[i]);
    }

    // Process command-specific arguments
    switch (args.command)
    {
    case Command::TOPOLOGY:
        if (tokens.size() > 1)
            args.target = parse_target(tokens[1]);
        break;

    case Command::LIST:
        if (tokens.size() == 2)
        {
            args.target = parse_target(tokens[1]);
            args.list_type = ListType::ALL;
        }
        if (tokens.size() == 3)
        {
            args.target = parse_target(tokens[1]);
            args.list_type = parse_list_type(tokens[2]);
        }
        break;
    case Command::STAT:
        if (separator_it == tokens.end())
        {
            args.parse_error = true;
            args.parse_error_message = "stat requires '-- <program>'";
            args.command = Command::HELP;
            break;
        }
        // Parse all options before the separator
        for (size_t i = 1; i < separator_pos; ++i)
        {
            const std::string &token = tokens[i];
            if (token == "-T")
            {
                // Next token should be the sampling period in milliseconds
                if (i + 1 < separator_pos)
                {
                    uint32_t v = 0;
                    if (!parse_u32(tokens[i + 1], v))
                    {
                        args.parse_error = true;
                        args.parse_error_message = "Invalid value for -T: '" + tokens[i + 1] + "'";
                        args.command = Command::HELP;
                        break;
                    }
                    args.sampling_period_ms = v;
                    ++i;
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -T";
                    args.command = Command::HELP;
                    break;
                }
            }
            if (token == "-S")
            {
                // Next token should be the socket ID
                if (i + 1 < separator_pos)
                {
                    uint32_t v = 0;
                    if (!parse_u32(tokens[i + 1], v))
                    {
                        args.parse_error = true;
                        args.parse_error_message = "Invalid value for -S: '" + tokens[i + 1] + "'";
                        args.command = Command::HELP;
                        break;
                    }
                    args.socket_id = v;
                    ++i;
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -S";
                    args.command = Command::HELP;
                    break;
                }
            }
            if (token == "-e" || token == "--event")
            {
                // Next token should be the event name
                if (i + 1 < separator_pos && tokens[i + 1][0] != '-')
                {
                    args.events.push_back(tokens[++i]);
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -e/--event";
                    args.command = Command::HELP;
                    break;
                }
            }
            else if (token == "-m" || token == "--metric")
            {
                // Next token should be the metric name
                if (i + 1 < separator_pos && tokens[i + 1][0] != '-')
                {
                    args.metrics.push_back(tokens[++i]);
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -m/--metric";
                    args.command = Command::HELP;
                    break;
                }
            }
            else if (token == "--bench")
            {
                // Format: --bench freq-scaling
                if (i + 1 < separator_pos)
                {
                    args.bench_type = parse_bench_type(tokens[++i]);
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for --bench";
                    args.command = Command::HELP;
                    break;
                }
            }
        }
        break;

    default:
        break;
    }

    return args;
}

void execute_command(const CommandArgs &args)
{
    if (args.parse_error)
    {
        std::cerr << "Error: " << args.parse_error_message << "\n\n";
        print_help();
        return;
    }

    if (cli_debug_enabled())
    {
        std::cout << "Parsed Arguments:\n"
                  << to_string(args) << "\n\n";
    }
    switch (args.command)
    {
    case Command::TOPOLOGY:
    {
        OPTKIT_INIT({false});
        execute_topology_command(args);
        break;
    }
    case Command::LIST:
    {
        OPTKIT_INIT({false});
        execute_list_command(args);
        break;
    }
    case Command::STAT:
    {
        execute_stat_command(args);
        break;
    }
    case Command::REPORT:
    {
        execute_report_command(args);
        break;
    }

    case Command::HELP:
    case Command::UNKNOWN:
        print_help();
        break;
    default:
        std::cerr << "Error: Unknown command\n";
        print_help();
        break;
    }
}