#include <omp.h>
#include <cmath>
#include "optkit.hh"

__attribute__((noinline)) void deep_math_function()
{
    volatile double x = 0;
    // NOTE: Increased loop count to 500 Million to ensure we catch samples!
    for (long i = 0; i < 500000000; i++)
    {
        x += 1.0;
    }
}

__attribute__((noinline)) void test_me_bitch()
{
    volatile double x = 0;
    // NOTE: Increased loop count to 500 Million to ensure we catch samples!
    for (long i = 0; i < 500000000; i++)
    {
        x += 1.0;
    }
    deep_math_function();

    // Prevent tail-call optimization so `test_me_bitch` stays in the call stack.
    x += 0.0;
}
int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    OPTKIT_CALLSTACK_PROFILER("optkit");
    deep_math_function();
    test_me_bitch();
    // #pragma omp parallel
    // {
    // No manual registration needed. Sampler catches us via inherit=1.
    // openmp_worker_task(omp_get_thread_num());
    // }
    return 0;
}
