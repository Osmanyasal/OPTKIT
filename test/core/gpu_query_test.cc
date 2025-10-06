#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include "core/gpu_query.hh"

using namespace optkit::gpu;

class GpuQueryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize both vendors if available
        nvidia_available = Query::init(GpuVendor::NVIDIA);
        amd_available = Query::init(GpuVendor::AMD);
    }

    void TearDown() override
    {
        // Shutdown in reverse order
        if (amd_available)
            Query::shutdown(GpuVendor::AMD);
        if (nvidia_available)
            Query::shutdown(GpuVendor::NVIDIA);
    }

    bool nvidia_available = false;
    bool amd_available = false;
};

// Basic initialization and shutdown tests
TEST_F(GpuQueryTest, InitializationAndShutdown)
{
    // Test that we can initialize and shutdown without crashes
    EXPECT_NO_THROW({
        bool nvidia_init = Query::init(GpuVendor::NVIDIA);
        bool amd_init = Query::init(GpuVendor::AMD);

        if (nvidia_init)
            EXPECT_TRUE(Query::shutdown(GpuVendor::NVIDIA));
        if (amd_init)
            EXPECT_TRUE(Query::shutdown(GpuVendor::AMD));
    });
}

TEST_F(GpuQueryTest, MultipleInitializationsSafe)
{
    // Test that multiple initializations don't cause issues
    bool result1 = Query::init(GpuVendor::NVIDIA);
    bool result2 = Query::init(GpuVendor::NVIDIA);

    // Both should succeed (second should be no-op)
    EXPECT_EQ(result1, result2);

    if (result1)
        Query::shutdown(GpuVendor::NVIDIA);
}

// Device count tests
TEST_F(GpuQueryTest, DeviceCountQuery)
{
    if (nvidia_available)
    {
        uint32_t nvidia_count = 0;
        EXPECT_TRUE(Query::get_device_count(GpuVendor::NVIDIA, nvidia_count));
        EXPECT_GE(nvidia_count, 0); // Should be 0 or more

        std::cout << "Found " << nvidia_count << " NVIDIA devices" << std::endl;
    }

    if (amd_available)
    {
        uint32_t amd_count = 0;
        EXPECT_TRUE(Query::get_device_count(GpuVendor::AMD, amd_count));
        EXPECT_GE(amd_count, 0); // Should be 0 or more

        std::cout << "Found " << amd_count << " AMD devices" << std::endl;
    }
}

// Basic info tests
TEST_F(GpuQueryTest, BasicInfoQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuBasicInfo basic_info;
            EXPECT_TRUE(Query::get_basic_info(GpuVendor::NVIDIA, 0, basic_info));

            EXPECT_EQ(basic_info.vendor, GpuVendor::NVIDIA);
            EXPECT_EQ(basic_info.vendor_string, "NVIDIA");
            EXPECT_EQ(basic_info.id, 0);
            EXPECT_FALSE(basic_info.device_name.empty());

            std::cout << "NVIDIA Device 0: " << basic_info.device_name << std::endl;
            std::cout << "Architecture: " << basic_info.architecture << std::endl;
        }
    }
}

TEST_F(GpuQueryTest, InvalidDeviceIndexHandling)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        Query::get_device_count(GpuVendor::NVIDIA, device_count);

        GpuBasicInfo basic_info;
        // Try to query a device that doesn't exist
        EXPECT_FALSE(Query::get_basic_info(GpuVendor::NVIDIA, device_count + 100, basic_info));
    }
}

// Power monitoring tests
TEST_F(GpuQueryTest, PowerMonitoring)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            double power_watts = 0.0;
            bool power_available = Query::get_device_power(GpuVendor::NVIDIA, 0, power_watts);

            if (power_available)
            {
                EXPECT_GE(power_watts, 0.0);
                EXPECT_LE(power_watts, 1000.0); // Reasonable upper bound
                std::cout << "NVIDIA Device 0 power: " << power_watts << " W" << std::endl;
            }
        }
    }
}

TEST_F(GpuQueryTest, PowerLimitsQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            double limit_watts, default_power, min_limit, max_limit;
            bool is_configurable;

            bool limits_available = Query::get_device_power_limits(
                GpuVendor::NVIDIA, 0, limit_watts, default_power,
                min_limit, max_limit, is_configurable);

            if (limits_available)
            {
                EXPECT_GE(limit_watts, 0.0);
                EXPECT_GE(default_power, 0.0);
                EXPECT_LE(min_limit, max_limit);

                std::cout << "NVIDIA Device 0 power limits: "
                          << min_limit << "W - " << max_limit << "W"
                          << " (current: " << limit_watts << "W)" << std::endl;
            }
        }
    }
}

// Temperature monitoring tests
TEST_F(GpuQueryTest, TemperatureMonitoring)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuTemperatureInfo temp_info;
            bool temp_available = Query::get_temperature_info(GpuVendor::NVIDIA, 0, temp_info);

            if (temp_available)
            {
                EXPECT_GE(temp_info.current_device_temperature_celsius, 0.0);
                EXPECT_LE(temp_info.current_device_temperature_celsius, 150.0); // Reasonable temperature range

                std::cout << "NVIDIA Device 0 temperatures: GPU=" << temp_info.current_device_temperature_celsius
                          << "°C, Memory=" << temp_info.current_memory_temperature_celsius << "°C" << std::endl;
            }
        }
    }
}

// Memory info tests
TEST_F(GpuQueryTest, MemoryInfoQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuMemoryInfo memory_info;
            EXPECT_TRUE(Query::get_memory_info(GpuVendor::NVIDIA, 0, memory_info));

            if (memory_info.total_global_memory_MBytes > 0)
            {
                EXPECT_GT(memory_info.total_global_memory_MBytes, 0);
                EXPECT_LE(memory_info.used_memory_MBytes, memory_info.total_global_memory_MBytes);
                EXPECT_EQ(memory_info.free_memory_MBytes,
                          memory_info.total_global_memory_MBytes - memory_info.used_memory_MBytes);

                std::cout << "NVIDIA Device 0 memory: "
                          << memory_info.used_memory_MBytes << "/"
                          << memory_info.total_global_memory_MBytes << " MB used" << std::endl;
            }
        }
    }
}

// Compute capability tests
TEST_F(GpuQueryTest, ComputeCapabilityQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuComputeInfo compute_info;
            EXPECT_TRUE(Query::get_compute_info(GpuVendor::NVIDIA, 0, compute_info));

            if (compute_info.compute_capability_major > 0)
            {
                EXPECT_GT(compute_info.compute_capability_major, 0);
                EXPECT_GE(compute_info.compute_capability_minor, 0);
                EXPECT_EQ(compute_info.warp_size, 32); // NVIDIA warp size is always 32

                std::cout << "NVIDIA Device 0 compute capability: "
                          << compute_info.compute_capability_major << "."
                          << compute_info.compute_capability_minor << std::endl;
                std::cout << "Multiprocessors: " << compute_info.multiprocessor_count
                          << ", Total cores: " << compute_info.total_cores << std::endl;
            }
        }
    }
}

// Clock information tests
TEST_F(GpuQueryTest, ClockInfoQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuClockInfo clock_info;
            EXPECT_TRUE(Query::get_clock_info(GpuVendor::NVIDIA, 0, clock_info));

            if (clock_info.current_graphics_clock_MHz > 0)
            {
                EXPECT_GT(clock_info.current_graphics_clock_MHz, 0);
                EXPECT_GT(clock_info.current_memory_clock_MHz, 0);
                EXPECT_LE(clock_info.current_graphics_clock_MHz, clock_info.max_graphics_clock_MHz);

                std::cout << "NVIDIA Device 0 clocks: Graphics="
                          << clock_info.current_graphics_clock_MHz << "MHz, Memory="
                          << clock_info.current_memory_clock_MHz << "MHz" << std::endl;
            }
        }
    }
}

// Version information tests
TEST_F(GpuQueryTest, VersionInfoQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuVersionInfo version_info;
            EXPECT_TRUE(Query::get_version_info(GpuVendor::NVIDIA, 0, version_info));

            EXPECT_GT(version_info.driver_major_minor, 0.0);
            EXPECT_FALSE(version_info.driver_version_string.empty());
            EXPECT_FALSE(version_info.library_version_string.empty());

            std::cout << "NVIDIA Driver version: " << version_info.driver_version_string
                      << ", Library: " << version_info.library_version_string << std::endl;
        }
    }
}

// Comprehensive device query test
TEST_F(GpuQueryTest, ComprehensiveDeviceQuery)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            GpuDeviceInfo device_info;
            EXPECT_TRUE(Query::device_query(GpuVendor::NVIDIA, 0, device_info));

            // Verify all sub-structures are populated
            EXPECT_EQ(device_info.basic.vendor, GpuVendor::NVIDIA);
            EXPECT_FALSE(device_info.basic.device_name.empty());
            EXPECT_GT(device_info.version.driver_major_minor, 0.0);

            std::cout << "=== Comprehensive NVIDIA Device 0 Info ===" << std::endl;
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
TEST_F(GpuQueryTest, UtilityFunctions)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            // Test warp size
            uint32_t warp_size = 0;
            EXPECT_TRUE(Query::get_warp_size(GpuVendor::NVIDIA, 0, warp_size));
            EXPECT_EQ(warp_size, 32);

            // Test device name
            std::string device_name;
            EXPECT_TRUE(Query::get_device_name(GpuVendor::NVIDIA, 0, device_name));
            EXPECT_FALSE(device_name.empty());

            // Test architecture
            uint32_t architecture = 0;
            EXPECT_TRUE(Query::get_architecture(GpuVendor::NVIDIA, 0, architecture));
            EXPECT_GT(architecture, 0);
        }
    }
}

// Stress test - query all devices
TEST_F(GpuQueryTest, StressTestAllDevices)
{
    std::vector<GpuVendor> vendors = {GpuVendor::NVIDIA, GpuVendor::AMD};

    for (auto vendor : vendors)
    {
        bool vendor_available = (vendor == GpuVendor::NVIDIA) ? nvidia_available : amd_available;
        if (!vendor_available)
            continue;

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

                double gpu_temp, mem_temp;
                Query::get_device_temperature(vendor, i, gpu_temp, mem_temp); // May fail

                std::cout << "  Device " << i << ": " << basic_info.device_name
                          << " (Power: " << power << "W)" << std::endl;
            }
        }
    }
}

// Error handling tests
TEST_F(GpuQueryTest, ErrorHandlingInvalidVendor)
{
    // Test with invalid vendor enum (should not crash)
    uint32_t device_count = 0;
    EXPECT_FALSE(Query::get_device_count(static_cast<GpuVendor>(999), device_count));
}

TEST_F(GpuQueryTest, ErrorHandlingUninitializedVendor)
{
    // Test operations on uninitialized vendor
    Query::shutdown(GpuVendor::NVIDIA); // Ensure it's shutdown

    uint32_t device_count = 0;
    EXPECT_FALSE(Query::get_device_count(GpuVendor::NVIDIA, device_count));

    // Re-initialize for other tests
    Query::init(GpuVendor::NVIDIA);
}

// Performance test
TEST_F(GpuQueryTest, PerformanceTest)
{
    if (nvidia_available)
    {
        uint32_t device_count = 0;
        if (Query::get_device_count(GpuVendor::NVIDIA, device_count) && device_count > 0)
        {
            auto start = std::chrono::high_resolution_clock::now();

            // Perform 100 power queries
            for (int i = 0; i < 100; ++i)
            {
                double power = 0.0;
                Query::get_device_power(GpuVendor::NVIDIA, 0, power);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            std::cout << "100 power queries took: " << duration.count() << " microseconds" << std::endl;
            EXPECT_LT(duration.count(), 1000000); // Should complete within 1 second
        }
    }
}
