#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

#include "solution.hh"
#include "solution_patch.hh"
#include "constants.hh"
#include "optkit.hh"

// Forward declaration
void init(std::vector<float> &A, std::vector<float> &B, std::vector<float> &C);

// Simple CPU matrix multiplication for validation
void cpu_matrix_multiply(const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            float sum = 0.0f;
            for (int k = 0; k < size; k++)
            {
                sum += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = sum;
        }
    }
}

bool validate_results(const std::vector<float> &result, const std::vector<float> &expected, int size)
{
    for (int i = 0; i < size * size; i++)
    {
        if (std::abs(result[i] - expected[i]) > EPSILON)
        {
            std::cerr << "Validation failed at index " << i << ": got " << result[i]
                      << ", expected " << expected[i] << std::endl;
            return false;
        }
    }
    return true;
}

int main()
{
    OPTKIT_INIT(false);

    // Initialize matrices
    std::vector<float> A, B, C_naive, C_tiled, C_expected;
    init(A, B, C_naive);

    // Create copies for different implementations
    C_tiled = C_naive;
    C_expected = C_naive;

    int size = MATRIX_SIZE;
    std::cout << "Matrix size: " << size << "x" << size << std::endl;
    std::cout << "Total elements: " << size * size << std::endl;
    std::cout << "Memory per matrix: " << (size * size * sizeof(float)) / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Tile size: " << TILE_SIZE << "x" << TILE_SIZE << std::endl;

    // Compute expected result using CPU
    // std::cout << "Computing reference result on CPU..." << std::endl;
    // cpu_matrix_multiply(A, B, C_expected, size);

    // Test naive GPU implementation
    // try
    // {
    //     solution(A, B, C_naive, size);
    //     if (!validate_results(C_naive, C_expected, size))
    //     {
    //         std::cerr << "Naive GPU solution validation failed" << std::endl;
    //         return 1;
    //     }
    //     std::cout << "Naive GPU solution validation passed" << std::endl;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Naive GPU solution error: " << e.what() << std::endl;
    //     return 1;
    // }

    // // Test tiled GPU implementation
    // try
    // {
    //     solution_patch(A, B, C_tiled, size);
    //     if (!validate_results(C_tiled, C_expected, size))
    //     {
    //         std::cerr << "Tiled GPU solution validation failed" << std::endl;
    //         return 1;
    //     }
    //     std::cout << "Tiled GPU solution validation passed" << std::endl;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Tiled GPU solution error: " << e.what() << std::endl;
    //     return 1;
    // }

    // std::cout << "Validation Successful" << std::endl;

    double naive_duration_ms = 0.0;
    double tiled_duration_ms = 0.0;

    // START BENCHMARKING
    {
        std::cout << "\nBenchmarking naive GPU matrix multiplication..." << std::endl;

        optkit::utils::BlockTimer block_timer("naive GPU", naive_duration_ms);
        OPTKIT_GPU_ENERGY_EVENTS_WITH_METRICS("naive_matrix_multiply", optkit::metrics::energy::gpu_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            // Reset result matrix
            std::fill(C_naive.begin(), C_naive.end(), 0.0f);
            solution(A, B, C_naive, size);
        }
    }

    {
        std::cout << "\nBenchmarking tiled GPU matrix multiplication..." << std::endl;

        optkit::utils::BlockTimer block_timer("tiled GPU", tiled_duration_ms);
        OPTKIT_GPU_ENERGY_EVENTS_WITH_METRICS("tiled_matrix_multiply", optkit::metrics::energy::gpu_metrics::all_metrics());

        for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
        {
            // Reset result matrix
            std::fill(C_tiled.begin(), C_tiled.end(), 0.0f);
            solution_patch(A, B, C_tiled, size);
        }
    }

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Naive GPU Duration (ms): " << naive_duration_ms << std::endl;
    std::cout << "Tiled GPU Duration (ms): " << tiled_duration_ms << std::endl;
    std::cout << "Speedup: " << naive_duration_ms / tiled_duration_ms << "x" << std::endl;

    // Calculate and display performance metrics
    double operations = 2.0 * size * size * size; // 2*N^3 operations for matrix multiply
    double naive_gflops = (operations * BENCHMARK_ITERATIONS) / (naive_duration_ms * 1e6);
    double tiled_gflops = (operations * BENCHMARK_ITERATIONS) / (tiled_duration_ms * 1e6);

    std::cout << "\nPerformance Analysis:" << std::endl;
    std::cout << "Total operations: " << operations << " (2*N^3)" << std::endl;
    std::cout << "Naive GPU GFLOPS: " << naive_gflops << std::endl;
    std::cout << "Tiled GPU GFLOPS: " << tiled_gflops << std::endl;
    std::cout << "GFLOPS improvement: " << tiled_gflops / naive_gflops << "x" << std::endl;

    return 0;
}