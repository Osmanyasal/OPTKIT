#include <iostream>
#include <vector>
#include <numeric>
#include <atomic>
#include <omp.h>

#include "optkit.hh"

// Unpadded accumulator structure: adjacent elements share the same cache line
struct FalseSharingAccumulator
{
    std::atomic<uint64_t> count{0};
};

// Cache-line aligned accumulator: eliminates false sharing across cores
#define CACHELINE_ALIGN alignas(64)
struct CACHELINE_ALIGN PaddedAccumulator
{
    std::atomic<uint64_t> count{0};
};

static uint64_t run_false_sharing_workload(int num_threads, size_t iterations)
{
    std::vector<FalseSharingAccumulator> accumulators(num_threads);

    #pragma omp parallel num_threads(num_threads) shared(accumulators)
    {
        int tid = omp_get_thread_num();
        for (size_t i = 0; i < iterations; ++i)
        {
            // Frequent concurrent writes to adjacent atomic variables in memory
            accumulators[tid].count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    uint64_t total = 0;
    for (const auto &acc : accumulators)
    {
        total += acc.count.load();
    }
    return total;
}

static uint64_t run_padded_workload(int num_threads, size_t iterations)
{
    std::vector<PaddedAccumulator> accumulators(num_threads);

    #pragma omp parallel num_threads(num_threads) shared(accumulators)
    {
        int tid = omp_get_thread_num();
        for (size_t i = 0; i < iterations; ++i)
        {
            // Writes to cacheline-isolated variables
            accumulators[tid].count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    uint64_t total = 0;
    for (const auto &acc : accumulators)
    {
        total += acc.count.load();
    }
    return total;
}

int main(int argc, char **argv)
{
    // Initialize OPTKIT (creates output folder and session context)
    OPTKIT_INIT();

    const int num_threads = omp_get_max_threads() > 1 ? omp_get_max_threads() : 4;
    constexpr size_t iterations = 200000000;

    std::cout << "Running False Sharing vs Cache-Aligned benchmark with " 
              << num_threads << " threads and " << iterations << " iterations per thread...\n";

    // 1. Measure all_mpki on false sharing (unaligned/unpadded) workload
    {
        std::cout << "Profiling false-sharing workload (all_mpki)...\n";
        OPTKIT_CPU_EVENTS("false_sharing_unpadded", optkit::metrics::performance::cpu_metrics::all_mpki());
        uint64_t res = run_false_sharing_workload(num_threads, iterations);
        std::cout << "False sharing result: " << res << "\n";
    }

    // 2. Measure all_mpki on cacheline-padded workload
    {
        std::cout << "Profiling padded workload (all_mpki)...\n";
        OPTKIT_CPU_EVENTS("padded_cacheline_aligned", optkit::metrics::performance::cpu_metrics::all_mpki());
        uint64_t res = run_padded_workload(num_threads, iterations);
        std::cout << "Padded result: " << res << "\n";
    }

    std::cout << "OPTKIT inline instrumentation completed successfully.\n";
    return 0;
}