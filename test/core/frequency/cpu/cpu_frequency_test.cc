#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>

#include "core/frequency/cpu/cpu_frequency.hh"
#include "core/query.hh"

#include "common/utils.hh"
#include "utils/utils.hh"
using namespace optkit::frequency;

class CPUFrequencyTest : public ::testing::Test
{
protected:
    int16_t socket = 0;
    int16_t cpu = 0;

    bool exists(const std::string &filename)
    {
        return optkit::utils::is_path_exists("/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/" + filename);
    }
};

//
// Unit conversion tests
//

TEST_F(CPUFrequencyTest, ConvertFrequencyWithUnit_ShouldWorkCorrectly)
{
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3.5GHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500MHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500000KHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500000000"), 3500000000);
}

TEST_F(CPUFrequencyTest, ConvertFrequencyWithInvalidUnit_ShouldThrow)
{
    EXPECT_THROW(CPUFrequency::convert_frequency_with_unit("3.5abc"), std::invalid_argument);
    EXPECT_THROW(CPUFrequency::convert_frequency_with_unit("abc"), std::invalid_argument);
}

TEST_F(CPUFrequencyTest, FrequencyChangeLatency_LessThan10ms)
{
    // Measure how long frequency changes take
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < OPTKIT_ENV_CPU_NUM_SOCKETS; i++)
    {
        CPUFrequency::set_core_frequency(QueryCPUFrequency::get_cpuinfo_max_freq(0), i);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Frequency change for all sockets took: " << duration.count() << " ms\n";

    for (size_t i = 0; i < OPTKIT_ENV_CPU_NUM_SOCKETS; i++)
    {
        CPUFrequency::reset_core_frequency(i);
    }
    EXPECT_LT(duration.count(), 10); // Should be < 10ms
}

TEST_F(CPUFrequencyTest, InvalidSocketNumbers)
{
    // Test with socket = -1, 999, etc.
    EXPECT_EQ(CPUFrequency::get_core_frequency(-1), -1);
    CPUFrequency::set_core_frequency(QueryCPUFrequency::get_cpuinfo_max_freq(0), -1); // Expect OPTKIT ERROR
}

TEST_F(CPUFrequencyTest, InvalidCPUNumbers)
{
    // Test with cpu = -1, 9999, offline cores
    EXPECT_EQ(CPUFrequency::get_core_frequency(-1), -1);
}

TEST_F(CPUFrequencyTest, FrequencyOutOfRange)
{
    // Test frequencies way below min or above max
    CPUFrequency::set_core_frequency(1, socket);
    CPUFrequency::set_core_frequency(999999999999, socket);
}

//
// Read-only core frequency queries
//

TEST_F(CPUFrequencyTest, GetCoreFrequency_ShouldReturnPositiveIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "scaling_cur_freq not available";

    int64_t freq = CPUFrequency::get_core_frequency(cpu);
    std::cout << "Core " << cpu << " freq: " << freq << " KHz\n";
    EXPECT_GT(freq, 0);
}

TEST_F(CPUFrequencyTest, GetCoreFrequencies_ShouldReturnListIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    auto freqs = CPUFrequency::get_core_frequencies(socket);
    ASSERT_FALSE(freqs.empty());
    for (auto f : freqs)
    {
        std::cout << "Core freq: " << f << " KHz\n";
        EXPECT_GT(f, 0);
    }
}

TEST_F(CPUFrequencyTest, GetCoreFrequencyRange_ShouldBeValidIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    auto freqs = CPUFrequency::get_core_frequency(0, 3, socket);
    ASSERT_FALSE(freqs.empty());
    for (auto f : freqs)
    {
        std::cout << "Range freq: " << f << " KHz\n";
        EXPECT_GT(f, 0);
    }
}

TEST_F(CPUFrequencyTest, SetAndResetCoreFrequencySweepAllSockets)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    const int64_t step = 200'000;                           // 0.2 GHz in KHz
    const auto wait_time = std::chrono::milliseconds(1000); // 1 second

    for (const auto &[socket, cores] : optkit::Query::detect_cpu_packages())
    {
        if (cores.empty())
            continue;

        size_t total_freq_tests = 0;
        double accepted_freq = 0;

        int64_t min_freq = QueryCPUFrequency::get_cpuinfo_min_freq(cores.front());
        int64_t max_freq = QueryCPUFrequency::get_cpuinfo_max_freq(cores.front());

        if (min_freq <= 0 || max_freq <= 0 || min_freq >= max_freq)
        {
            std::cout << "Invalid frequency range on socket " << socket
                      << ": min=" << min_freq << " max=" << max_freq << "\n";
            continue;
        }

        int64_t start_freq = ((min_freq + step - 1) / step) * step;
        std::cout << "[Socket " << socket << "] Sweep start: " << start_freq / 1.0e6 << " GHz\n";

        for (int64_t freq = start_freq; freq <= max_freq; freq += step)
        {
            std::cout << "\tSetting all cores on socket " << socket << " to " << freq / 1.0e6 << " GHz\n";
            CPUFrequency::set_core_frequency(freq, socket);

            std::this_thread::sleep_for(wait_time);

            auto read_freqs = CPUFrequency::get_core_frequencies(socket);
            int64_t sum_freq = 0;
            for (size_t i = 0; i < read_freqs.size(); ++i)
            {
                std::cout << "\t\tCore " << cores[i] << " read: " << read_freqs[i] / 1.0e6 << " GHz\n";
                sum_freq += read_freqs[i];
            }

            double avg_freq = static_cast<double>(sum_freq) / static_cast<int64_t>(read_freqs.size());
            std::cout << "\t\t[Socket " << socket << "] Avg read freq: " << avg_freq / 1.0e6 << " GHz\n";
            if (std::abs(avg_freq - freq) <= step)
            {
                std::cout << "\t\tStatus: ACCEPT\n";
                accepted_freq++;
            }
            else
            {
                std::cout << "\t\tStatus: REJECT\n";
            }
            total_freq_tests++;
            // EXPECT_NEAR(avg_freq, freq, step); // ±0.2 GHz in KHz
        }

        double acceptance_rate = 100.0 * (accepted_freq / total_freq_tests);
        std::cout << "Acceptance Rate:" << acceptance_rate << "%\n";
        EXPECT_TRUE(acceptance_rate >= 75.0); // 75% freqs must be accepted to pass.

        // Reset all core frequencies for socket
        CPUFrequency::reset_core_frequency(socket);
        std::this_thread::sleep_for(wait_time);

        std::cout << "[Socket " << socket << "] Verifying reset...\n";

        int64_t total_min = 0, total_max = 0;
        for (int32_t cpu : cores)
        {
            total_min += QueryCPUFrequency::get_scaling_min_limit(cpu);
            total_max += QueryCPUFrequency::get_scaling_max_limit(cpu);
        }

        double avg_min = static_cast<double>(total_min) / static_cast<int64_t>(cores.size());
        double avg_max = static_cast<double>(total_max) / static_cast<int64_t>(cores.size());

        std::cout << "\tAvg scaling_min = " << avg_min / 1.0e6 << " GHz, expected = " << min_freq / 1.0e6 << " GHz\n";
        std::cout << "\tAvg scaling_max = " << avg_max / 1.0e6 << " GHz, expected = " << max_freq / 1.0e6 << " GHz\n";

        EXPECT_NEAR(avg_min, min_freq, ERROR_RATE);
        EXPECT_NEAR(avg_max, max_freq, ERROR_RATE);
    }
}

#if OPTKIT_ENV_CPU_INTEL
//
// Uncore MSR-based queries
//

TEST_F(CPUFrequencyTest, GetUncoreFrequency_ShouldReturnPositiveIfMSRAvailable)
{
    try
    {
        int64_t freq = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Uncore frequency: " << freq << " KHz\n";
        EXPECT_GT(freq, 0);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR read not supported on this system";
    }
}

TEST_F(CPUFrequencyTest, GetUncoreMinMax_ShouldReturnValidValues)
{
    try
    {
        auto [min, max] = CPUFrequency::get_uncore_min_max(socket);
        std::cout << "Uncore min: " << min << " KHz, max: " << max << " KHz\n";
        EXPECT_GE(min, 0);
        EXPECT_GE(max, 0);
        EXPECT_LE(min, max);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR uncore limit not readable on this system";
    }
}

TEST_F(CPUFrequencyTest, DISABLED_SetAndResetUncoreFrequency)
{
    try
    {
        auto [min, max] = CPUFrequency::get_uncore_min_max(socket);
        int64_t test_freq = (min + max) / 2;

        CPUFrequency::set_uncore_frequency(test_freq, socket);
        int64_t current = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Set uncore frequency: " << current << " KHz\n";

        CPUFrequency::reset_uncore_frequency(socket);
        int64_t reset = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Reset uncore frequency: " << reset << " KHz\n";

        EXPECT_GT(current, 0);
        EXPECT_GT(reset, 0);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR write not supported";
    }
}

#endif
