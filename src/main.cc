#include <omp.h>
#include "optkit.hh"
#include "core/pmu/cpu/perf/events/amd64/fam19h_zen4.hh"
#include "core/pmu/cpu/perf/events/intel/icl.hh"

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
        {                // Iterate over columns of B
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

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();
    // OPTKIT_PERFORMANCE_EVENTS("main","test",tt, {{ optkit::amd64::fam19h_zen4::RETIRED_INSTRUCTIONS, "Retired Instructions"}});
    OPTKIT_PERFORMANCE_EVENTS("main","test",tt, {{ optkit::intel::icl::INSTRUCTIONS_RETIRED, "Retired Instructions"}});


    return 0;
}
