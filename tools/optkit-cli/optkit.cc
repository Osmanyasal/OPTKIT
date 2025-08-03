#include <iostream>
#include "optkit.hh"
#include "utils.hh"

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});

    // Parse command line arguments
    auto args = parse_arguments(argc, argv);

    // Execute the parsed command
    execute_command(args);

    return 0;
}