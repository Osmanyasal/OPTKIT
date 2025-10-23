#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "solution.hh"
#include "solution_patch.hh"
#include "constants.hh"
#include "optkit.hh"

// Forward declaration
void init(std::vector<char> &data);

bool validate_file_contents(const std::string &filename, const std::vector<char> &expected_data)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for validation: " << filename << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_size != expected_data.size())
    {
        std::cerr << "File size mismatch. Expected: " << expected_data.size()
                  << ", Got: " << file_size << std::endl;
        return false;
    }

    // Read and compare data
    std::vector<char> file_data(file_size);
    file.read(file_data.data(), file_size);

    if (file_data != expected_data)
    {
        std::cerr << "File content mismatch" << std::endl;
        return false;
    }

    return true;
}

void cleanup_test_files()
{
    std::filesystem::remove("unbuffered_test.dat");
    std::filesystem::remove("buffered_test.dat");
    std::filesystem::remove("optkit_test.dat");
}

int main()
{
    OPTKIT_INIT(false);

    // Initialize test data
    std::vector<char> test_data(UNBUFFERED_IO_N * UNBUFFERED_IO_CHUNK_SIZE);
    init(test_data);

    std::cout << "Data size: " << test_data.size() / 1024 / 1024 << " MB" << std::endl;

    cleanup_test_files();

    // Test unbuffered solution
    try
    {
        solution(test_data, "unbuffered_test.dat");
        if (!validate_file_contents("unbuffered_test.dat", test_data))
        {
            std::cerr << "Unbuffered solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unbuffered solution error: " << e.what() << std::endl;
        return 1;
    }

    // Test buffered solution
    try
    {
        solution_patch(test_data, "buffered_test.dat");
        if (!validate_file_contents("buffered_test.dat", test_data))
        {
            std::cerr << "Buffered solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Buffered solution error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Validation Successful" << std::endl;

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;
    double third_duration_ms = 0.0;

    // START BENCHMARKING
    {
        std::cout << "\nBenchmarking unbuffered I/O (O_SYNC)..." << std::endl;

        optkit::utils::BlockTimer block_timer("unbuffered I/O", first_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("unbuffered_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution(test_data, "unbuffered_test.dat");
        }
    }

    {
        std::cout << "\nBenchmarking buffered I/O (standard library)..." << std::endl;

        optkit::utils::BlockTimer block_timer("buffered I/O", second_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("buffered_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution_patch(test_data, "buffered_test.dat");
        }
    }

    std::string test_data_str(test_data.begin(), test_data.end());
    {
        std::cout << "\nOPTKIT write_file function call..." << std::endl;

        optkit::utils::BlockTimer block_timer("OPTKIT_WRITE", third_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("optkit_write", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            optkit::utils::write_file("optkit_test.dat", test_data_str);
        }
    }

    cleanup_test_files();

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Unbuffered Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Buffered Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "OPTKIT_WRITE Duration (ms): " << third_duration_ms << std::endl;
    std::cout << "Speedup (Buffered/Unbuffered): " << first_duration_ms / second_duration_ms << "x" << std::endl;
    std::cout << "Speedup (Unbuffered/OPTKIT_WRITE): " << first_duration_ms / third_duration_ms << "x" << std::endl;
    std::cout << "Speedup (Buffered/OPTKIT_WRITE): " << second_duration_ms / third_duration_ms << "x" << std::endl;

    return 0;
}