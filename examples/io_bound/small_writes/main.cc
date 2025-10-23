#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "solution.hh"
#include "solution_patch.hh"
#include "constants.hh"
#include "optkit.hh"

// Forward declaration
void init(std::vector<std::string> &data_chunks);

bool validate_file_contents(const std::string &filename, const std::vector<std::string> &expected_chunks)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for validation: " << filename << std::endl;
        return false;
    }

    // Calculate expected total size
    size_t expected_size = 0;
    for (const auto &chunk : expected_chunks)
    {
        expected_size += chunk.size();
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_size != expected_size)
    {
        std::cerr << "File size mismatch. Expected: " << expected_size
                  << ", Got: " << file_size << std::endl;
        return false;
    }

    // Read and validate content
    for (const auto &expected_chunk : expected_chunks)
    {
        std::string file_chunk(expected_chunk.size(), '\0');
        file.read(&file_chunk[0], expected_chunk.size());

        if (file_chunk != expected_chunk)
        {
            std::cerr << "Chunk content mismatch" << std::endl;
            return false;
        }
    }

    return true;
}

void cleanup_test_files()
{
    std::filesystem::remove("small_writes_test.dat");
    std::filesystem::remove("batched_writes_test.dat");
}

int main()
{
    OPTKIT_INIT(false);

    // Initialize test data
    std::vector<std::string> test_chunks;
    init(test_chunks);

    size_t total_size = 0;
    for (const auto &chunk : test_chunks)
    {
        total_size += chunk.size();
    }

    std::cout << "Number of chunks: " << test_chunks.size() << std::endl;
    std::cout << "Chunk size: " << SMALL_WRITE_SIZE << " bytes" << std::endl;
    std::cout << "Total data size: " << total_size / 1024 << " KB" << std::endl;

    cleanup_test_files();

    // Test small writes solution
    try
    {
        solution(test_chunks, "small_writes_test.dat");
        if (!validate_file_contents("small_writes_test.dat", test_chunks))
        {
            std::cerr << "Small writes solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Small writes solution error: " << e.what() << std::endl;
        return 1;
    }

    // Test batched writes solution
    try
    {
        solution_patch(test_chunks, "batched_writes_test.dat");
        if (!validate_file_contents("batched_writes_test.dat", test_chunks))
        {
            std::cerr << "Batched writes solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Batched writes solution error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Validation Successful" << std::endl;

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;

    // START BENCHMARKING
    {
        std::cout << "\nBenchmarking small frequent writes..." << std::endl;

        optkit::utils::BlockTimer block_timer("small writes", first_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("small_writes_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution(test_chunks, "small_writes_test.dat");
        }
    }

    {
        std::cout << "\nBenchmarking batched writes..." << std::endl;

        optkit::utils::BlockTimer block_timer("batched writes", second_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("batched_writes_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            solution_patch(test_chunks, "batched_writes_test.dat");
        }
    }

    cleanup_test_files();

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Small Writes Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Batched Writes Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "Speedup: " << first_duration_ms / second_duration_ms << "x" << std::endl;

    return 0;
}