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
        {"msrmod", Command::MSRMOD},
        {"train", Command::TRAIN},
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
    else if (separator_it == tokens.end() && args.command == Command::STAT)
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

            if(token == "-ss" || token == "--screenshot")
            {
                args.is_screenshot = true;
            }
            else if (token == "--all")
            {
                args.enable_disk = true;
                args.enable_gpu_events = true;
                args.enable_gpu_energy = true;
                args.enable_cpu_energy = true;
            }
            else if (token == "--disk")
            {
                args.enable_disk = true;
            }
            else if (token == "--gpu-events")
            {
                args.enable_gpu_events = true;
            }
            else if (token == "--gpu-energy")
            {
                args.enable_gpu_energy = true;
            }
            else if (token == "--cpu-energy")
            {
                args.enable_cpu_energy = true;
            }
            else if (token == "--energy")
            {
                args.enable_cpu_energy = true;
                args.enable_gpu_energy = true;
            }
            else if (token == "-a")
            {
                args.system_wide = true;
            }
            else if (token == "-S")
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
            else if (token == "-e" || token == "--event")
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
            else if (token == "--freq-limit")
            {
                if (i + 1 < separator_pos)
                {
                    uint32_t v = 0;
                    if (!parse_u32(tokens[i + 1], v))
                    {
                        args.parse_error = true;
                        args.parse_error_message = "Invalid value for --freq-limit: '" + tokens[i + 1] + "'";
                        args.command = Command::HELP;
                        break;
                    }
                    args.freq_limit = v;
                    ++i;
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for --freq-limit";
                    args.command = Command::HELP;
                    break;
                }
            }
            else if (token == "--freq-start")
            {
                if (i + 1 < separator_pos)
                {
                    uint32_t v = 0;
                    if (!parse_u32(tokens[i + 1], v))
                    {
                        args.parse_error = true;
                        args.parse_error_message = "Invalid value for --freq-start: '" + tokens[i + 1] + "'";
                        args.command = Command::HELP;
                        break;
                    }
                    args.freq_start = v;
                    ++i;
                }
                else
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for --freq-start";
                    args.command = Command::HELP;
                    break;
                }
            }
            else if (token == "-o" || token == "--output")
            {
                args.dump_to_file = true;
                if (i + 1 < separator_pos && !tokens[i + 1].empty() && tokens[i + 1][0] != '-')
                    ++i;
            }
            else
            {
                args.parse_error = true;
                args.parse_error_message = "Unknown stat option: '" + token + "'";
                args.command = Command::HELP;
                break;
            }
        }
        break;

    case Command::TRAIN:
        // Format: optkit train <folder>
        if (tokens.size() < 2)
        {
            args.parse_error = true;
            args.parse_error_message = "train requires a folder argument: optkit train <folder>";
            args.command = Command::HELP;
            break;
        }
        args.train_folder = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i)
            args.train_args.push_back(tokens[i]);
        break;

    case Command::REPORT:
        // Format: optkit report <folder_or_file> [more_folders_or_files...]
        // Historically this command is treated as a "folder" input; store the first path in args.program.
        if (tokens.size() < 2)
        {
            args.parse_error = true;
            args.parse_error_message = "report requires at least one path: optkit report <report_data_folder(s)>";
            args.command = Command::HELP;
            break;
        }
        args.program = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i)
            args.program_args.push_back(tokens[i]);
        break;

    case Command::MSRMOD:
    {
        bool have_value = false;
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            const std::string &token = tokens[i];
            if (token == "-r" || token == "--read")
            {
                args.msr_op = MsrOp::READ_OP;
            }
            else if (token == "-w" || token == "--write")
            {
                args.msr_op = MsrOp::WRITE_OP;
            }
            else if (token == "-c" || token == "--cpu")
            {
                if (i + 1 >= tokens.size())
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -c/--cpu";
                    args.command = Command::HELP;
                    break;
                }
                uint32_t v = 0;
                if (!parse_u32(tokens[++i], v))
                {
                    args.parse_error = true;
                    args.parse_error_message = "Invalid value for -c/--cpu: '" + tokens[i] + "'";
                    args.command = Command::HELP;
                    break;
                }
                args.msr_cpu = v;
            }
            else if (token == "-a" || token == "--addr" || token == "--address")
            {
                if (i + 1 >= tokens.size())
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -a/--addr";
                    args.command = Command::HELP;
                    break;
                }
                uint64_t v = 0;
                if (!parse_u64_base0(tokens[++i], v))
                {
                    args.parse_error = true;
                    args.parse_error_message = "Invalid value for -a/--addr: '" + tokens[i] + "'";
                    args.command = Command::HELP;
                    break;
                }
                args.msr_address = v;
            }
            else if (token == "-v" || token == "--value")
            {
                if (i + 1 >= tokens.size())
                {
                    args.parse_error = true;
                    args.parse_error_message = "Missing value for -v/--value";
                    args.command = Command::HELP;
                    break;
                }
                uint64_t v = 0;
                if (!parse_u64_base0(tokens[++i], v))
                {
                    args.parse_error = true;
                    args.parse_error_message = "Invalid value for -v/--value: '" + tokens[i] + "'";
                    args.command = Command::HELP;
                    break;
                }
                args.msr_value = v;
                have_value = true;
            }
            else if (token == "-h" || token == "--help")
            {
                args.command = Command::HELP;
                break;
            }
            else
            {
                args.parse_error = true;
                args.parse_error_message = "Unknown msrmod option: '" + token + "'";
                args.command = Command::HELP;
                break;
            }
        }

        if (!args.parse_error && args.command == Command::MSRMOD)
        {
            if (args.msr_op == MsrOp::NONE)
            {
                args.parse_error = true;
                args.parse_error_message = "msrmod requires -r/--read or -w/--write";
                args.command = Command::HELP;
            }
            else if (args.msr_cpu == static_cast<uint32_t>(-1))
            {
                args.parse_error = true;
                args.parse_error_message = "msrmod requires -c/--cpu";
                args.command = Command::HELP;
            }
            else if (args.msr_address == static_cast<uint64_t>(-1))
            {
                args.parse_error = true;
                args.parse_error_message = "msrmod requires -a/--addr";
                args.command = Command::HELP;
            }
            else if (args.msr_op == MsrOp::WRITE_OP && !have_value)
            {
                args.parse_error = true;
                args.parse_error_message = "msrmod write requires -v/--value";
                args.command = Command::HELP;
            }
        }
        break;
    }

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

    // if (cli_debug_enabled())
    // {
    std::cout << "Parsed Arguments:\n"
              << to_string(args) << "\n\n";
    // }
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

    case Command::TRAIN:
    {
        execute_train_command(args);
        break;
    }

    case Command::MSRMOD:
    {
        execute_msrmod_command(args);
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