#include "utils.hh"

void execute_list_command(const CommandArgs &args)
{
    std::cout << "Listing " << to_string(args.list_type);

    if (args.target != Target::ALL)
    {
        std::cout << " for " << (args.target == Target::CPU ? "CPU" : "GPU");
    }
    std::cout << "\n";

    // TODO: Implement list functionality
    switch (args.list_type)
    {
    case ListType::ALL:
        std::cout << "  - Available PMU events\n";
        std::cout << "  - Available metrics\n";
        break;
    case ListType::EVENTS:
        std::cout << "  - Available PMU events\n";
        break;
    case ListType::METRICS:
        std::cout << "  - Available metrics\n";
        break;
    default:
        std::cerr << "Error: Unknown list type\n";
        break;
    }
}