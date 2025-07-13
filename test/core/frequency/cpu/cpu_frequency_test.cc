#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>

#include "core/frequency/cpu/cpu_frequency.hh"
using namespace optkit::core::frequency;

class CPUFrequencyRealTest : public ::testing::Test
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

TEST_F(CPUFrequencyRealTest, ConvertFrequencyWithUnit_ShouldWorkCorrectly)
{
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3.5GHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500MHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500000KHz"), 3500000000);
    EXPECT_EQ(CPUFrequency::convert_frequency_with_unit("3500000000"), 3500000000);
}

TEST_F(CPUFrequencyRealTest, ConvertFrequencyWithInvalidUnit_ShouldThrow)
{
    EXPECT_THROW(CPUFrequency::convert_frequency_with_unit("3.5abc"), std::invalid_argument);
    EXPECT_THROW(CPUFrequency::convert_frequency_with_unit("abc"), std::invalid_argument);
}

//
// Read-only core frequency queries
//

TEST_F(CPUFrequencyRealTest, GetCoreFrequency_ShouldReturnPositiveIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "scaling_cur_freq not available";

    int64_t freq = CPUFrequency::get_core_frequency(cpu);
    std::cout << "Core " << cpu << " freq: " << freq << " Hz\n";
    EXPECT_GT(freq, 0);
}

TEST_F(CPUFrequencyRealTest, GetCoreFrequencies_ShouldReturnListIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    auto freqs = CPUFrequency::get_core_frequencies(socket);
    ASSERT_FALSE(freqs.empty());
    for (auto f : freqs)
    {
        std::cout << "Core freq: " << f << " Hz\n";
        EXPECT_GT(f, 0);
    }
}

TEST_F(CPUFrequencyRealTest, GetCoreFrequencyRange_ShouldBeValidIfAvailable)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    auto freqs = CPUFrequency::get_core_frequency(0, 3, socket);
    ASSERT_FALSE(freqs.empty());
    for (auto f : freqs)
    {
        std::cout << "Range freq: " << f << " Hz\n";
        EXPECT_GT(f, 0);
    }
}

//
// Uncore MSR-based queries
//

TEST_F(CPUFrequencyRealTest, GetUncoreFrequency_ShouldReturnPositiveIfMSRAvailable)
{
    try
    {
        int64_t freq = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Uncore frequency: " << freq << " Hz\n";
        EXPECT_GT(freq, 0);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR read not supported on this system";
    }
}

TEST_F(CPUFrequencyRealTest, GetUncoreMinMax_ShouldReturnValidValues)
{
    try
    {
        auto [min, max] = CPUFrequency::get_uncore_min_max(socket);
        std::cout << "Uncore min: " << min << " Hz, max: " << max << " Hz\n";
        EXPECT_GE(min, 0);
        EXPECT_GE(max, 0);
        EXPECT_LE(min, max);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR uncore limit not readable on this system";
    }
}

TEST_F(CPUFrequencyRealTest, SetAndResetCoreFrequencySweep)
{
    if (!exists("scaling_cur_freq"))
        GTEST_SKIP() << "cpufreq not available";

    int64_t original = CPUFrequency::get_core_frequency(cpu);
    ASSERT_GT(original, 0);

    int64_t min_freq = QueryCPUFrequency::get_cpuinfo_min_freq(cpu); // in Hz
    int64_t max_freq = QueryCPUFrequency::get_cpuinfo_max_freq(cpu); // in Hz
    if (min_freq <= 0 || max_freq <= 0 || min_freq >= max_freq)
        GTEST_SKIP() << "Invalid CPU frequency range: min=" << min_freq << " max=" << max_freq;

    const int64_t step = 200'000'000;                      // 0.2 GHz in Hz
    const auto wait_time = std::chrono::milliseconds(500); // 500 ms wait

    // Round up min_freq to nearest 0.2 GHz
    int64_t start_freq = ((min_freq + step - 1) / step) * step;
    std::cout << "start_freq=" << start_freq / 1.0e9 << "\n";

    for (int64_t freq = start_freq; freq <= max_freq; freq += step)
    {
        std::cout << "Setting CPU " << cpu << " to " << freq  / 1.0e9 << " GHz\n";
        CPUFrequency::set_core_frequency(freq, cpu, socket);

        std::this_thread::sleep_for(wait_time); // 🕒 wait for frequency to stabilize

        int64_t read_freq = CPUFrequency::get_core_frequency(cpu);
        std::cout << " -> Read back: " << read_freq / 1.0e9 << " GHz\n";
        EXPECT_NEAR(read_freq, freq, 10'000'000); // ±10 MHz tolerance
    }

    // Reset
    CPUFrequency::reset_core_frequency(socket);
    std::this_thread::sleep_for(wait_time);
    int64_t reset_freq = CPUFrequency::get_core_frequency(cpu);
    std::cout << "Reset core frequency to: " << reset_freq / 1.0e9  << " GHz\n";
    EXPECT_GT(reset_freq, 0);
}

TEST_F(CPUFrequencyRealTest, DISABLED_SetAndResetUncoreFrequency)
{
    try
    {
        auto [min, max] = CPUFrequency::get_uncore_min_max(socket);
        int64_t test_freq = (min + max) / 2;

        CPUFrequency::set_uncore_frequency(test_freq, socket);
        int64_t current = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Set uncore frequency: " << current << " Hz\n";

        CPUFrequency::reset_uncore_frequency(socket);
        int64_t reset = CPUFrequency::get_uncore_frequency(socket);
        std::cout << "Reset uncore frequency: " << reset << " Hz\n";

        EXPECT_GT(current, 0);
        EXPECT_GT(reset, 0);
    }
    catch (...)
    {
        GTEST_SKIP() << "MSR write not supported";
    }
}
