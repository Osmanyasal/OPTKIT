#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

#include "../../src/optkit.hh"
#include "../../src/core/metrics/performance/module.hh"
#include "../../src/core/pmu/cpu/perf/module.hh"

namespace
{
    void llc_heavy_workload()
    {
        constexpr std::size_t element_count = 64U * 1024U * 1024U;
        std::vector<std::uint64_t> data(element_count, 1ULL);

        for (int pass = 0; pass < 4; ++pass)
        {
            for (std::size_t index = 0; index < data.size(); index += 64)
            {
                data[index] += static_cast<std::uint64_t>(index + pass);
            }
        }

        volatile std::uint64_t checksum = std::accumulate(data.begin(), data.end(), 0ULL);
        (void)checksum;
    }
}

int main()
{
    OPTKIT_INIT();

#if OPTKIT_ENV_CPU_RISCV && OPTKIT_ENV_LIB_PERF_EVENT
    {
        OPTKIT_CPU_EVENTS(
            "riscv_inst_retired_example",
            optkit::metrics::performance::cpu_metrics::get_metric("INST_RETIRED"));
        llc_heavy_workload();
    }

    {
        optkit::metrics::MetricBuilder<uint64_t> llc_metrics;
        llc_metrics.add(optkit::metrics::performance::cpu_metrics::get_metric("LLC-load-misses"));
        llc_metrics.add(optkit::metrics::performance::cpu_metrics::get_metric("LLC-store-misses"));

        OPTKIT_CPU_EVENTS("riscv_llc_misses_example", llc_metrics);
        llc_heavy_workload();
    }

    return 0;
#else
    std::cerr << "This example requires a RISC-V build with perf_event support enabled.\n";
    return 1;
#endif
}