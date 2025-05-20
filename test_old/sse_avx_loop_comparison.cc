
#include <optkit.hh>
#include <immintrin.h> // for AVX
#include <xmmintrin.h> // for SSE
#include <events/intel/icl.hh>

namespace optkit::test
{

    // Function to perform addition using SSE
    void add__sse(float *a, float *b, float *result, int size)
    {
        __m128 *xmm_a = reinterpret_cast<__m128 *>(a);
        __m128 *xmm_b = reinterpret_cast<__m128 *>(b);
        __m128 *xmm_result = reinterpret_cast<__m128 *>(result);

        for (int i = 0; i < size / 4; ++i)
        {
            xmm_result[i] = _mm_add_ps(xmm_a[i], xmm_b[i]);
        }
    }

    // Function to perform addition using AVX
    void add__avx(float *a, float *b, float *result, int size)
    {
        // Loop for processing 8 elements at a time
        for (int i = 0; i < size / 8; i += 8)
        {
            // Load 8 elements from Arrays A and B into AVX registers
            __m256 avx_a = _mm256_loadu_ps(&a[i]);
            __m256 avx_b = _mm256_loadu_ps(&b[i]);

            // Perform vector addition
            __m256 avx_result = _mm256_add_ps(avx_a, avx_b);

            // Store the result back into Array C
            _mm256_storeu_ps(&result[i], avx_result);
        }
    }

    // Function to perform addition in a serial manner
    void add__serial(float *a, float *b, float *result, int size)
    {
        for (int i = 0; i < size; ++i)
        {
            result[i] = a[i] + b[i];
        }
    }

    void example__sse_avx_loop_comparison()
    {
        const int rapl_repeat = 500000000; // 50M
        const int ARRAY_SIZE = 16;
        float aa[ARRAY_SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
        float bb[ARRAY_SIZE] = {16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
        float result_sse[ARRAY_SIZE];
        float result_avx[ARRAY_SIZE];
        float result_serial[ARRAY_SIZE];

        std::cout << "========= SYSTEM NOISE ===========" << std::endl;

        {
            OPTKIT_RAPL_REPEAT(add_sse, "SYSTEM NOISE", rapl_repeat)
            {
                // do nothing
            }
        }

        std::cout << "========= SSE ===========" << std::endl;
        {
            OPTKIT_RAPL_REPEAT(add_sse, "Add SSE", rapl_repeat)
            {
                add__sse(aa, bb, result_sse, ARRAY_SIZE);
            }
        }

        std::cout << "========= AVX ===========" << std::endl;

        {
            OPTKIT_RAPL_REPEAT(add_avx, "Add AVX", rapl_repeat)
            {
                add__avx(aa, bb, result_avx, ARRAY_SIZE);
            }

            sleep(1);
            add_avx.read();
        }

        std::cout << "========= SERIAL =========" << std::endl;

        {
            OPTKIT_RAPL_REPEAT(add_serial, "Add SERIAL", rapl_repeat)
            {
                add__serial(aa, bb, result_serial, ARRAY_SIZE);
            }

            sleep(1);
            add_serial.read();

            sleep(1);
            add_serial.read();
        }

        std::cout << "========== DONE ==========\n";
    }
} // namespace optkit::examples
