#include <iostream>
#include "optkit.hh"

int main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    std::cout << "OPTKIT initialized. Run tests with: ./test/optkit_test\n";
    return 0;
}
