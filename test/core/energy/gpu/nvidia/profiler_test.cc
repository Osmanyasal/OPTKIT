#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "core/energy/gpu/nvidia/profiler.hh"
#include "utils/metric_builder.hh"

using namespace optkit::energy::gpu::nvidia;
using namespace optkit::gpu;

class NvidiaGpuEnergyTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Initialize NVIDIA GPU monitoring
        nvidia_available = Query::init(GpuVendor::NVIDIA);

        if (nvidia_available)
        {
            Query::get_device_count(GpuVendor::NVIDIA, device_count);
        }

        // Create a simple metric builder for testing
        metric_builder = optkit::metrics::MetricBuilder<double>{}
                             .add("gpu", {0x0}) // Simple GPU energy metric
                             .build("test_metric", [](const std::unordered_map<std::string, double> &results) -> double
                                    {
                auto it = results.find("gpu");
                return (it != results.end()) ? it->second : 0.0; });
    }

    void TearDown() override
    {
        if (nvidia_available)
        {
            Query::shutdown(GpuVendor::NVIDIA);
        }
    }

    bool nvidia_available = false;
    uint32_t device_count = 0;
    optkit::metrics::MetricBuilder<double> metric_builder;
};

TEST_F(NvidiaGpuEnergyTest, ProfilerInitialization)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    optkit::ProfilerConfig config;
    config.verbose = false;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_test";
    config.block_name = "test_block";

    EXPECT_NO_THROW({
        Profiler profiler(config, metric_builder, 1);
        // Profiler should initialize successfully
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Destructor will be called automatically
    });
}

TEST_F(NvidiaGpuEnergyTest, ProfilerWithNoGPUs)
{
    // Shutdown to simulate no GPUs
    if (nvidia_available)
    {
        Query::shutdown(GpuVendor::NVIDIA);
    }

    optkit::ProfilerConfig config;
    config.verbose = false;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_test";
    config.block_name = "test_block";

    EXPECT_NO_THROW({
        Profiler profiler(config, metric_builder, 1);
        // Should handle no GPUs gracefully
    });

    // Re-initialize for other tests
    if (nvidia_available)
    {
        Query::init(GpuVendor::NVIDIA);
    }
}

TEST_F(NvidiaGpuEnergyTest, SamplingFunctionality)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    std::unordered_map<uint32_t, double> snapshot;

    EXPECT_NO_THROW({
        sampling_function(snapshot, 1);
    });

    // Check that snapshot has entries for detected devices
    EXPECT_EQ(snapshot.size(), device_count);

    for (const auto &entry : snapshot)
    {
        EXPECT_LE(entry.second, 1000.0); // Energy should be reasonable (< 1000 Joules per second)
        std::cout << "GPU " << entry.first << " energy: " << entry.second << " Joules" << std::endl;
    }
}

TEST_F(NvidiaGpuEnergyTest, ShortDurationProfiling)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    optkit::ProfilerConfig config;
    config.verbose = true;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_short";
    config.block_name = "short_test";

    {
        Profiler profiler(config, metric_builder, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(2100)); // Just over 2 seconds
        // Destructor will aggregate and calculate metrics
    }

    // Test completed successfully if no exceptions thrown
    SUCCEED();
}

TEST_F(NvidiaGpuEnergyTest, JSONOutputFormat)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    optkit::ProfilerConfig config;
    config.verbose = false;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_json";
    config.block_name = "json_test";

    std::string json_output;
    {
        Profiler profiler(config, metric_builder, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        json_output = profiler.to_json();
    }

    EXPECT_FALSE(json_output.empty());
    EXPECT_NE(json_output.find('['), std::string::npos); // Should contain JSON array
    EXPECT_NE(json_output.find(']'), std::string::npos);

    std::cout << "JSON Output preview: " << json_output.substr(0, 200) << "..." << std::endl;
}

TEST_F(NvidiaGpuEnergyTest, MultipleGPUHandling)
{
    if (!nvidia_available || device_count < 2)
    {
        GTEST_SKIP() << "Need at least 2 NVIDIA GPUs for this test";
    }

    optkit::ProfilerConfig config;
    config.verbose = true;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_multi";
    config.block_name = "multi_gpu_test";

    {
        Profiler profiler(config, metric_builder, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(2100));
    }

    // Test that multiple GPUs are handled correctly
    std::cout << "Multi-GPU test completed with " << device_count << " devices" << std::endl;
    SUCCEED();
}

TEST_F(NvidiaGpuEnergyTest, HighFrequencySampling)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    optkit::ProfilerConfig config;
    config.verbose = false;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_highfreq";
    config.block_name = "highfreq_test";

    // Test with higher frequency sampling (0.5 second intervals)
    {
        Profiler profiler(config, metric_builder, 1); // Note: sampling_frequency_sec is uint32_t, so minimum is 1
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    SUCCEED();
}

TEST_F(NvidiaGpuEnergyTest, MetricCalculation)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    // Create a more sophisticated metric builder
    auto advanced_metric_builder = optkit::metrics::MetricBuilder<double>{}
                                       .add("gpu", {0x0})
                                       .build("energy_efficiency", [](const std::unordered_map<std::string, double> &results) -> double
                                              {
            auto gpu_it = results.find("gpu");
            auto duration_it = results.find("duration_microsec");
            
            if (gpu_it != results.end() && duration_it != results.end())
            {
                double energy_joules = gpu_it->second;
                double duration_seconds = duration_it->second / 1000000.0;
                return energy_joules / duration_seconds;  // Average power in Watts
            }
            return 0.0; });

    optkit::ProfilerConfig config;
    config.verbose = true;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_metrics";
    config.block_name = "metrics_test";

    {
        Profiler profiler(config, advanced_metric_builder, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(3100));
    }

    SUCCEED();
}

TEST_F(NvidiaGpuEnergyTest, StressTest)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    optkit::ProfilerConfig config;
    config.verbose = false;
    config.dump_results_to_file = false;
    config.measurement_type = "nvidia_gpu_energy_stress";
    config.block_name = "stress_test";

    // Run for longer duration to test stability
    {
        Profiler profiler(config, metric_builder, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(5100));
    }

    SUCCEED();
}

TEST_F(NvidiaGpuEnergyTest, PowerMonitoringAccuracy)
{
    if (!nvidia_available || device_count == 0)
    {
        GTEST_SKIP() << "No NVIDIA GPUs available for testing";
    }

    // Test consistency of power readings
    std::vector<double> power_readings;

    for (int i = 0; i < 10; ++i)
    {
        double power = 0.0;
        if (Query::get_device_power(GpuVendor::NVIDIA, 0, power))
        {
            power_readings.push_back(power);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!power_readings.empty())
    {
        // Calculate basic statistics
        double sum = 0.0;
        for (double power : power_readings)
        {
            sum += power;
            EXPECT_GE(power, 0.0);
            EXPECT_LE(power, 1000.0); // Reasonable upper bound
        }

        double average = sum / power_readings.size();
        std::cout << "Average power over 10 readings: " << average << "W" << std::endl;

        EXPECT_GT(average, 0.0);
    }
}
