#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

#include "../../src/optkit.hh"
#include "../../src/core/metrics/performance/module.hh"
#include "../../src/core/pmu/cpu/perf/block_profiler.hh"
#include "../../src/core/pmu/cpu/perf/riscv/module.hh"

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
    auto instructions_metrics = optkit::metrics::MetricBuilder<uint64_t>{};
    instructions_metrics.add(optkit::metrics::performance::cpu_metrics::get_metric("instructions"));

    auto llc_miss_metrics = optkit::metrics::MetricBuilder<uint64_t>{};
    llc_miss_metrics
        .add(optkit::metrics::performance::cpu_metrics::get_metric("LLC-load-misses"))
        .add(optkit::metrics::performance::cpu_metrics::get_metric("LLC-store-misses"));

    auto instructions_config = optkit::pmu::cpu::perf::riscv::instructions_profiler_config(
        "riscv_instructions_example",
        false,
        false,
        0,
        -1,
        "cpu_pmu.instructions");

    auto llc_miss_config = optkit::pmu::cpu::perf::riscv::llc_load_misses_profiler_config(
        "riscv_llc_misses_example",
        false,
        false,
        0,
        -1,
        "cpu_pmu.llc_misses");

    {
        // The shared metric interface provides the event mappings, while the RISC-V
        // perf configs still select the correct perf_event type for each profiler.
        optkit::pmu::cpu::perf::BlockProfiler instructions_profiler(instructions_config, instructions_metrics);
        optkit::pmu::cpu::perf::BlockProfiler llc_miss_profiler(llc_miss_config, llc_miss_metrics);
        llc_heavy_workload();
    }

    // For system-wide per-core monitoring, use pid = -1 and cpu = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS.
    // Example:
    // auto system_wide_cfg = optkit::pmu::cpu::perf::riscv::llc_load_misses_profiler_config(
    //     "riscv_llc_misses_system_wide", false, false, -1, OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS);

    return 0;
#else
    std::cerr << "This example requires a RISC-V build with perf_event support enabled.\n";
    return 1;
#endif
}