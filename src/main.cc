#include <omp.h>

#include "optkit.hh"
#include "instructions.hh"

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
    MetricBuilder tt = cpu::core_metrics::AllTopdown();

    {
        OPTKIT_CPU_EVENTS("test", tt);

        for (int i = 0; i < 100000; i++)
        {
            // std::cout << "i->" << i << "\n";
            instructions_million();
        }
    }

    std::cout << "end!\n";
    return 0;
}
