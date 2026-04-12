#include <cstdio>
#include "optkit.hh"

extern "C" void run_workload();

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    {
        OPTKIT_GPU_EVENTS("gpm_workload", optkit::metrics::performance::gpu_metrics::all_metrics());
        run_workload();
    }
    return 0;
}
