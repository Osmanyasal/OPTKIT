#include <omp.h>

#include "optkit.hh"
#include "core/pmu/cpu/events/amd64/fam19h_zen4.hh"
#include "core/pmu/cpu/events/intel/icl.hh"
#include "core/pmu/cpu/query_pmu.hh"

#include "core/metrics/cpu/module.hh"
#include "instructions.hh"

#include <unistd.h>

#define VECTOR_SIZE 100000000  // 100 million elements
#define NUM_ACCESSES 100000000 // 100 million random accesses

void random_access()
{
    // OPTKIT_COMPUTE_INTENSITY(ci_random_access, "random_access");

    // Initialize the vector with random values
    std::vector<double> vec(VECTOR_SIZE);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0); // Random values between 0.0 and 10.0

    for (size_t i = 0; i < VECTOR_SIZE; ++i)
    {
        vec[i] = dis(gen);
    }

    double sum = 0.0;

#pragma omp parallel reduction(+ : sum)
    {
        std::mt19937 thread_gen(rd() ^ std::hash<int>{}(omp_get_thread_num() ^ time(nullptr)));
        std::uniform_int_distribution<> thread_dis(0, vec.size() - 1);

#pragma omp for
        for (int32_t i = 0; i < NUM_ACCESSES; ++i)
        {
            int32_t idx = thread_dis(thread_gen); // Get a truly random index
            sum += vec[idx];
        }
    }

    std::cout << "Sum: " << std::fixed << sum << std::endl; // Output the sum to check correctness
}

void normalMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int j = 0; j < m; j++)
        { // Iterate over columns of B
            for (int k = 0; k < p; k++)
            { // Sum over the common dimension
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void loopInterchangeMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int k = 0; k < p; k++)
        { // Iterate over columns of A/rows of B
            for (int j = 0; j < m; j++)
            { // Iterate over columns of B
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void vector_add(const std::vector<float> &a,
                const std::vector<float> &b,
                std::vector<float> &result)
{
    size_t size = a.size();
    result.resize(size);

#pragma omp parallel for simd
    for (size_t i = 0; i < size; ++i)
    {
        result[i] = a[i] + b[i];
    }
}

using namespace optkit::core::metrics;
int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    //     size_t n = 1 << 20;            // ~1 million elements
    //     std::vector<float> A(n, 1.0f); // all 1s
    //     std::vector<float> B(n, 2.0f); // all 2s
    //     std::vector<float> C;

    //     MetricBuilder mb{};
    //     mb.add(to_string(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY), cpu::event_mapper::get(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY))
    //         .add(to_string(cpu::core_events::RETIRED_FLOPS_ANY), cpu::event_mapper::get(cpu::core_events::RETIRED_FLOPS_ANY))
    //         .add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED));
    //     {
    //         OPTKIT_CPU_EVENTS("main", mb);

    // #pragma omp parallel
    //         {
    // #pragma omp master
    //             {
    //                 std::cout << omp_get_max_threads() << "\n";
    //             }

    //             vector_add(A, B, C);
    //         }
    //     }
    //     std::cout << "done \n";

    MetricBuilder tt = cpu::core_metrics::TopdownL1();

    OPTKIT_CPU_EVENTS("test", tt);

    sleep(1);
    for (int i = 0; i < 100; i++)
    {
        std::cout << "i->" << i << "\n";
        instructions_million();
    }

    std::cout << "end!\n";
    return 0;
}
