#include <omp.h>
#include "optkit.hh"
#include <immintrin.h> // AVX intrinsics

#define VECTOR_SIZE 1000000

// Generate a vector of doubles where vec[i] = i
template <class T>
inline std::vector<T> generate_vector(size_t n = VECTOR_SIZE)
{
    std::vector<T> vec(n);
    for (size_t i = 0; i < n; ++i)
        vec[i] = static_cast<T>(i);
    return vec;
}

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});
    OPTKIT_CPU_TEMPERATURE_EVENTS("main", {});
    // optkit::core::metrics::MetricBuilder mb{true, true};

    // mb.add(optkit::core::metrics::cpu::core_metrics::IpC());

    // OPTKIT_CPU_EVENTS("main", mb);

    // instructions_million();
    // var12.read_and_store();
    // instructions_million();

    sleep(1);
    for (int j = 0; j < 1000; j++)
    {
        std::vector<double> v1 = generate_vector<double>(); // 1 million elements
        std::vector<double> v2 = generate_vector<double>(); // 1 million elements
        std::vector<double> v3(VECTOR_SIZE);

        // OPTKIT_CPU_EVENTS("FLOPs_AVX", optkit::core::metrics::cpu::core_metrics::IpAVXAnyFlop());

        constexpr int simd_width = 4; // AVX 256-bit holds 4 doubles
        int i = 0;

        // Process blocks of 4 doubles at once
        for (; i + simd_width <= VECTOR_SIZE; i += simd_width)
        {
            // Load 4 doubles from v1 and v2
            __m256d vec1 = _mm256_loadu_pd(&v1[i]);
            __m256d vec2 = _mm256_loadu_pd(&v2[i]);

            // vec1 * 2.0 + vec2
            __m256d vec_mul = _mm256_mul_pd(vec1, _mm256_set1_pd(2.0));
            __m256d vec_res = _mm256_add_pd(vec_mul, vec2);

            // Store result
            _mm256_storeu_pd(&v3[i], vec_res);
        }

        // Process remaining elements scalar way
        for (; i < VECTOR_SIZE; i++)
        {
            v3[i] = v1[i] * 2.0 + v2[i];
        }
    }
    return 0;
}
