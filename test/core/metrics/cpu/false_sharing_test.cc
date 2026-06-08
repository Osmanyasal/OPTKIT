#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <thread>
#include <vector>

#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"
#include "core/pmu/cpu/perf/block_profiler.hh"

using namespace optkit::metrics;

namespace
{
    struct FalseSharingAccumulator
    {
        std::atomic<uint32_t> value{0};
    };

    struct alignas(64) PaddedAccumulator
    {
        std::atomic<uint32_t> value{0};
    };

    struct RunMeasurement
    {
        std::size_t result{0};
        double duration_ms{0.0};
        bool has_false_sharing_ratio{false};
        double false_sharing_ratio{std::numeric_limits<double>::quiet_NaN()};
    };

    std::size_t serial_reference(const std::vector<uint32_t> &data)
    {
        std::size_t value = 0;
        for (uint32_t item : data)
        {
            item += 1000;
            item ^= 0xADEDAE;
            item |= (item >> 24);
            value += item % 13;
        }
        return value;
    }

    template <typename Accumulator>
    std::size_t run_parallel_accumulators(const std::vector<uint32_t> &data, std::size_t thread_count)
    {
        std::vector<Accumulator> accumulators(thread_count);
        std::vector<std::thread> workers;
        workers.reserve(thread_count);

        const std::size_t chunk_size = (data.size() + thread_count - 1) / thread_count;
        for (std::size_t thread_index = 0; thread_index < thread_count; ++thread_index)
        {
            workers.emplace_back([&, thread_index]()
            {
                const std::size_t begin = thread_index * chunk_size;
                const std::size_t end = std::min(begin + chunk_size, data.size());
                auto &target = accumulators[thread_index].value;

                for (std::size_t i = begin; i < end; ++i)
                {
                    uint32_t item = data[i];
                    item += 1000;
                    item ^= 0xADEDAE;
                    item |= (item >> 24);
                    target.fetch_add(item % 13, std::memory_order_relaxed);
                }
            });
        }

        for (auto &worker : workers)
            worker.join();

        std::size_t result = 0;
        for (const auto &accumulator : accumulators)
            result += accumulator.value.load(std::memory_order_relaxed);
        return result;
    }

    template <typename Workload>
    RunMeasurement measure_workload(const char *block_name, Workload &&workload)
    {
        RunMeasurement measurement;
        const auto &metric_builder = performance::cpu_metrics::get_metric("false_sharing_ratio");
        const bool can_measure_false_sharing = !metric_builder.metric_events.empty();

        if (can_measure_false_sharing)
        {
            optkit::pmu::cpu::perf::PerfProfilerConfig config(block_name, false, false, 0, -1, "cpu_pmu", true, false, false);
            optkit::pmu::cpu::perf::BlockProfiler profiler(config, metric_builder);

            const auto start = std::chrono::steady_clock::now();
            measurement.result = workload();
            const auto end = std::chrono::steady_clock::now();

            measurement.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            profiler.read_and_store();

            const auto aggregated_results = profiler.aggregate();
            const auto metric_results = metric_builder.calculate(aggregated_results);
            if (!metric_results.empty())
            {
                measurement.has_false_sharing_ratio = std::isfinite(metric_results.front().second);
                measurement.false_sharing_ratio = metric_results.front().second;
            }
            return measurement;
        }

        const auto start = std::chrono::steady_clock::now();
        measurement.result = workload();
        const auto end = std::chrono::steady_clock::now();
        measurement.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
        return measurement;
    }
}

TEST(FalseSharingMetrics, ContendedCountersVsPaddedCounters)
{
    const std::size_t hardware_threads = std::thread::hardware_concurrency();
    if (hardware_threads < 2)
        GTEST_SKIP() << "False-sharing test requires at least two hardware threads.";

    const std::size_t thread_count = std::min<std::size_t>(hardware_threads, 8);
    const std::size_t element_count = 16 * 1024 * 1024;

    std::vector<uint32_t> data(element_count);
    std::iota(data.begin(), data.end(), 0u);

    const std::size_t expected_result = serial_reference(data);

    const RunMeasurement false_sharing_run = measure_workload(
        "FalseSharing_Bad",
        [&]()
        {
            return run_parallel_accumulators<FalseSharingAccumulator>(data, thread_count);
        });

    const RunMeasurement fixed_run = measure_workload(
        "FalseSharing_Fixed",
        [&]()
        {
            return run_parallel_accumulators<PaddedAccumulator>(data, thread_count);
        });

    EXPECT_EQ(expected_result, false_sharing_run.result);
    EXPECT_EQ(expected_result, fixed_run.result);

    GTEST_LOG_(INFO) << "False-sharing run duration (ms): " << false_sharing_run.duration_ms;
    GTEST_LOG_(INFO) << "Fixed run duration (ms): " << fixed_run.duration_ms;
    GTEST_LOG_(INFO) << "Speedup: " << (false_sharing_run.duration_ms / fixed_run.duration_ms) << "x";

    if (false_sharing_run.has_false_sharing_ratio && fixed_run.has_false_sharing_ratio)
    {
        GTEST_LOG_(INFO) << "False-sharing ratio bad (%): " << false_sharing_run.false_sharing_ratio;
        GTEST_LOG_(INFO) << "False-sharing ratio fixed (%): " << fixed_run.false_sharing_ratio;
        EXPECT_GT(false_sharing_run.false_sharing_ratio, fixed_run.false_sharing_ratio);
    }
    else
    {
        GTEST_LOG_(INFO) << "false_sharing_ratio is not supported on this CPU configuration; duration comparison only.";
    }
}