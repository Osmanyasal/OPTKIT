#include <iostream>
#include <chrono>
#include <cstdint>
#include <unistd.h>

#include "optkit.hh"
#include "core/callstack/profiler.hh"
#include "core/pmu/cpu/perf/profiler_config.hh"
#include "utils/utils.hh"

// ============================================================================
// NESTED FUNCTION DEFINITIONS (Single-Threaded)
// ============================================================================

__attribute__((noinline)) void level_5()
{
    // Deepest level: 500M iterations
    volatile double x = 0;
    for (long i = 0; i < 500000000; i++)
    {
        x += 1.0;
    }
}

__attribute__((noinline)) void level_4()
{
    // Mid-depth: 300M iterations
    volatile double x = 0;
    for (long i = 0; i < 300000000; i++)
    {
        x += 1.0;
    }
    level_5();
    x += 0.0; // Prevent tail-call optimization
}

__attribute__((noinline)) void level_3()
{
    // Shallow: 200M iterations
    volatile double x = 0;
    for (long i = 0; i < 200000000; i++)
    {
        x += 1.0;
    }
    level_4();
    x += 0.0;
}

__attribute__((noinline)) void level_2()
{
    // Very shallow: 100M iterations
    volatile double x = 0;
    for (long i = 0; i < 100000000; i++)
    {
        x += 1.0;
    }
    level_3();
    x += 0.0;
}

__attribute__((noinline)) void level_1()
{
    // Direct child of main: 50M iterations
    volatile double x = 0;
    for (long i = 0; i < 50000000; i++)
    {
        x += 1.0;
    }
    level_2();
    x += 0.0;
}

static __attribute__((noinline)) void workload_once()
{
    level_1();
}

int main(int32_t argc, char **argv)
{
    OPTKIT_INIT();

    // call level_1 once to warm up
    workload_once();

    // start measurement after warm up
    // call level_1 a few times
    static constexpr int kIters = 3;
    double baseline_ms = 0.0;
    {
        BLOCK_TIMER("baseline", baseline_ms);
        for (int i = 0; i < kIters; ++i)
            workload_once();
    }

    // stop measurement

    // Profiled run (sampling/collection overhead; avoids file I/O + verbose printing)
    double profiled_ms = 0.0;
    {
        BLOCK_TIMER("profiled", profiled_ms);
        OPTKIT_CALLSTACK_PROFILER("overhead_block");
        for (int i = 0; i < kIters; ++i)
            workload_once();
    }

    const double overhead_ms = profiled_ms - baseline_ms;
    const double overhead_pct = (baseline_ms > 0.0) ? (overhead_ms / baseline_ms) * 100.0 : 0.0;

    std::cout << "\n[Callstack profiler overhead]\n";
    std::cout << "  iterations: " << kIters << "\n";
    std::cout << "  baseline:   " << baseline_ms << " ms\n";
    std::cout << "  profiled:   " << profiled_ms << " ms\n";
    std::cout << "  overhead:   " << overhead_ms << " ms (" << overhead_pct << "%)\n\n";

    return 0;
}
