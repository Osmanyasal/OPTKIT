#include <iostream>
#include "utils.hh"

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});
    CommandArgs args = parse_arguments(argc, argv);
    execute_command(args);
    return 0;
}