#include <omp.h>
#include "optkit.hh"
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

    std::cout << "GPU Device Query Example" << std::endl;
    std::cout << "========================" << std::endl;

    // Initialize GPU monitoring
    if (!optkit::gpu::Query::init())
    {
        std::cerr << "Failed to initialize GPU monitoring libraries" << std::endl;
        return 1;
    }

    // Get device count
    uint32_t device_count = optkit::gpu::Query::get_device_count();
    std::cout << "Found " << device_count << " GPU device(s)" << std::endl
              << std::endl;

    if (device_count == 0)
    {
        std::cout << "No GPU devices found. Make sure you have:" << std::endl;
        std::cout << "- NVIDIA GPU with NVML library installed, or" << std::endl;
        std::cout << "- AMD GPU with ROCm SMI library installed" << std::endl;
        optkit::gpu::Query::shutdown();
        return 0;
    }

    // Query each device
    for (uint32_t i = 0; i < device_count; i++)
    {
        optkit::gpu::GpuDeviceInfo device_info = optkit::gpu::Query::device_query(static_cast<int32_t>(i));
        std::cout << device_info << "\n";
    }

    // Alternative: Query a specific device by ID
    std::cout << "Querying device 0 specifically:" << std::endl;
    optkit::gpu::GpuDeviceInfo device_0 = optkit::gpu::Query::device_query(0);
    if (device_0.basic.vendor != optkit::gpu::GpuVendor::UNKNOWN)
    {
        std::cout << "Device 0: " << device_0.basic.device_name
                  << " (" << device_0.basic.vendor_string << ")" << std::endl;
    }
    else
    {
        std::cout << "Device 0 not found or not supported" << std::endl;
    }

    // GPU Query Methods Test
    std::cout << "=== GPU Query Methods Test ===" << std::endl;

    // Check vendor-specific power monitoring availability
    std::cout << "NVIDIA power available: " << optkit::gpu::Query::is_nvidia_power_available() << std::endl;
    std::cout << "AMD power available: " << optkit::gpu::Query::is_amd_power_available() << std::endl;
    std::cout << "Intel GPU power available: " << optkit::gpu::Query::is_intel_gpu_power_available() << std::endl;

    // Get available power measurement methods
    int32_t methods = optkit::gpu::Query::get_available_power_methods();
    std::cout << "Available power methods (bitmask): " << methods << std::endl;

    // Get power-capable GPUs
    auto gpus = optkit::gpu::Query::get_power_capable_gpus();
    std::cout << "Found " << gpus.size() << " power-capable GPUs:" << std::endl;
    for (const auto &gpu : gpus)
    {
        std::cout << gpu << std::endl;
    }

    // Get system-wide GPU power info
    auto system_info = optkit::gpu::Query::get_system_gpu_power_info();
    std::cout << system_info << std::endl;

    // Cleanup vendor libraries
    optkit::gpu::Query::shutdown();
    std::cout << "=== End GPU Query Test ===" << std::endl;

    return 0;

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
