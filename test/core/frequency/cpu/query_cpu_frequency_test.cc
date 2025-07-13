// test_query_cpu_frequency_real.cc

#include <iostream>
#include <algorithm>
#include <gtest/gtest.h>
#include "utils/utils.hh"
#include "core/frequency/cpu/cpu_frequency.hh"
#include "core/frequency/cpu/query_cpu_frequency.hh"

namespace fs = std::filesystem;
using namespace optkit::core::frequency;

class QueryCPUFrequencyRealTest : public ::testing::Test
{
protected:
    int32_t core = 0;

    std::string get_driver()
    {
        return QueryCPUFrequency::get_scaling_driver(core);
    }

    bool is_modern_driver()
    {
        static const std::vector<std::string> modern = {
            "intel_pstate", "amd-pstate", "amd-pstate-epp"};
        std::string drv = get_driver();
        return std::find(modern.begin(), modern.end(), drv) != modern.end();
    }

    std::string path(const std::string &filename)
    {
        return "/sys/devices/system/cpu/cpu" + std::to_string(core) + "/cpufreq/" + filename;
    }

    bool exists(const std::string &filename)
    {
        return optkit::utils::is_path_exists(path(filename));
    }
};

TEST_F(QueryCPUFrequencyRealTest, ScalingDriver_ShouldBePresent)
{
    std::string driver = get_driver();
    std::cout << "Scaling driver: " << driver << "\n";
    ASSERT_FALSE(driver.empty());
}

TEST_F(QueryCPUFrequencyRealTest, ScalingAvailableFrequencies_MayBeEmptyForModernDriver)
{
    if (!exists("scaling_available_frequencies"))
        GTEST_SKIP() << "File not present: scaling_available_frequencies";

    auto freqs = QueryCPUFrequency::get_scaling_available_frequencies(core);
    std::cout << "Available frequencies count: " << freqs.size() << "\n";

    if (is_modern_driver())
    {
        EXPECT_TRUE(freqs.empty()) << "Expected empty frequencies for modern driver";
    }
    else
    {
        EXPECT_FALSE(freqs.empty()) << "Expected non-empty frequencies for legacy driver";
        for (auto f : freqs)
            std::cout << "  " << f << " Hz\n";
    }
}

TEST_F(QueryCPUFrequencyRealTest, BIOSLimit_MayBeAbsent)
{
    if (!exists("bios_limit"))
        GTEST_SKIP() << "bios_limit not present";

    int64_t bios = QueryCPUFrequency::get_bios_limit(core);
    std::cout << "BIOS limit: " << bios << " Hz\n";
    EXPECT_GT(bios, 0);
}

TEST_F(QueryCPUFrequencyRealTest, ScalingGovernor_ShouldBeReadable)
{
    if (!exists("scaling_governor"))
        GTEST_SKIP() << "scaling_governor not present";

    std::string gov = QueryCPUFrequency::get_scaling_governor(core);
    std::cout << "Current governor: " << gov << "\n";
    ASSERT_FALSE(gov.empty());
}

TEST_F(QueryCPUFrequencyRealTest, AvailableGovernors_ShouldBeListIfPresent)
{
    if (!exists("scaling_available_governors"))
        GTEST_SKIP() << "scaling_available_governors not present";

    auto govs = QueryCPUFrequency::get_available_governors(core);
    ASSERT_FALSE(govs.empty());
    for (const auto &g : govs)
        std::cout << "Available governor: " << g << ", ";
    std::cout << std::endl;
}

TEST_F(QueryCPUFrequencyRealTest, ScalingMinMaxLimits_ShouldBeValidIfPresent)
{
    if (!exists("scaling_min_freq") || !exists("scaling_max_freq"))
        GTEST_SKIP() << "Min/max scaling files not present";

    int64_t min = QueryCPUFrequency::get_scaling_min_limit(core);
    int64_t max = QueryCPUFrequency::get_scaling_max_limit(core);

    std::cout << "Scaling min: " << min << " Hz, max: " << max << " Hz\n";
    EXPECT_GE(min, 0);
    EXPECT_GE(max, 0);
    EXPECT_LE(min, max);
}

TEST_F(QueryCPUFrequencyRealTest, CpuinfoMinMaxFreq_ShouldBeValidIfPresent)
{
    if (!exists("cpuinfo_min_freq") || !exists("cpuinfo_max_freq"))
        GTEST_SKIP() << "cpuinfo min/max not present";

    double min = CPUFrequency::convert_frequency_with_unit(std::to_string(QueryCPUFrequency::get_cpuinfo_min_freq(core)) + "hz", CPUFrequency::Unit::GHz);
    double max = CPUFrequency::convert_frequency_with_unit(std::to_string(QueryCPUFrequency::get_cpuinfo_max_freq(core)) + "hz", CPUFrequency::Unit::GHz);

    std::cout << "Cpuinfo min: " << min << " Hz, max: " << max << " Hz\n";
    
    EXPECT_GE(min, 0);
    EXPECT_GE(max, 0);
    EXPECT_LE(min, max);
}
