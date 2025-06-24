#include <gtest/gtest.h>
#include "core/metrics/metric_builder.hh"

using namespace optkit::core::metrics;

// Note that, event numbers (hex format) are not important here, we do not measure any events we just test the behaviour of MetricBuilder.

TEST(MetricBuilderTest, AddEmptySingleName)
{
    MetricBuilder builder;
    builder.add("cycles", {});
    ASSERT_TRUE(builder.metric_events.empty());
}

TEST(MetricBuilderTest, AddEmptyVectorOfEvents)
{
    MetricBuilder builder;
    std::vector<std::pair<std::string, uint64_t>> empty_events;
    builder.add(empty_events);
    ASSERT_TRUE(builder.metric_events.empty());
}

TEST(MetricBuilderTest, AddEmptyNameButEventCode)
{
    MetricBuilder builder;
    std::vector<std::pair<std::string, uint64_t>> empty_events{{"",333}};
    builder.add(empty_events);
    ASSERT_EQ(builder.metric_events.size(), 1);
}

TEST(MetricBuilderTest, AddEmptyBuilder)
{
    MetricBuilder builder;
    MetricBuilder empty;
    builder.add(empty);
    ASSERT_TRUE(builder.metric_events.empty());
    ASSERT_TRUE(builder.metric_names().empty());
}

TEST(MetricBuilderTest, AddSingleEventAvoidsDuplicates)
{
    MetricBuilder builder;
    builder.add("inst_retired", {0x00c0});
    builder.add("inst_retired", {0x00c0}); // Duplicate

    ASSERT_EQ(builder.metric_events.size(), 1);
    EXPECT_EQ(builder.metric_events[0].first, "inst_retired");
    EXPECT_EQ(builder.metric_events[0].second, 0x00c0);
}

TEST(MetricBuilderTest, AddMultipleEvents)
{
    MetricBuilder builder;
    builder.add("cache_misses", {0x412e, 0x412f});

    ASSERT_EQ(builder.metric_events.size(), 2);
    EXPECT_EQ(builder.metric_events[0].first, "cache_misses");
    EXPECT_EQ(builder.metric_events[0].second, 0x412e);
    EXPECT_EQ(builder.metric_events[1].first, "cache_misses");
    EXPECT_EQ(builder.metric_events[1].second, 0x412f);
}

TEST(MetricBuilderTest, AddFromOtherBuilder)
{
    MetricBuilder builder1;
    builder1.add("inst_retired", {0x00c0});
    builder1.build("IPC", [](const auto &m)
                   { return m.at("inst_retired") / static_cast<double>(m.at("cpu_cycles")); });

    MetricBuilder builder2;
    builder2.add(builder1); // Inherit events + calculations

    ASSERT_EQ(builder2.metric_events.size(), 1);
    EXPECT_EQ(builder2.metric_events[0].first, "inst_retired");
    EXPECT_EQ(builder2.metric_events[0].second, 0x00c0);

    auto names = builder2.metric_names();
    ASSERT_EQ(names.size(), 1);
    EXPECT_EQ(names[0], "IPC");
}

TEST(MetricBuilderTest, BuildAndCalculateMetrics)
{
    MetricBuilder builder;
    builder.add("inst_retired", {0x00c0});
    builder.add("cpu_cycles", {0x003c});
    builder.build("IPC", [](const auto &m)
                  { return m.at("inst_retired") / static_cast<double>(m.at("cpu_cycles")); });

    std::unordered_map<std::string, uint64_t> results = {
        {"inst_retired", 4'000'000},
        {"cpu_cycles", 8'000'000}};

    auto metrics = builder.calculate(results);
    ASSERT_EQ(metrics.size(), 1);
    EXPECT_EQ(metrics[0].first, "IPC");
    EXPECT_DOUBLE_EQ(metrics[0].second, 0.5);
}

TEST(MetricBuilderTest, CalculateMultipleMetrics)
{
    MetricBuilder builder;
    builder.add("inst_retired", {0x00c0})
        .add("cpu_cycles", {0x003c})
        .add("cache_misses", {0x412e})
        .build("IPC", [](const auto &m)
               { return m.at("inst_retired") / static_cast<double>(m.at("cpu_cycles")); })
        .build("MPKI", [](const auto &m)
               { return m.at("cache_misses") * 1000.0 / m.at("inst_retired"); });

    std::unordered_map<std::string, uint64_t> results = {
        {"inst_retired", 5'000'000},
        {"cpu_cycles", 10'000'000},
        {"cache_misses", 25'000}};

    auto metrics = builder.calculate(results);
    ASSERT_EQ(metrics.size(), 2);

    std::unordered_map<std::string, double> computed;
    for (const auto &[name, value] : metrics)
    {
        computed[name] = value;
    }

    ASSERT_EQ(computed.size(), 2);
    EXPECT_DOUBLE_EQ(computed["IPC"], 0.5);
    EXPECT_DOUBLE_EQ(computed["MPKI"], 5.0);
}

TEST(MetricBuilderTest, MetricNamesList)
{
    MetricBuilder builder;
    builder.build("Metric1", [](const auto &)
                  { return 1.0; });
    builder.build("Metric2", [](const auto &)
                  { return 2.0; });

    auto names = builder.metric_names();
    ASSERT_EQ(names.size(), 2);
    EXPECT_NE(std::find(names.begin(), names.end(), "Metric1"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "Metric2"), names.end());
}

TEST(MetricBuilderTest, GetCalculationFunctionByName)
{
    MetricBuilder builder;
    builder.build("TestMetric", [](const auto &m)
                  { return m.at("a") + m.at("b"); });

    auto func = builder.metric_calculation_func("TestMetric");
    double result = func({{"a", 3}, {"b", 7}});
    EXPECT_DOUBLE_EQ(result, 10.0);
}

TEST(MetricBuilderTest, PreventDuplicateAcrossBuilderMerge)
{
    MetricBuilder b1, b2;
    b1.add("eventA", {0x1});
    b2.add("eventA", {0x1, 0x2});

    b1.add(b2); // Should only add 0x2

    ASSERT_EQ(b1.metric_events.size(), 2);
    EXPECT_EQ(b1.metric_events[0].first, "eventA");
    EXPECT_EQ(b1.metric_events[0].second, 0x1);
    EXPECT_EQ(b1.metric_events[1].first, "eventA");
    EXPECT_EQ(b1.metric_events[1].second, 0x2);
}

TEST(MetricBuilderTest, MultipleMetricsShareEvents)
{
    MetricBuilder builder;
    builder.add("a", {0x1}).add("b", {0x2});
    builder.build("RatioAtoB", [](const auto &m)
                  { return static_cast<double>(m.at("a")) / m.at("b"); });
    builder.build("Sum", [](const auto &m)
                  { return static_cast<double>(m.at("a") + m.at("b")); });

    std::unordered_map<std::string, uint64_t> results = {
        {"a", 30}, {"b", 10}};

    auto metrics = builder.calculate(results);

    // Store in map for unordered comparison
    std::unordered_map<std::string, double> computed;
    for (const auto &[name, value] : metrics)
        computed[name] = value;

    ASSERT_EQ(computed.size(), 2);
    EXPECT_DOUBLE_EQ(computed["RatioAtoB"], 3.0);
    EXPECT_DOUBLE_EQ(computed["Sum"], 40.0);
}

TEST(MetricBuilderTest, ThrowsOnMissingMetric)
{
    MetricBuilder builder;
    EXPECT_THROW({ builder.metric_calculation_func("nonexistent"); }, std::runtime_error);
}

TEST(MetricBuilderTest, MetricCalculationMissingInputKeyThrows)
{
    MetricBuilder builder;
    builder.build("NeedsX", [](const auto &m)
                  {
                      return m.at("x"); // Will throw
                  });

    std::unordered_map<std::string, uint64_t> results = {
        {"y", 10}};

    EXPECT_THROW({ builder.calculate(results); }, std::out_of_range);
}

TEST(MetricBuilderTest, IPCAndMPKIIntegration)
{
    MetricBuilder builder;
    builder.add("inst_retired", {0x00c0})
        .add("cpu_cycles", {0x003c})
        .add("cache_misses", {0x412e})
        .build("IPC", [](const auto &m)
               { return m.at("inst_retired") / static_cast<double>(m.at("cpu_cycles")); })
        .build("MPKI", [](const auto &m)
               { return m.at("cache_misses") * 1000.0 / m.at("inst_retired"); });

    std::unordered_map<std::string, uint64_t> results = {
        {"inst_retired", 10'000'000},
        {"cpu_cycles", 20'000'000},
        {"cache_misses", 50'000}};

    auto metrics = builder.calculate(results);
    ASSERT_EQ(metrics.size(), 2);

    std::unordered_map<std::string, double> expected = {
        {"IPC", 0.5},
        {"MPKI", 5.0}};

    for (const auto &[name, value] : metrics)
    {
        ASSERT_NE(expected.find(name), expected.end());
        EXPECT_DOUBLE_EQ(value, expected[name]);
    }
}
