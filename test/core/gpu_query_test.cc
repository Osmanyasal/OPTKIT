#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include "core/gpu_query.hh"
#include "common/utils.hh"

using namespace optkit::gpu;

// RAII class to manage GPU vendor initialization and shutdown
/**
 * @brief Since we init inside OPTKIT and call it in main_test.cc, this class should return the available vendors only.
 *
 */
class GPUVendors
{
public:
    GPUVendors()
    {
        // init all vendors
        for (GpuVendor vendor = GpuVendor::NVIDIA; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
        {
            bool is_vendor_available = Query::init(vendor);
            if (is_vendor_available)
            {
                if (!Query::is_device_exists(vendor))
                {
                    std::cout << "Device doesn't exists for vendor:" << to_string(vendor) << "\n";
                    Query::shutdown(vendor);
                    continue;
                }

                available_vendors.push_back(vendor);
                std::cout << "Initialized vendor " << to_string(vendor) << " successfully." << std::endl;
            }
        }
    }
    ~GPUVendors()
    {
        for (const auto &vendor : available_vendors)
        {
            if (Query::shutdown(vendor))
                std::cout << "Shutdown vendor " << to_string(vendor) << " successfully." << std::endl;
            else
                std::cout << "Failed to shutdown vendor " << to_string(vendor) << "." << std::endl;
        }
    }
    std::vector<GpuVendor> available_vendors;
};

TEST(GpuQueryTest, InitializationAndShutdown)
{
    // init
    for (GpuVendor vendor = GpuVendor::NVIDIA; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        bool is_vendor_available = Query::init(vendor);
        if (is_vendor_available)
        {
            if (!Query::is_device_exists(vendor))
                continue;

            uint32_t count = 0;
            if (Query::get_device_count(vendor, count))
                std::cout << "Vendor " << to_string(vendor) << " has " << count << " devices." << std::endl;
        }
    }

    // shutdown
    for (GpuVendor vendor = GpuVendor::NVIDIA; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        if (Query::shutdown(vendor))
            std::cout << "Shutdown vendor " << to_string(vendor) << " successfully." << std::endl;
    }
}

TEST(GpuQueryTest, MultipleInitializationsSafe)
{
    for (GpuVendor vendor = GpuVendor::NVIDIA; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        bool result1 = Query::init(vendor);
        bool result2 = Query::init(vendor);
        bool result3 = Query::init(vendor);

        if (Query::is_device_exists(vendor))
        {
            EXPECT_TRUE(result1);
            EXPECT_TRUE(result2);
            EXPECT_TRUE(result3);
            Query::shutdown(vendor);
        }
        else
        {
            EXPECT_FALSE(result1);
            EXPECT_FALSE(result2);
            EXPECT_FALSE(result3);
            continue;
        }
    }
}

// Device count tests
TEST(GpuQueryTest, DeviceCountQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t vendor_count = 0;
        EXPECT_TRUE(Query::get_device_count(vendor, vendor_count));
        EXPECT_GE(vendor_count, 0); // Should be 0 or more
        std::cout << "Found " << vendor_count << " devices for vendor " << to_string(vendor) << std::endl;
    }
}

// Basic info tests
TEST(GpuQueryTest, BasicInfoQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuBasicInfo basic_info;
                EXPECT_TRUE(Query::get_basic_info(vendor, device_index, basic_info));

                EXPECT_EQ(basic_info.vendor, vendor);
                EXPECT_EQ(basic_info.vendor_string, to_string(vendor));
                EXPECT_EQ(basic_info.id, device_index);
                EXPECT_FALSE(basic_info.device_name.empty());
                EXPECT_GT(basic_info.architecture, 0); // Architecture should be a positive integer

                std::cout << to_string(vendor) << " Device[" << device_index << "]: " << basic_info.device_name << std::endl;
                std::cout << "Architecture: " << basic_info.architecture << std::endl;
            }
        }
    }
}

TEST(GpuQueryTest, InvalidDeviceIndexHandling)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuBasicInfo basic_info;
                EXPECT_FALSE(Query::get_basic_info(vendor, device_index + 1000, basic_info));
            }
        }
    }
}

// Power monitoring tests
TEST(GpuQueryTest, PowerMonitoring)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                double power_watts = 0.0;
                if (Query::get_device_power(vendor, device_index, power_watts))
                {
                    EXPECT_GE(power_watts, 0.0);
                    EXPECT_LE(power_watts, 1000.0); // Reasonable upper bound
                    std::cout << to_string(vendor) << " Device[" << device_index << "] power: " << power_watts << " Watts" << std::endl;
                }
            }
        }
    }
}

TEST(GpuQueryTest, PowerLimitsQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                double power_watts = 0.0;
                if (Query::get_device_power(vendor, device_index, power_watts))
                {
                    std::cout << to_string(vendor) << " Device[" << device_index << "] current power: " << power_watts << " Watts" << std::endl;
                    double limit_watts, default_power, min_limit, max_limit;
                    bool is_configurable;

                    bool limits_available = Query::get_device_power_limits(
                        vendor, device_index, limit_watts, default_power,
                        min_limit, max_limit, is_configurable);

                    if (limits_available)
                    {
                        EXPECT_GE(limit_watts, 0.0);
                        EXPECT_GE(default_power, 0.0);
                        EXPECT_LE(min_limit, max_limit);

                        std::cout << to_string(vendor) << " Device[" << device_index << "] power limits: "
                                  << min_limit << "W - " << max_limit << "W"
                                  << " (current: " << limit_watts << "W)" << std::endl;
                    }
                }
            }
        }
    }
}

// Temperature monitoring tests
TEST(GpuQueryTest, TemperatureMonitoring)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuTemperatureInfo temp_info = {};
                if (Query::get_temperature_info(vendor, device_index, temp_info))
                {
                    EXPECT_GE(temp_info.current_device_temperature_celsius, 0.0);
                    EXPECT_LE(temp_info.current_device_temperature_celsius, 150.0); // Reasonable temperature range

                    std::cout << to_string(vendor) << " Device[" << device_index << "] temperatures: GPU=" << temp_info.current_device_temperature_celsius
                              << "°C, Memory=" << temp_info.current_memory_temperature_celsius << "°C" << std::endl;
                }
            }
        }
    }
}

// Memory info tests
TEST(GpuQueryTest, MemoryInfoQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuMemoryInfo memory_info = {};
                EXPECT_TRUE(Query::get_memory_info(vendor, device_index, memory_info));

                if (memory_info.total_global_memory_MBytes > 0)
                {
                    EXPECT_GT(memory_info.total_global_memory_MBytes, 0);
                    EXPECT_LE(memory_info.used_memory_MBytes, memory_info.total_global_memory_MBytes);
                    auto diff = memory_info.total_global_memory_MBytes - memory_info.used_memory_MBytes;
                    EXPECT_NEAR(memory_info.free_memory_MBytes, diff, diff * ERROR_RATE);

                    std::cout << to_string(vendor) << " Device[" << device_index << "] memory: "
                              << memory_info.used_memory_MBytes << "/"
                              << memory_info.free_memory_MBytes << "/"
                              << memory_info.total_global_memory_MBytes << " MB used" << std::endl;
                }
            }
        }
    }
}

// Compute capability tests
TEST(GpuQueryTest, ComputeCapabilityQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuComputeInfo compute_info = {};
                Query::get_compute_info(vendor, device_index, compute_info);

                if (compute_info.compute_capability_major > 0)
                {
                    EXPECT_GT(compute_info.compute_capability_major, 0);
                    EXPECT_GE(compute_info.compute_capability_minor, 0);
                    EXPECT_GE(compute_info.warp_size, 32); // NVIDIA warp size is always 32, amd goes to 64 so it is 32 at least or more.

                    std::cout << to_string(vendor) << " Device[" << device_index << "] compute capability: "
                              << compute_info.compute_capability_major << "."
                              << compute_info.compute_capability_minor << std::endl;
                    std::cout << to_string(vendor) << " Device[" << device_index << "] Multiprocessors: "
                              << compute_info.multiprocessor_count << ", Total cores: "
                              << compute_info.total_cores << std::endl;
                }
            }
        }
    }
}

// Clock information tests
TEST(GpuQueryTest, ClockInfoQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuClockInfo clock_info = {};
                EXPECT_TRUE(Query::get_clock_info(vendor, device_index, clock_info));

                if (clock_info.current_graphics_clock_MHz > 0)
                {
                    EXPECT_GT(clock_info.current_graphics_clock_MHz, 0);
                    EXPECT_GT(clock_info.current_memory_clock_MHz, 0);
                    EXPECT_LE(clock_info.current_graphics_clock_MHz, clock_info.max_graphics_clock_MHz);

                    std::cout << to_string(vendor) << " Device[" << device_index << "] clocks: Graphics="
                              << clock_info.current_graphics_clock_MHz << "MHz, Memory="
                              << clock_info.current_memory_clock_MHz << "MHz" << std::endl;
                }
            }
        }
    }
}

// Version information tests
TEST(GpuQueryTest, VersionInfoQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        Query::get_device_count(vendor, device_count);
        if (device_count > 0)
        {
            for (size_t device_index = 0; device_index < device_count; device_index++)
            {
                GpuVersionInfo version_info = {};
                EXPECT_TRUE(Query::get_version_info(vendor, device_index, version_info));

                EXPECT_GT(version_info.driver_major_minor, 0.0);
                EXPECT_FALSE(version_info.driver_version_string.empty());
                EXPECT_FALSE(version_info.library_version_string.empty());

                std::cout << to_string(vendor) << " Device[" << device_index << "] Driver version: " << version_info.driver_version_string
                          << ", Library: " << version_info.library_version_string << std::endl;
            }
        }
    }
}

// Comprehensive device query test
TEST(GpuQueryTest, ComprehensiveDeviceQuery)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    // Test NVIDIA if available
    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            GpuDeviceInfo device_info;
            EXPECT_TRUE(Query::device_query(vendor, 0, device_info));

            // Verify all sub-structures are populated
            EXPECT_EQ(device_info.basic.vendor, vendor);
            EXPECT_FALSE(device_info.basic.device_name.empty());
            EXPECT_GT(device_info.version.driver_major_minor, 0.0);

            std::cout << "=== Comprehensive " << to_string(vendor) << " Device 0 Info ===" << std::endl;
            std::cout << "Name: " << device_info.basic.device_name << std::endl;
            std::cout << "Architecture: " << device_info.basic.architecture << std::endl;
            std::cout << "Driver: " << device_info.version.driver_version_string << std::endl;
            std::cout << "Memory: " << device_info.memory.total_global_memory_MBytes << " MB" << std::endl;
            std::cout << "Compute: " << device_info.compute.compute_capability_major
                      << "." << device_info.compute.compute_capability_minor << std::endl;
        }
    }
}

// Utility function tests
TEST(GpuQueryTest, UtilityFunctions)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            // Test warp size
            uint32_t warp_size = 0;
            EXPECT_TRUE(Query::get_warp_size(vendor, 0, warp_size));
            EXPECT_GE(warp_size, 32); // AMD can be 32 or 64

            // Test device name
            std::string device_name;
            EXPECT_TRUE(Query::get_device_name(vendor, 0, device_name));
            EXPECT_FALSE(device_name.empty());

            // Test architecture
            uint32_t architecture = 0;
            EXPECT_TRUE(Query::get_architecture(vendor, 0, architecture));
            EXPECT_GT(architecture, 0);

            std::cout << to_string(vendor) << " Device 0 utility info:" << std::endl;
            std::cout << "  Warp size: " << warp_size << std::endl;
            std::cout << "  Name: " << device_name << std::endl;
            std::cout << "  Architecture: " << architecture << std::endl;
        }
    }
}

// Stress test - query all devices
TEST(GpuQueryTest, StressTestAllDevices)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            std::cout << "Testing all " << device_count << " " << to_string(vendor) << " devices" << std::endl;

            for (uint32_t i = 0; i < device_count; ++i)
            {
                GpuBasicInfo basic_info;
                EXPECT_TRUE(Query::get_basic_info(vendor, i, basic_info));

                double power = 0.0;
                Query::get_device_power(vendor, i, power); // May fail on some devices

                GpuTemperatureInfo temp_info = {};
                Query::get_temperature_info(vendor, i, temp_info); // May fail

                std::cout << "  Device " << i << ": " << basic_info.device_name
                          << " (Power: " << power << "W, Temp: "
                          << temp_info.current_device_temperature_celsius << "°C)" << std::endl;
            }
        }
    }
}

// Error handling tests
TEST(GpuQueryTest, ErrorHandlingInvalidVendor)
{
    // Test with invalid vendor enum (should not crash)
    uint32_t device_count = 0;
    EXPECT_FALSE(Query::get_device_count(static_cast<GpuVendor>(999), device_count));
}

TEST(GpuQueryTest, ErrorHandlingUninitializedVendor)
{
    // Test operations on uninitialized vendor
    Query::shutdown(GpuVendor::NVIDIA); // Ensure it's shutdown

    uint32_t device_count = 0;
    EXPECT_FALSE(Query::get_device_count(GpuVendor::NVIDIA, device_count));
}

// Performance test
TEST(GpuQueryTest, PowerPerformanceTest)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            auto start = std::chrono::high_resolution_clock::now();

            // Perform 100 power queries
            for (int i = 0; i < 100; ++i)
            {
                double power = 0.0;
                Query::get_device_power(vendor, 0, power);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            std::cout << to_string(vendor) << ": 100 power queries took "
                      << duration.count() << " microseconds" << std::endl;
            EXPECT_LT(duration.count(), 1000000); // Should complete within 1 second
        }
    }
}

// Clock frequency control tests
TEST(GpuQueryTest, ClockResetTest)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            for (uint32_t device_index = 0; device_index < device_count; ++device_index)
            {
                GpuClockInfo clock_info_before = {};
                if (!Query::get_clock_info(vendor, device_index, clock_info_before))
                {
                    std::cout << to_string(vendor) << " Device[" << device_index
                              << "] does not support clock info query" << std::endl;
                    continue;
                }

                if (!clock_info_before.has_frequency_control)
                {
                    std::cout << to_string(vendor) << " Device[" << device_index
                              << "] does not support frequency control" << std::endl;
                    continue;
                }

                std::cout << "\n=== Testing Clock Reset for " << to_string(vendor)
                          << " Device[" << device_index << "] ===" << std::endl;

                std::cout << "Graphics clock before reset: "
                          << clock_info_before.current_graphics_clock_MHz << " MHz" << std::endl;
                std::cout << "Memory clock before reset: "
                          << clock_info_before.current_memory_clock_MHz << " MHz" << std::endl;
                // Try to reset clocks
                bool reset_success = Query::reset_clock(vendor, device_index);

                if (reset_success)
                {
                    std::cout << "Clock reset successful" << std::endl;

                    // Give hardware time to settle
                    usleep(500000); // 500ms

                    GpuClockInfo clock_info_after = {};
                    EXPECT_TRUE(Query::get_clock_info(vendor, device_index, clock_info_after));

                    std::cout << "Graphics clock after reset: "
                              << clock_info_after.current_graphics_clock_MHz << " MHz" << std::endl;
                    std::cout << "Memory clock after reset: "
                              << clock_info_after.current_memory_clock_MHz << " MHz" << std::endl;
                }
                else
                {
                    std::cout << "Clock reset not supported or failed (may require elevated privileges)" << std::endl;
                }
            }
        }
    }
}

TEST(GpuQueryTest, ClockSetAndVerifyTest)
{
    GPUVendors all_vendors{};              // RAII init/shutdown
    const uint32_t TOLERANCE_MHZ = 50;     // Allow 50 MHz tolerance for clock setting
    const double SUCCESS_THRESHOLD = 0.90; // Require 90% success rate

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            for (uint32_t device_index = 0; device_index < device_count; ++device_index)
            {
                GpuClockInfo clock_info = {};
                if (!Query::get_clock_info(vendor, device_index, clock_info))
                {
                    std::cout << to_string(vendor) << " Device[" << device_index
                              << "] does not support clock info query" << std::endl;
                    continue;
                }

                if (!clock_info.has_frequency_control)
                {
                    std::cout << to_string(vendor) << " Device[" << device_index
                              << "] does not support frequency control" << std::endl;
                    continue;
                }

                if (clock_info.memory_supported_clock_rates_MHz.empty() ||
                    clock_info.graphics_supported_clock_rates_MHz.empty())
                {
                    std::cout << to_string(vendor) << " Device[" << device_index
                              << "] has no supported clock rates available" << std::endl;
                    continue;
                }

                std::cout << "\n=== Testing Clock Setting for " << to_string(vendor)
                          << " Device[" << device_index << "] ===" << std::endl;

                // Save original clocks
                uint32_t original_graphics_clk = clock_info.current_graphics_clock_MHz;
                uint32_t original_memory_clk = clock_info.current_memory_clock_MHz;

                std::cout << "Original clocks - Graphics: " << original_graphics_clk
                          << " MHz, Memory: " << original_memory_clk << " MHz" << std::endl;

                uint32_t total_tests = 0;
                uint32_t successful_sets = 0;

                // Test each memory clock with its associated graphics clocks
                for (const auto &mem_gfx_pair : clock_info.graphics_supported_clock_rates_MHz)
                {
                    uint32_t mem_clk = mem_gfx_pair.first;
                    const std::vector<uint32_t> &gfx_clks = mem_gfx_pair.second;

                    std::cout << "\nTesting Memory Clock: " << mem_clk << " MHz with "
                              << gfx_clks.size() << " graphics clock(s)" << std::endl;

                    for (const auto &gfx_clk : gfx_clks)
                    {
                        total_tests++;

                        // Set the clock
                        bool set_result = Query::set_clock(vendor, device_index, mem_clk, gfx_clk);

                        if (!set_result)
                        {
                            std::cout << "  [FAILED] Could not set clocks to Mem=" << mem_clk
                                      << " MHz, Gfx=" << gfx_clk << " MHz" << std::endl;
                            continue;
                        }

                        // Give hardware time to apply the setting
                        usleep(300000); // 300ms

                        // Verify the setting
                        GpuClockInfo verify_clock_info = {};
                        if (!Query::get_clock_info(vendor, device_index, verify_clock_info))
                        {
                            std::cout << "  [FAILED] Could not query clock info after setting" << std::endl;
                            continue;
                        }

                        uint32_t actual_gfx = verify_clock_info.current_graphics_clock_MHz;
                        uint32_t actual_mem = verify_clock_info.current_memory_clock_MHz;

                        // Check if clocks are within tolerance
                        bool gfx_match = (actual_gfx >= gfx_clk - TOLERANCE_MHZ) &&
                                         (actual_gfx <= gfx_clk + TOLERANCE_MHZ);
                        bool mem_match = (actual_mem >= mem_clk - TOLERANCE_MHZ) &&
                                         (actual_mem <= mem_clk + TOLERANCE_MHZ);

                        if (gfx_match && mem_match)
                        {
                            successful_sets++;
                            std::cout << "  [PASS] Set and verified - Mem=" << mem_clk
                                      << " MHz (actual: " << actual_mem << "), Gfx=" << gfx_clk
                                      << " MHz (actual: " << actual_gfx << ")" << std::endl;
                        }
                        else
                        {
                            std::cout << "  [FAIL] Set but verification failed - Requested: Mem="
                                      << mem_clk << " MHz, Gfx=" << gfx_clk << " MHz, Got: Mem="
                                      << actual_mem << " MHz, Gfx=" << actual_gfx << " MHz" << std::endl;
                        }
                    }
                }

                // Calculate success rate
                double success_rate = (total_tests > 0) ? (static_cast<double>(successful_sets) / static_cast<double>(total_tests)) : 0.0;

                std::cout << "\n=== Clock Setting Test Results ===" << std::endl;
                std::cout << "Total tests: " << total_tests << std::endl;
                std::cout << "Successful: " << successful_sets << std::endl;
                std::cout << "Success rate: " << (success_rate * 100.0) << "%" << std::endl;

                // Restore original clocks
                std::cout << "\nRestoring original clocks..." << std::endl;
                bool restore_result = Query::set_clock(vendor, device_index,
                                                       original_memory_clk, original_graphics_clk);
                if (restore_result)
                {
                    usleep(300000); // 300ms
                    GpuClockInfo final_clock_info = {};
                    Query::get_clock_info(vendor, device_index, final_clock_info);
                    std::cout << "Restored clocks - Graphics: "
                              << final_clock_info.current_graphics_clock_MHz
                              << " MHz, Memory: " << final_clock_info.current_memory_clock_MHz
                              << " MHz" << std::endl;
                }
                else
                {
                    std::cout << "Failed to restore original clocks" << std::endl;
                    // Try reset as fallback
                    Query::reset_clock(vendor, device_index);
                }

                // Assert that we met the success threshold
                if (total_tests > 0)
                {
                    EXPECT_GE(success_rate, SUCCESS_THRESHOLD)
                        << "Clock setting success rate " << (success_rate * 100.0)
                        << "% is below threshold " << (SUCCESS_THRESHOLD * 100.0) << "%";
                }
            }
        }
    }
}

TEST(GpuQueryTest, ClockBoundaryTest)
{
    GPUVendors all_vendors{}; // RAII init/shutdown

    for (const auto &vendor : all_vendors.available_vendors)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(vendor, device_count) && device_count > 0)
        {
            for (uint32_t device_index = 0; device_index < device_count; ++device_index)
            {
                GpuClockInfo clock_info = {};
                if (!Query::get_clock_info(vendor, device_index, clock_info) ||
                    !clock_info.has_frequency_control)
                {
                    continue;
                }

                std::cout << "\n=== Testing Clock Boundaries for " << to_string(vendor)
                          << " Device[" << device_index << "] ===" << std::endl;

                // Save original clocks
                uint32_t original_graphics_clk = clock_info.current_graphics_clock_MHz;
                uint32_t original_memory_clk = clock_info.current_memory_clock_MHz;

                // Test minimum clocks
                if (clock_info.min_graphics_clock_MHz > 0 && clock_info.min_memory_clock_MHz > 0)
                {
                    std::cout << "Testing minimum clocks: Mem=" << clock_info.min_memory_clock_MHz
                              << " MHz, Gfx=" << clock_info.min_graphics_clock_MHz << " MHz" << std::endl;

                    bool min_result = Query::set_clock(vendor, device_index,
                                                       clock_info.min_memory_clock_MHz,
                                                       clock_info.min_graphics_clock_MHz);
                    if (min_result)
                    {
                        usleep(100000);
                        GpuClockInfo verify_info = {};
                        Query::get_clock_info(vendor, device_index, verify_info);
                        std::cout << "Actual clocks: Mem=" << verify_info.current_memory_clock_MHz
                                  << " MHz, Gfx=" << verify_info.current_graphics_clock_MHz << " MHz" << std::endl;
                    }
                }

                // Test maximum clocks
                if (clock_info.max_graphics_clock_MHz > 0 && clock_info.max_memory_clock_MHz > 0)
                {
                    std::cout << "Testing maximum clocks: Mem=" << clock_info.max_memory_clock_MHz
                              << " MHz, Gfx=" << clock_info.max_graphics_clock_MHz << " MHz" << std::endl;

                    bool max_result = Query::set_clock(vendor, device_index,
                                                       clock_info.max_memory_clock_MHz,
                                                       clock_info.max_graphics_clock_MHz);
                    if (max_result)
                    {
                        usleep(100000);
                        GpuClockInfo verify_info = {};
                        Query::get_clock_info(vendor, device_index, verify_info);
                        std::cout << "Actual clocks: Mem=" << verify_info.current_memory_clock_MHz
                                  << " MHz, Gfx=" << verify_info.current_graphics_clock_MHz << " MHz" << std::endl;
                    }
                }

                // Test invalid clocks (should fail gracefully)
                std::cout << "Testing invalid clocks (should fail)" << std::endl;
                bool invalid_result = Query::set_clock(vendor, device_index, 999999, 999999);
                EXPECT_FALSE(invalid_result) << "Setting invalid clock frequencies should fail";

                // Restore original clocks
                Query::set_clock(vendor, device_index, original_memory_clk, original_graphics_clk);
                usleep(100000);
            }
        }
    }
}