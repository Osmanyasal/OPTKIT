#include <omp.h>
#include "optkit.hh"
#include "core/energy/gpu/query.hh"
#include <immintrin.h> // AVX intrinsics

#define VECTOR_SIZE 1000000

// Generate a vector of doubles where vec[i] = i
template <class T>
inline std::vector<T> generate_vector(size_t n = VECTOR_SIZE)
{
    std::vector<T> vec(n);
    for (size_t i = 0; i < n; ++i)
        vec[i] = static_cast<T>(i);
    return vec;
}

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT({false});

    OPTKIT_CPU_ENERGY(main, "main");

    // GPU Query Methods Test
    std::cout << "=== GPU Query Methods Test ===" << std::endl;

    // Check vendor-specific power monitoring availability
    std::cout << "NVIDIA power available: " << optkit::energy::gpu::Query::is_nvidia_power_available() << std::endl;
    std::cout << "AMD power available: " << optkit::energy::gpu::Query::is_amd_power_available() << std::endl;
    std::cout << "Intel GPU power available: " << optkit::energy::gpu::Query::is_intel_gpu_power_available() << std::endl;

    // Get available power measurement methods
    int32_t methods = optkit::energy::gpu::Query::get_available_power_methods();
    std::cout << "Available power methods (bitmask): " << methods << std::endl;

    // Check GPU capabilities
    std::cout << "GPU frequency control available: " << optkit::energy::gpu::Query::is_gpu_frequency_control_available() << std::endl;
    std::cout << "GPU temperature monitoring available: " << optkit::energy::gpu::Query::is_gpu_temperature_available() << std::endl;
    std::cout << "GPU utilization monitoring available: " << optkit::energy::gpu::Query::is_gpu_utilization_monitoring_available() << std::endl;
    std::cout << "GPU memory power monitoring available: " << optkit::energy::gpu::Query::is_gpu_memory_power_available() << std::endl;

    // Get power-capable GPUs
    auto gpus = optkit::energy::gpu::Query::get_power_capable_gpus();
    std::cout << "Found " << gpus.size() << " power-capable GPUs:" << std::endl;
    for (const auto &gpu : gpus)
    {
        std::cout << "  GPU " << gpu.id << ": " << gpu.name
                  << " (Vendor: " << optkit::energy::gpu::to_string(gpu.vendor) << ")"
                  << " Max Power: " << gpu.max_power_watts << "W"
                  << " Current Power: " << gpu.current_power_watts << "W" << std::endl;
        std::cout << "    Power monitoring: " << gpu.has_power_monitoring
                  << ", Frequency control: " << gpu.has_frequency_control
                  << ", Temperature monitoring: " << gpu.has_temperature_monitoring << std::endl;

        // Get power limits for this GPU
        auto limits = optkit::energy::gpu::Query::get_gpu_power_limits(gpu.id);
        std::cout << "    Power limits - Min: " << limits.min_power_watts << "W"
                  << ", Max: " << limits.max_power_watts << "W"
                  << ", Default: " << limits.default_power_watts << "W"
                  << ", Current limit: " << limits.current_limit_watts << "W"
                  << ", Configurable: " << limits.is_configurable << std::endl;
    }

    // Get system-wide GPU power info
    auto system_info = optkit::energy::gpu::Query::get_system_gpu_power_info();
    std::cout << "System GPU Power Info:" << std::endl;
    std::cout << "  Total GPU power budget: " << system_info.total_gpu_power_budget_watts << "W" << std::endl;
    std::cout << "  Current GPU power usage: " << system_info.current_gpu_power_usage_watts << "W" << std::endl;
    std::cout << "  Available GPU power headroom: " << system_info.available_gpu_power_headroom_watts << "W" << std::endl;
    std::cout << "  Number of power-monitored GPUs: " << system_info.num_power_monitored_gpus << std::endl;

    // Cleanup vendor libraries
    optkit::energy::gpu::Query::cleanup_vendor_libraries();

    std::cout << "=== End GPU Query Test ===" << std::endl;

    std::cout << optkit::Query::is_smt_enabled() << "\n";
    sleep(5);

    // return 0;
    auto result = optkit::energy::rapl::Query::rapl_domain_info();

    // print result
    std::cout << "Detected RAPL Domains:" << std::endl;
    for (const auto &domain_info : result)
    {
        std::cout << domain_info << std::endl;
    }
    // exit(0);
    // OPTKIT_CPU_TEMPERATURE_EVENTS("main", {});
    // OPTKIT_DISK_EVENTS("main", optkit::metrics::disk::core_metrics::AllMetrics());
    OPTKIT_CPU_EVENTS("main", optkit::metrics::cpu::core_metrics::CPUMaxCapacityBasedUtilization());
    // optkit::metrics::MetricBuilder mb{true, true};

    // mb.add(optkit::metrics::cpu::core_metrics::IpC());

    // OPTKIT_CPU_EVENTS("main", mb);

    // instructions_million();
    // var12.read_and_store();
    // instructions_million();

    // std::string name;

    // std::cout << "Enter your name:" << std::endl;
    // std::getline(std::cin, name);
    // std::cout << "Hello, " << name << "!" << std::endl;
    int num_threads = omp_get_max_threads();
    std::cout << "Using " << num_threads << " threads.\n";

    // Shared flag to stop after some time
    std::atomic<bool> stop{false};

    // Launch a timer thread to stop after e.g. 10 seconds
    std::thread timer([&stop]()
                      {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        stop = true; });

// Run OpenMP parallel region
#pragma omp parallel
    {
        // Each thread runs a tight floating-point loop
        double x = 1.0;
        while (!stop.load(std::memory_order_relaxed))
        {
            // Some floating-point work
            x *= 1.0000001;
            x /= 1.0000001;
            x += 0.0000001;
            x -= 0.0000001;

            // Prevent compiler from optimizing away
            if (x > 1e100)
                x = 1.0;
        }
    }

    timer.join();

    std::cout << "Benchmark finished.\n";
    return 0;
#if 0
    sleep(1);
    for (int j = 0; j < 1000; j++)
    {
        std::vector<double> v1 = generate_vector<double>(); // 1 million elements
        std::vector<double> v2 = generate_vector<double>(); // 1 million elements
        std::vector<double> v3(VECTOR_SIZE);

        // OPTKIT_CPU_EVENTS("FLOPs_AVX", optkit::metrics::cpu::core_metrics::IpAVXAnyFlop());

        constexpr int simd_width = 4; // AVX 256-bit holds 4 doubles
        int i = 0;

        // Process blocks of 4 doubles at once
        for (; i + simd_width <= VECTOR_SIZE; i += simd_width)
        {
            // Load 4 doubles from v1 and v2
            __m256d vec1 = _mm256_loadu_pd(&v1[i]);
            __m256d vec2 = _mm256_loadu_pd(&v2[i]);

            // vec1 * 2.0 + vec2
            __m256d vec_mul = _mm256_mul_pd(vec1, _mm256_set1_pd(2.0));
            __m256d vec_res = _mm256_add_pd(vec_mul, vec2);

            // Store result
            _mm256_storeu_pd(&v3[i], vec_res);
        }

        // Process remaining elements scalar way
        for (; i < VECTOR_SIZE; i++)
        {
            v3[i] = v1[i] * 2.0 + v2[i];
        }
    }
#endif
    return 0;
}
