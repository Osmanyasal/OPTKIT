#include <omp.h>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"
using namespace optkit::core::metrics;
using namespace optkit::core::pmu::cpu;

TEST(CPUPerfGroupEventsTest, Instructions_1M)
{
    int32_t expected_result = 1'000'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
        .build(to_string(cpu::core_events::INST_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_GROUP_EVENTS_REPEAT("Instructions_1M", mb, REPEAT)
    {
        instructions_million();
    }
    auto aggregated_results = var17.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}
TEST(CPUPerfGroupEventsTest, BranchInst1500K)
{
    int32_t expected_result = 1'500'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::BRANCH_INST_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_INST_RETIRED))
        .build(to_string(cpu::core_events::BRANCH_INST_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::BRANCH_INST_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_GROUP_EVENTS_REPEAT("BranchInst1500K", mb, REPEAT)
    {
        branches();
    }
    auto aggregated_results = var34.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::BRANCH_INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}

TEST(CPUPerfGroupEventsTest, BranchMisp250K)
{
    int32_t expected_result = 250'000;

    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::BRANCH_MISP_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_MISP_RETIRED))
        .build(to_string(cpu::core_events::BRANCH_MISP_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::BRANCH_MISP_RETIRED)) / (double)REPEAT; });

    OPTKIT_CPU_GROUP_EVENTS_REPEAT("BranchMisp250K", mb, REPEAT)
    {
        random_branches(500'000, true);
    }

    auto aggregated_results = var52.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::BRANCH_MISP_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE * 5); // error rate -> 25%
}

TEST(CPUPerfGroupEventsTest, RetiredFlopAny1M)
{
    size_t expected_result = 1'000'000;
    MetricBuilder mb{false};
#if OPTKIT_ENV_CPU_AMD
    mb.add(to_string(cpu::core_events::RETIRED_FLOPS_ANY), cpu::event_mapper::get(cpu::core_events::RETIRED_FLOPS_ANY))
        .build(to_string(cpu::core_events::RETIRED_FLOPS_ANY), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::RETIRED_FLOPS_ANY)) / (double)REPEAT; });
#else // OPTKIT_ENV_CPU_INTEL
    mb.add(to_string(cpu::native_events::FP_ARITH_INST_RETIRED_SCALAR), cpu::event_mapper::get(cpu::native_events::FP_ARITH_INST_RETIRED_SCALAR))
        .build(to_string(cpu::native_events::FP_ARITH_INST_RETIRED_SCALAR), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::native_events::FP_ARITH_INST_RETIRED_SCALAR)) / (double)REPEAT; });
#endif

    size_t n = 1 << 20;            // ~1 million elements
    std::vector<float> A(n, 1.0f); // all 1s
    std::vector<float> B(n, 2.0f); // all 2s
    std::vector<float> C;

    size_t size = A.size();
    C.resize(size);

    OPTKIT_CPU_GROUP_EVENTS_REPEAT("RetiredFlopAny1M", mb, REPEAT)
    {
        for (size_t i = 0; i < size; ++i)
        {
            C[i] = A[i] + B[i];
        }
    }
    auto aggregated_results = var84.aggregate();
#if OPTKIT_ENV_CPU_AMD
    auto result = aggregated_results.at(to_string(cpu::core_events::RETIRED_FLOPS_ANY)) / (double)REPEAT;
#else
    auto result = aggregated_results.at(to_string(cpu::native_events::FP_ARITH_INST_RETIRED_SCALAR)) / (double)REPEAT;
#endif
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}

// note that accumulated result also contains elements below, not just the region.
TEST(CPUPerfGroupEventsTest, ReadsAndAccumulatesEventData)
{
    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
        .build(to_string(cpu::core_events::INST_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT; });
    double total = 0;
    OPTKIT_CPU_GROUP_EVENTS_REPEAT("ReadsAndAccumulatesEventData", mb, REPEAT)
    {
        instructions_million();
        // Note: *REPEAT already reads_and_store each iteration automatically.
        // Since we reset counters any read, after the following line, loop will read close to 0. but it is okay since we save to same vector and accumulate.
        total += var108.read_and_store().second[0]; // get the first value.
    }
    total /= (double)REPEAT;
    auto aggregated_results = var108.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(total, result, total * ERROR_RATE);
}
TEST(CPUPerfGroupEventsTest, EnableDisableEventCounting)
{
    size_t expected_result = 500'000; // apprx
    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
        .build(to_string(cpu::core_events::INST_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT; });

    bool is_enabled = true;
    OPTKIT_CPU_GROUP_EVENTS_REPEAT("EnableDisableEventCounting", mb, REPEAT)
    {
        if (is_enabled)
            PMUEventManager::enable_all_events();
        else
            PMUEventManager::disable_all_events();

        is_enabled = !is_enabled;
        instructions_million();
    }
    auto aggregated_results = var129.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT;
    EXPECT_NEAR(expected_result, result, expected_result * ERROR_RATE);
}

TEST(CPUPerfGroupEventsTest, AddMoreEventsThanGroupLimitTest)
{
    size_t expected_result = 500'000; // apprx
    MetricBuilder mb{false};
    mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
        .build(to_string(cpu::core_events::INST_RETIRED), [](const auto &map) -> double
               { return get_event_count(map, to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT; });

    auto event = mb.metric_events[0];
    for (int i = 1; i < PMUEventManager::pmu_num_cntrs(); i++)
        mb.metric_events.push_back(event);

    OPTKIT_CPU_GROUP_EVENTS_REPEAT("EnableDisableEventCounting", mb, REPEAT)
    {
        instructions_million();
    }
    auto aggregated_results = var156.aggregate();
    auto result = aggregated_results.at(to_string(cpu::core_events::INST_RETIRED)) / (double)REPEAT;
    GTEST_LOG_(INFO) << "result: " << result;
    EXPECT_NEAR(0, result, 0 * ERROR_RATE);
}