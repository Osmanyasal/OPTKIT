#include <omp.h>
#include "optkit.hh"
#include <immintrin.h> // AVX intrinsics
#include <utils/gpu.hh>

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
    // OPTKIT_HWMON_TEMPERATURE_EVENTS("main", {});
    // OPTKIT_GPU_TEMPERATURE_EVENTS("main gpu temp", {});
    // OPTKIT_GPU_ENERGY_EVENTS("main gpu energy", {});
    OPTKIT_CPU_ENERGY("main_block");
    // optkit::gpu::GpuDeviceInfo info;
    // optkit::gpu::Query::device_query(optkit::gpu::GpuVendor::NVIDIA, 0, info);
    // std::cout << info << "\n";

#if 0
    std::cout << "GPU Device Query Example" << std::endl;
    std::cout << "========================" << std::endl;

    std::cout << "Driver version:" << optkit::gpu::Query::get_driver_version() << std::endl;
    std::cout << "Library version:" << optkit::gpu::Query::get_library_version() << std::endl;

    // Get device count
    uint32_t device_count = optkit::gpu::Query::get_device_count();
    std::cout << "Found " << device_count << " GPU device(s)" << std::endl;
    std::cout << "Architecture: " << optkit::gpu::Query::get_gpu_architecture(0) << std::endl;

    // Query each device
    for (uint32_t i = 0; i < device_count; i++)
    {
        optkit::gpu::GpuDeviceInfo device_query = optkit::gpu::Query::device_query(static_cast<int32_t>(i));
        std::cout << device_query << "\n";
    }

    // GPU Query Methods Test
    std::cout << "=== GPU Query Methods Test ===" << std::endl;

    // Check vendor-specific power monitoring availability
    // std::cout << "NVIDIA power available: " << optkit::gpu::Query::is_nvidia_power_available() << std::endl;
    // std::cout << "AMD power available: " << optkit::gpu::Query::is_amd_power_available() << std::endl;

    std::cout << optkit::Query::is_smt_enabled() << "\n";

    // return 0;
    auto result = optkit::energy::rapl::Query::rapl_domain_info();

    // print result
    std::cout << "Detected RAPL Domains:" << std::endl;
    for (const auto &domain_info : result)
    {
        std::cout << domain_info << std::endl;
    }
#endif

    sleep(5);
    var24.read_and_store();
    sleep(5);

    // optkit::gpu::Query::device_query(optkit::gpu::GpuVendor::NVIDIA, 0, info);
    // std::cout << info << "\n";
    // exit(0);
    return 0;
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

    // Launch a timer thread to stop after e.g. 100 seconds
    std::thread timer([&stop]()
                      {
        std::this_thread::sleep_for(std::chrono::seconds(100));
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
