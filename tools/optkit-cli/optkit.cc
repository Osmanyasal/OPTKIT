#include <iostream>
#include "utils.hh"

int32_t main(int32_t argc, char **argv)
{
    CommandArgs args = parse_arguments(argc, argv);
    if (args.bench_type == BenchType::DEFAULT)
        OPTKIT_INIT({false});
    else
        OPTKIT_INIT({true});
    execute_command(args);
    return 0;
}