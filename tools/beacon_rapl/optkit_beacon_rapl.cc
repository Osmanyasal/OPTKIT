#include <iostream>
#include <optkit.hh>

int32_t main(int32_t argc, char **argv)
{
    // OptimizerKit optkit;
    OPTKIT_INIT();

    int value = argc > 1 ? std::atoi(argv[1]) : -1;
    OPTKIT_INFO("Monitoring socket energy for {} sec. (-1 means indefinite)", value);

    OPTKIT_RAPL(main_block, "main_block");
    // RaplProfiler main_block{"main_block", "rapl",{RaplReadMethods::PERF, (int)RaplDomain::PACKAGE | (int) RaplDomain::PP0}};
    if (value == -1)
        while (true)
        {
            // first read's usually 0 because it executes faster than rapl update (each 1ms)
            std::cout << main_block.read_val() << "\n";
            sleep(1);
        }
    else
    {
        while (value-- >= 0)
        {
            std::cout << main_block.read().second << "\n";
            sleep(1);
        }
    }

    return 0;
}