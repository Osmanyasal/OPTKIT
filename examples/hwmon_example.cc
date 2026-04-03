#include "core/energy/cpu/hwmon/module.hh"
#include <iostream>
#include <chrono>
#include <thread>

/**
 * @brief Example demonstrating HWMON energy profiling for Grace chips
 * 
 * This example shows how to use the HWMON profiler to measure power/energy
 * consumption on ARM-based systems like NVIDIA Grace.
 */

// Simple workload function
void compute_workload(int iterations)
{
    volatile double result = 0.0;
    for (int i = 0; i < iterations; i++)
    {
        result += std::sqrt(i * 3.14159);
    }
}

int main()
{
    std::cout << "=== HWMON Energy Profiling Example ===" << std::endl;
    
    // Check if HWMON is available
    auto methods = optkit::energy::hwmon::Query::avail_hwmon_read_methods();
    std::cout << "Available HWMON read methods: " << methods << std::endl;
    
    if (!optkit::energy::hwmon::Query::is_hwmon_sysfs_avail())
    {
        std::cerr << "HWMON sysfs interface not available!" << std::endl;
        return 1;
    }
    
    // Display available HWMON domains
    const auto& domains = optkit::energy::hwmon::Query::hwmon_domain_info();
    std::cout << "\nAvailable HWMON domains:" << std::endl;
    for (const auto& domain : domains)
    {
        std::cout << "  " << domain << std::endl;
    }
    std::cout << std::endl;
    
    // Example 1: Basic energy measurement
    {
        std::cout << "Example 1: Basic HWMON energy measurement" << std::endl;
        OPTKIT_HWMON_ENERGY("example1_basic")
        {
            compute_workload(10000000);
        }
        std::cout << std::endl;
    }
    
    // Example 2: With custom metrics
    {
        std::cout << "Example 2: HWMON with custom metrics" << std::endl;
        auto custom_metrics = optkit::metrics::energy::cpu_metrics::all_metrics();
        
        OPTKIT_HWMON_ENERGY_WITH_METRICS("example2_with_metrics", custom_metrics)
        {
            compute_workload(20000000);
        }
        std::cout << std::endl;
    }
    
    // Example 3: Repeated measurements
    {
        std::cout << "Example 3: Repeated HWMON measurements" << std::endl;
        OPTKIT_HWMON_ENERGY_REPEAT("example3_repeat", 5)
        {
            compute_workload(5000000);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << std::endl;
    }
    
    // Example 4: Sampling mode (continuous monitoring)
    {
        std::cout << "Example 4: HWMON sampling mode" << std::endl;
        OPTKIT_HWMON_ENERGY_SAMPLING("example4_sampling")
        {
            // Profiler samples power every second in background thread
            for (int i = 0; i < 3; i++)
            {
                compute_workload(10000000);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        std::cout << std::endl;
    }
    
    // Example 5: Manual profiler usage
    {
        std::cout << "Example 5: Manual HWMON profiler usage" << std::endl;
        
        optkit::ProfilerConfig config{
            "example5_manual",        // block_name
            "hwmon_energy",           // measurement_type
            false,                    // is_reset_after_read
            false,                    // is_sampling
            true,                     // dump_results_to_file
            true                      // verbose
        };
        
        auto metrics = optkit::metrics::energy::cpu_metrics::all_metrics();
        optkit::energy::hwmon::Profiler profiler(config, metrics);
        
        // Take multiple readings
        for (int i = 0; i < 3; i++)
        {
            compute_workload(5000000);
            
            // Manual read and store
            auto reading = profiler.read_and_store();
            std::cout << "  Reading " << (i + 1) << ": " << reading.second << std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Profiler destructor will aggregate and save results
        std::cout << std::endl;
    }
    
    std::cout << "=== All examples completed ===" << std::endl;
    std::cout << "Results saved to execution folder." << std::endl;
    
    return 0;
}
