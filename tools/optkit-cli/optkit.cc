#include <iostream>
#include "optkit.hh"
#include "utils.hh"

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});

    auto args = parse_arguments(argc, argv);
    execute_command(args);

    return 0;
}