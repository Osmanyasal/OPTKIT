#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "optkit.hh"

namespace
{
    double flops_workload(int iterations, int size)
    {
        std::vector<double> a(static_cast<size_t>(size) * size, 1.1);
        std::vector<double> b(static_cast<size_t>(size) * size, 1.2);
        std::vector<double> c(static_cast<size_t>(size) * size, 0.0);

        auto idx = [size](int row, int col)
        { return static_cast<size_t>(row) * size + col; };

        double total = 0.0;
        for (int iter = 0; iter < iterations; ++iter)
        {
            std::fill(c.begin(), c.end(), 0.0);
            for (int i = 0; i < size; ++i)
            {
                for (int k = 0; k < size; ++k)
                {
                    const double a_val = a[idx(i, k)];
                    for (int j = 0; j < size; ++j)
                    {
                        c[idx(i, j)] += a_val * b[idx(k, j)];
                    }
                }
            }
            total += c[0];
        }
        return total;
    }
}

int main()
{
    // Create the OPTKIT engine; this also sets up the execution folder.
    OPTKIT_INIT();
    // OPTKIT_CPU_EVENTS_DISTINCT_CORES("carm_cpp_block", optkit::metrics::performance::cpu_metrics::get_metric("carm"));
    OPTKIT_CPU_EVENTS("carm_cpp_block", optkit::metrics::performance::cpu_metrics::get_metric("carm"));
    double total = flops_workload(5000, 192);
    std::cout << "Workload complete. Total=" << total << "\n";

    return 0;
}
