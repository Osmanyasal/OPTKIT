#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include "core/gpu_query.hh"

using namespace optkit::gpu;

// RAII class to manage GPU vendor initialization and shutdown
class GPUVendors
{
public:
    GPUVendors()
    {
        // init all vendors
        for (GpuVendor vendor = GpuVendor::BEGIN; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
        {
            if (!Query::is_vendor_exists(vendor))
                continue;

            bool is_vendor_available = Query::init(vendor);
            if (is_vendor_available)
            {
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
            {
                std::cout << "Shutdown vendor " << to_string(vendor) << " successfully." << std::endl;
            }
            else
            {
                std::cout << "Failed to shutdown vendor " << to_string(vendor) << "." << std::endl;
            }
        }
    }
    std::vector<GpuVendor> available_vendors;
};

TEST(GpuQueryTest, InitializationAndShutdown)
{
    // init
    for (GpuVendor vendor = GpuVendor::BEGIN; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        if (!Query::is_vendor_exists(vendor))
            continue;

        bool is_vendor_available = Query::init(vendor);
        if (is_vendor_available)
        {
            uint32_t count = 0;
            if (Query::get_device_count(vendor, count))
            {
                std::cout << "Vendor " << to_string(vendor) << " has " << count << " devices." << std::endl;
            }
        }
        else
        {
            ADD_FAILURE() << "Failed to initialize vendor " << to_string(vendor) << " although it exists.";
        }
    }
    // shutdown
    for (GpuVendor vendor = GpuVendor::BEGIN; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        if (!Query::is_vendor_exists(vendor))
            continue;

        if (Query::shutdown(vendor))
        {
            std::cout << "Shutdown vendor " << to_string(vendor) << " successfully." << std::endl;
        }
        else
        {
            ADD_FAILURE() << "Failed to shutdown vendor " << to_string(vendor) << " although it exists.";
        }
    }
}

TEST(GpuQueryTest, MultipleInitializationsSafe)
{
    for (GpuVendor vendor = GpuVendor::BEGIN; vendor < GpuVendor::END; vendor = static_cast<GpuVendor>(static_cast<int>(vendor) + 1))
    {
        if (!Query::is_vendor_exists(vendor))
            continue;
        bool result1 = Query::init(vendor);
        bool result2 = Query::init(vendor);
        bool result3 = Query::init(vendor);

        // Both should succeed (second should be no-op)
        EXPECT_EQ(result1, result2);
        EXPECT_EQ(result1, result3);

        if (result1)
            Query::shutdown(vendor);
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
                    EXPECT_EQ(memory_info.free_memory_MBytes,
                              memory_info.total_global_memory_MBytes - memory_info.used_memory_MBytes);

                    std::cout << to_string(vendor) << " Device[" << device_index << "] memory: "
                              << memory_info.used_memory_MBytes << "/"
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
                EXPECT_TRUE(Query::get_compute_info(vendor, device_index, compute_info));

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

    // Re-initialize for other tests
    Query::init(GpuVendor::NVIDIA);
}

// Performance test
TEST(GpuQueryTest, PerformanceTest)
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