#include <omp.h>
#include "optkit.hh"

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();

    optkit::core::frequency::CPUFrequency::reset_core_frequency(0);

    return 0;
}
