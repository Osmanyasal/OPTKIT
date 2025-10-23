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

bool validate_directory_contents(const std::string &dir, const std::vector<std::string> &expected_chunks, bool is_fragmented)
{
    if (!std::filesystem::exists(dir))
    {
        std::cerr << "Directory does not exist: " << dir << std::endl;
        return false;
    }

    size_t expected_total_size = 0;
    for (const auto &chunk : expected_chunks)
    {
        expected_total_size += chunk.size();
    }

    size_t actual_total_size = 0;

    // Count files and total size
    for (const auto &entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_regular_file())
        {
            actual_total_size += entry.file_size();
        }
    }

    if (actual_total_size != expected_total_size)
    {
        std::cerr << "Total size mismatch in " << dir << ". Expected: " << expected_total_size
                  << ", Got: " << actual_total_size << std::endl;
        return false;
    }

    // Validate expected file counts
    size_t file_count = std::distance(std::filesystem::directory_iterator(dir),
                                      std::filesystem::directory_iterator{});

    size_t expected_file_count = is_fragmented ? FRAGMENTED_FILE_COUNT : LARGE_FILE_COUNT;
    if (file_count != expected_file_count)
    {
        std::cerr << "File count mismatch in " << dir << ". Expected: " << expected_file_count
                  << ", Got: " << file_count << std::endl;
        return false;
    }

    return true;
}

void cleanup_test_directories()
{
    if (std::filesystem::exists("fragmented_test"))
    {
        std::filesystem::remove_all("fragmented_test");
    }
    if (std::filesystem::exists("consolidated_test"))
    {
        std::filesystem::remove_all("consolidated_test");
    }
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
    std::cout << "Chunk size: " << SMALL_FILE_SIZE << " bytes" << std::endl;
    std::cout << "Total data size: " << total_size / 1024 << " KB" << std::endl;
    std::cout << "Fragmented approach: " << FRAGMENTED_FILE_COUNT << " files of " << SMALL_FILE_SIZE << " bytes each" << std::endl;
    std::cout << "Consolidated approach: " << LARGE_FILE_COUNT << " files of " << LARGE_FILE_SIZE << " bytes each" << std::endl;

    cleanup_test_directories();

    // Test fragmented files solution
    try
    {
        solution(test_chunks, "fragmented_test");
        if (!validate_directory_contents("fragmented_test", test_chunks, true))
        {
            std::cerr << "Fragmented files solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fragmented files solution error: " << e.what() << std::endl;
        return 1;
    }

    // Test consolidated files solution
    try
    {
        solution_patch(test_chunks, "consolidated_test");
        if (!validate_directory_contents("consolidated_test", test_chunks, false))
        {
            std::cerr << "Consolidated files solution validation failed" << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Consolidated files solution error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Validation Successful" << std::endl;

    double first_duration_ms = 0.0;
    double second_duration_ms = 0.0;

    // START BENCHMARKING
    {
        std::cout << "\nBenchmarking fragmented files creation..." << std::endl;

        optkit::utils::BlockTimer block_timer("fragmented files", first_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("fragmented_files_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            // Clean up before each run
            if (std::filesystem::exists("fragmented_test"))
            {
                std::filesystem::remove_all("fragmented_test");
            }
            solution(test_chunks, "fragmented_test");
        }
    }

    {
        std::cout << "\nBenchmarking consolidated files creation..." << std::endl;

        optkit::utils::BlockTimer block_timer("consolidated files", second_duration_ms);
        OPTKIT_DISK_EVENTS_WITH_METRICS("consolidated_files_solution", optkit::metrics::disk::core_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            // Clean up before each run
            if (std::filesystem::exists("consolidated_test"))
            {
                std::filesystem::remove_all("consolidated_test");
            }
            solution_patch(test_chunks, "consolidated_test");
        }
    }

    cleanup_test_directories();

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Fragmented Files Duration (ms): " << first_duration_ms << std::endl;
    std::cout << "Consolidated Files Duration (ms): " << second_duration_ms << std::endl;
    std::cout << "Speedup: " << first_duration_ms / second_duration_ms << "x" << std::endl;

    return 0;
}