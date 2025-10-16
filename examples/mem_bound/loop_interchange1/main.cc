#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

#include "solution.hh"
#include "solution_patch.hh"
#include "optkit.hh"

static bool equals(const Matrix &a, const Matrix &b)
{
    constexpr int maxErrors = 10;
    const float epsilon = std::sqrt(std::numeric_limits<float>::epsilon());

    int errors = 0;
    for (int i = 0; i < LOOP_INTERCHANGE1_N; i++)
    {
        for (int j = 0; j < LOOP_INTERCHANGE1_N; j++)
        {
            float va = a[i][j];
            float vb = b[i][j];
            float error = std::abs(va - vb);
            if (error >= epsilon)
            {
                std::cerr << "Result[" << i << ", " << j << "] = " << va
                          << ". Expected[" << i << ", " << j << "] = " << vb
                          << std::endl;
                if (++errors >= maxErrors)
                    return false;
            }
        }
    }
    return 0 == errors;
}

int main()
{
    OPTKIT_INIT(false);
    constexpr int k = 15;
    constexpr int k1 = 5;

    std::unique_ptr<Matrix> a(new Matrix());
    std::unique_ptr<Matrix> b(new Matrix());
    std::unique_ptr<Matrix> c(new Matrix());
    std::unique_ptr<Matrix> d(new Matrix());

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;
    {
        optkit::utils::BlockTimer block_timer("solution", first_duration_ms);
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::carm());
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        for (int i = 0; i < LOOP_INTERCHANGE1_BENCH_ITER; i++)
        {
            init(*a);
            zero(*b);
            identity(*c);
            identity(*d);
            {
                multiply(*b, *a, *d);
                if (!equals(*b, *a))
                {
                    std::cerr << "Validation Failed. a * 1" << std::endl;
                    return 1;
                }
            }
            {
                multiply(*b, *a, *a);
                *c = power(*a, 2);
                if (!equals(*b, *c))
                {
                    std::cerr << "Validation Failed. a^2" << std::endl;
                    return 1;
                }
            }
            *b = power(*a, k);
            *c = power(*a, k1);
            *d = power(*a, k - k1);
            multiply(*a, *c, *d);
            if (!equals(*a, *b))
            {
                std::cerr << "Validation Failed. a^k" << std::endl;
                return 1;
            }
        }
    }
    std::cout << "Validation Successful" << std::endl;
    {
        optkit::utils::BlockTimer block_timer("patch solution", second_duration_ms);
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::carm());

        for (int i = 0; i < LOOP_INTERCHANGE1_BENCH_ITER; i++)
        {
            init(*a);
            zero(*b);
            identity(*c);
            identity(*d);
            {
                multiply_patch(*b, *a, *d);
                if (!equals(*b, *a))
                {
                    std::cerr << "Validation Failed. a * 1" << std::endl;
                    return 1;
                }
            }
            {
                multiply_patch(*b, *a, *a);
                *c = power_patch(*a, 2);
                if (!equals(*b, *c))
                {
                    std::cerr << "Validation Failed. a^2" << std::endl;
                    return 1;
                }
            }
            *b = power_patch(*a, k);
            *c = power_patch(*a, k1);
            *d = power_patch(*a, k - k1);
            multiply_patch(*a, *c, *d);
            if (!equals(*a, *b))
            {
                std::cerr << "Validation Failed. a^k" << std::endl;
                return 1;
            }
        }
    }

    std::cout << "Validation Successful" << std::endl;

    std::cout << "First Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Second Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "Speedup: " << first_duration_ms / second_duration_ms << "x" << std::endl;
    return 0;
}