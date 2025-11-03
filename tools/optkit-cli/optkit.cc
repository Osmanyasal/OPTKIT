#include <iostream>
#include <memory>
#include "utils.hh"

int main(int argc, char **argv)
{
    CommandArgs args = parse_arguments(argc, argv);
    execute_command(args);

    return EXIT_SUCCESS;
}
