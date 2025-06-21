#include <omp.h>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

using namespace optkit::core::metrics;

TEST(BlockProfilerTest, Instructions_1M)
{
    int32_t expected_result = 1'000'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
        .build(to_string(cpu::core_events::INST_RETIRED), [](const auto &map) -> double
               { return map.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_EVENTS_REPEAT("Instructions_1M", mb, REPEAT)
    {
        instructions_million();
    }
    auto aggregated_results = var17.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}
TEST(BlockProfilerTest, BranchInst1500K)
{
    int32_t expected_result = 1'500'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::BRANCH_INST_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_INST_RETIRED))
        .build(to_string(cpu::core_events::BRANCH_INST_RETIRED), [](const auto &map) -> double
               { return map.at(to_string(cpu::core_events::BRANCH_INST_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_EVENTS_REPEAT("BranchInst1500K", mb, REPEAT)
    {
        branches();
    }
    auto aggregated_results = var34.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::BRANCH_INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}

TEST(BlockProfilerTest, BranchMisp250K)
{
    int32_t expected_result = 250'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::BRANCH_MISP_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_MISP_RETIRED))
        .build(to_string(cpu::core_events::BRANCH_MISP_RETIRED), [](const auto &map) -> double
               { return map.at(to_string(cpu::core_events::BRANCH_MISP_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_EVENTS_REPEAT("BranchMisp250K", mb, REPEAT)
    {
        random_branches(500'000, true);
    }

    auto aggregated_results = var52.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::BRANCH_MISP_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE * 5); // error rate -> 25%
}

TEST(BlockProfilerTest, SSE_AVX_1M)
{
    size_t expected_result = 1'000'000;
    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY), cpu::event_mapper::get(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY))
        .build(to_string(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY), [](const auto &map) -> double
               { return map.at(to_string(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY)) / (double)REPEAT; });

    size_t n = 1 << 20;            // ~1 million elements
    std::vector<float> A(n, 1.0f); // all 1s
    std::vector<float> B(n, 2.0f); // all 2s
    std::vector<float> C;

    size_t size = A.size();
    C.resize(size);

    OPTKIT_CPU_EVENTS_REPEAT("SSE_AVX_1M", mb, REPEAT)
    {
        for (size_t i = 0; i < size; ++i)
        {
            C[i] = A[i] + B[i];
        }
    }
    auto aggregated_results = var78.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::RETIRED_SSE_AVX_FLOPS_ANY)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}

TEST(BlockProfilerTest, ReadsAndAccumulatesEventData)
{
}
TEST(BlockProfilerTest, MeasuresBlockForVectorFlopEvents)
{
}
TEST(BlockProfilerTest, EnablesEventCounting)
{
}
TEST(BlockProfilerTest, DisablesEventCounting)
{
}
TEST(BlockProfilerTest, ResetEventCounting)
{
}
TEST(BlockProfilerTest, ComparesMultiplexingCountWithEventManager)
{
}
