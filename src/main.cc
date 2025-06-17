#include <omp.h>

#include "optkit.hh"
#include "core/pmu/cpu/events/amd64/fam19h_zen4.hh"
#include "core/pmu/cpu/events/intel/icl.hh"
#include "core/pmu/cpu/query_pmu.hh"

#include "core/metrics/cpu/amd/core_metrics.hh"
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

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();

    // {
    //     OPTKIT_CPU_EVENTS("b2", {{"Retired Instructions", optkit::intel::icl::INSTRUCTIONS_RETIRED}});

    //     // 10 instructions
    //     int i = 3;
    //     i++;
    //     i++;
    //     i++;
    //     i++;
    //     i++;
    //     i++;
    // }

    using namespace optkit::core::metrics::cpu;

    MetricBuilder branch_mispr = metrics::BranchMisprRatio();
    branch_mispr.add(to_string(CoreEvents::INST_RETIRED), mapper::get(CoreEvents::INST_RETIRED));
    std::cout << branch_mispr << "\n";

    {
        OPTKIT_CPU_EVENTS("b0", branch_mispr);
        // var112.read();
        // 9 instructions
        int i = 3;
        if (i >= 3)
        {
            i++;
            i++;
            i++;
            i++;
            i++;
            i++;
        }
    }

    std::cout << optkit::core::pmu::cpu::QueryPMU::default_pmu_info().num_cntrs << "\n";
    {
        auto metric = metrics::AllMPKI();
        std::cout << metric << "\n";
        OPTKIT_CPU_EVENTS("b0", metric);
        sleep(5);
    }
    return 0;
}
