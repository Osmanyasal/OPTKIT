#ifndef CONSTANTS_HH
#define CONSTANTS_HH

#include <cuda_runtime.h>
#include <string>
#include <stdexcept>

// Constants for GPU matrix multiplication benchmark
constexpr int MATRIX_SIZE = 16384;      // 16384x16384 matrices - exceeds cache
constexpr int TILE_SIZE = 32;           // 16x16 shared memory tiles - larger tiles
constexpr int BENCHMARK_ITERATIONS = 3; // Fewer iterations for large matrices
constexpr int THREADS_PER_BLOCK_X = 32; // Block dimensions
constexpr int THREADS_PER_BLOCK_Y = 32;
constexpr float EPSILON = 1e-3f; // Floating point comparison tolerance (relaxed for GPU vs CPU)

// Ensure tile size matches thread block dimensions
static_assert(TILE_SIZE == THREADS_PER_BLOCK_X, "Tile size must match block X dimension");
static_assert(TILE_SIZE == THREADS_PER_BLOCK_Y, "Tile size must match block Y dimension");

// Ensure matrix size is divisible by tile size for simplicity
static_assert(MATRIX_SIZE % TILE_SIZE == 0, "Matrix size must be divisible by tile size");

inline void checkCudaError(cudaError_t error, const char *operation)
{
    if (error != cudaSuccess)
    {
        throw std::runtime_error(std::string("CUDA error in ") + operation + ": " + cudaGetErrorString(error));
    }
}

#endif // CONSTANTS_HH