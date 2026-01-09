#include "solution.hh"
#include "solution_patch.hh"
#include "optkit.hh"

size_t original_solution(const std::vector<uint32_t> &data)
{
    size_t value = 0;

    for (size_t i = 0; i < data.size(); i++)
    {
        auto item = data[i];
        item += 1000;
        item ^= 0xADEDAE;
        item |= (item >> 24);

        value += item % 13;
    }

    return value;
}

int main()
{
    OPTKIT_INIT(); // Initialize OPTKIT
    OPTKIT_CALLSTACK_PROFILER("main");
    const auto size = 16 * 1024 * 1024;

    std::vector<uint32_t> data;
    data.reserve(size);

    for (int i = 0; i < size; i++)
    {
        data.push_back(i);
    }

    auto original_result = original_solution(data);

    // Use thread count from 1 to <number of HW threads>
    size_t max_threads = std::thread::hardware_concurrency();
    std::vector<int> threads(max_threads);
    std::iota(threads.begin(), threads.end(), 1); // Fill with 1, 2, 3, ..., max_threads

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;
    {
        optkit::utils::BlockTimer block_timer("parallel solution", first_duration_ms);
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        OPTKIT_CPU_EVENTS("solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        for (auto thread_count : threads)
        {
            auto result = solution(data, thread_count);
            if (original_result != result)
            {
                std::cerr << "Validation Failed for " << thread_count
                          << " thread(s). Original result = " << original_result
                          << "; Modified version returned = " << result << "\n";
                return 1;
            }
        }
    }

    {
        optkit::utils::BlockTimer block_timer("parallel patch solution", second_duration_ms);
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l1());
        // OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::topdown_l2());
        OPTKIT_CPU_EVENTS("patch solution", optkit::metrics::performance::cpu_metrics::all_mpki());
        for (auto thread_count : threads)
        {
            auto result = solution_patch(data, thread_count);
            if (original_result != result)
            {
                std::cerr << "Validation Failed for " << thread_count
                          << " thread(s). Original result = " << original_result
                          << "; Modified version returned = " << result << "\n";
                return 1;
            }
        }
    }

    std::cout << "Validation Successful" << std::endl;
    std::cout << "First Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Second Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "Speedup: " << first_duration_ms / second_duration_ms << "x" << std::endl;
    return 0;
}