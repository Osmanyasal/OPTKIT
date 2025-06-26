#include <omp.h> 
#include "optkit.hh"

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
