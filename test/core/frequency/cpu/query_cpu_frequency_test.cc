// test_query_cpu_frequency_real.cc

#include <iostream>
#include <algorithm>
#include <gtest/gtest.h>
#include "utils/utils.hh"

#include "core/query.hh"
#include "core/frequency/cpu/cpu_frequency.hh"
#include "core/frequency/cpu/query_cpu_frequency.hh"

namespace fs = std::filesystem;
using namespace optkit::core::frequency;

class QueryCPUFrequencyTest : public ::testing::Test
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

TEST_F(QueryCPUFrequencyTest, ScalingDriver_ShouldBePresent)
{
    std::string driver = get_driver();
    std::cout << "Scaling driver: " << driver << "\n";
    ASSERT_FALSE(driver.empty());
}

TEST_F(QueryCPUFrequencyTest, ScalingAvailableFrequencies_MayBeEmptyForModernDriver)
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

TEST_F(QueryCPUFrequencyTest, BIOSLimit_MayBeAbsent)
{
    if (!exists("bios_limit"))
        GTEST_SKIP() << "bios_limit not present";

    int64_t bios = QueryCPUFrequency::get_bios_limit(core);
    std::cout << "BIOS limit: " << bios << " Hz\n";
    EXPECT_GT(bios, 0);
}

TEST_F(QueryCPUFrequencyTest, ScalingGovernor_ShouldBeReadable)
{
    if (!exists("scaling_governor"))
        GTEST_SKIP() << "scaling_governor not present";

    std::string gov = QueryCPUFrequency::get_scaling_governor(core);
    std::cout << "Current governor: " << gov << "\n";
    ASSERT_FALSE(gov.empty());
}

TEST_F(QueryCPUFrequencyTest, AvailableGovernors_ShouldBeListIfPresent)
{
    if (!exists("scaling_available_governors"))
        GTEST_SKIP() << "scaling_available_governors not present";

    auto govs = QueryCPUFrequency::get_available_governors(core);
    ASSERT_FALSE(govs.empty());
    for (const auto &g : govs)
        std::cout << "Available governor: " << g << ", ";
    std::cout << std::endl;
}

TEST_F(QueryCPUFrequencyTest, ScalingMinMaxLimits_ShouldBeValidIfPresent)
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

TEST_F(QueryCPUFrequencyTest, CpuinfoMinMaxFreq_ShouldBeValidIfPresent)
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

TEST_F(QueryCPUFrequencyTest, SetValidGovernorOnEachSocket)
{
    const auto &cpu_packages = optkit::core::Query::detect_cpu_packages();
    if (cpu_packages.empty())
        GTEST_SKIP() << "No CPU package info detected";

    for (const auto &[socket, cores] : cpu_packages)
    {
        if (cores.empty())
        {
            GTEST_SKIP() << "No cores found for socket " << socket;
            continue;
        }

        int32_t sample_core = cores.front();

        auto available_governors = QueryCPUFrequency::get_available_governors(sample_core);
        if (available_governors.empty())
        {
            GTEST_SKIP() << "No available governors for socket " << socket;
            continue;
        }

        std::string valid_gov = available_governors.front();

        // Backup original governors
        std::map<int32_t, std::string> original_govs;
        for (int32_t core : cores)
            original_govs[core] = QueryCPUFrequency::get_scaling_governor(core);

        // Set new governor on socket
        QueryCPUFrequency::set_scaling_governor(valid_gov, socket); 

        // Verify governor applied on all cores
        for (int32_t core : cores)
        {
            std::string core_gov = QueryCPUFrequency::get_scaling_governor(core);
            EXPECT_EQ(core_gov, valid_gov) << "Core " << core << " governor mismatch on socket " << socket;
        }

        // Restore original governors
        for (const auto &[core, orig_gov] : original_govs)
        {
            QueryCPUFrequency::set_scaling_governor_percore(orig_gov, core); 
            std::string restored_gov = QueryCPUFrequency::get_scaling_governor(core);
            EXPECT_EQ(restored_gov, orig_gov) << "Failed to restore governor on core " << core;
        }
    }
}

TEST_F(QueryCPUFrequencyTest, SetInvalidGovernorOnEachSocket)
{
    const auto &cpu_packages = optkit::core::Query::detect_cpu_packages();
    if (cpu_packages.empty())
        GTEST_SKIP() << "No CPU package info detected";

    std::string invalid_gov = "invalid_governor_xyz";

    for (const auto &[socket, cores] : cpu_packages)
    {
        if (cores.empty())
        {
            GTEST_SKIP() << "No cores found for socket " << socket;
            continue;
        }

        // Backup original governors
        std::map<int32_t, std::string> original_govs;
        for (int32_t core : cores)
            original_govs[core] = QueryCPUFrequency::get_scaling_governor(core);

        // Should not throw, but won't apply invalid governor
        QueryCPUFrequency::set_scaling_governor(invalid_gov, socket);

        // Verify none of the cores have invalid governor set
        for (int32_t core : cores)
        {
            std::string core_gov = QueryCPUFrequency::get_scaling_governor(core);
            EXPECT_NE(core_gov, invalid_gov) << "Core " << core << " has invalid governor set on socket " << socket;
        }

        // Restore original governors in case anything changed
        for (const auto &[core, orig_gov] : original_govs)
        {
            ASSERT_NO_THROW(QueryCPUFrequency::set_scaling_governor_percore(orig_gov, core));
            std::string restored_gov = QueryCPUFrequency::get_scaling_governor(core);
            EXPECT_EQ(restored_gov, orig_gov) << "Failed to restore governor on core " << core;
        }
    }
}
