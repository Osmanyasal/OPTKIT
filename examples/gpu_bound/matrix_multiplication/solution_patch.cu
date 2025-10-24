#include "solution_patch.hh"
#include "constants.hh"
#include <cuda_runtime.h>
#include <stdexcept>

// Optimized constants for better performance
constexpr int BLOCK_ROWS = 16;
constexpr int BLOCK_COLS = 16;
constexpr int REG_TILE_M = 8; // Each thread computes 8x8 output elements
constexpr int REG_TILE_N = 8;
constexpr int SHARED_TILE_K = 8; // K-dimension tile size

// Highly optimized CUDA kernel with register blocking and vectorized loads
__global__ void optimized_tiled_matrix_multiply(const float *A, const float *B, float *C, int size)
{
    // Larger shared memory tiles
    __shared__ float tile_A[BLOCK_ROWS * REG_TILE_M][SHARED_TILE_K];
    __shared__ float tile_B[SHARED_TILE_K][BLOCK_COLS * REG_TILE_N];

    // Thread indices
    const int tx = threadIdx.x;
    const int ty = threadIdx.y;
    const int bx = blockIdx.x;
    const int by = blockIdx.y;

    // Calculate global output position for this thread's tile
    const int row_base = (by * BLOCK_ROWS + ty) * REG_TILE_M;
    const int col_base = (bx * BLOCK_COLS + tx) * REG_TILE_N;

    // Register file for accumulating results (8x8 = 64 registers per thread)
    float acc[REG_TILE_M][REG_TILE_N];

// Initialize accumulators
#pragma unroll
    for (int i = 0; i < REG_TILE_M; i++)
    {
#pragma unroll
        for (int j = 0; j < REG_TILE_N; j++)
        {
            acc[i][j] = 0.0f;
        }
    }

    // Process matrix in tiles along K dimension
    const int num_tiles = (size + SHARED_TILE_K - 1) / SHARED_TILE_K;

    for (int tile = 0; tile < num_tiles; tile++)
    {
        const int k_base = tile * SHARED_TILE_K;

// Cooperatively load tile_A into shared memory
// Each thread loads multiple elements with vectorization
#pragma unroll
        for (int i = 0; i < REG_TILE_M; i++)
        {
            const int row = row_base + i;
            if (row < size)
            {
#pragma unroll
                for (int k = tx; k < SHARED_TILE_K; k += BLOCK_COLS)
                {
                    const int col = k_base + k;
                    tile_A[ty * REG_TILE_M + i][k] = (col < size) ? A[row * size + col] : 0.0f;
                }
            }
            else
            {
#pragma unroll
                for (int k = tx; k < SHARED_TILE_K; k += BLOCK_COLS)
                {
                    tile_A[ty * REG_TILE_M + i][k] = 0.0f;
                }
            }
        }

// Cooperatively load tile_B into shared memory
#pragma unroll
        for (int k = ty; k < SHARED_TILE_K; k += BLOCK_ROWS)
        {
            const int row = k_base + k;
            if (row < size)
            {
#pragma unroll
                for (int j = 0; j < REG_TILE_N; j++)
                {
                    const int col = col_base + j;
                    tile_B[k][tx * REG_TILE_N + j] = (col < size) ? B[row * size + col] : 0.0f;
                }
            }
            else
            {
#pragma unroll
                for (int j = 0; j < REG_TILE_N; j++)
                {
                    tile_B[k][tx * REG_TILE_N + j] = 0.0f;
                }
            }
        }

        __syncthreads();

// Compute using register blocking
#pragma unroll
        for (int k = 0; k < SHARED_TILE_K; k++)
        {
            // Load shared memory values into registers
            float a_reg[REG_TILE_M];
            float b_reg[REG_TILE_N];

#pragma unroll
            for (int i = 0; i < REG_TILE_M; i++)
            {
                a_reg[i] = tile_A[ty * REG_TILE_M + i][k];
            }

#pragma unroll
            for (int j = 0; j < REG_TILE_N; j++)
            {
                b_reg[j] = tile_B[k][tx * REG_TILE_N + j];
            }

// Compute outer product and accumulate
#pragma unroll
            for (int i = 0; i < REG_TILE_M; i++)
            {
#pragma unroll
                for (int j = 0; j < REG_TILE_N; j++)
                {
                    acc[i][j] += a_reg[i] * b_reg[j];
                }
            }
        }

        __syncthreads();
    }

// Write results back to global memory
#pragma unroll
    for (int i = 0; i < REG_TILE_M; i++)
    {
        const int row = row_base + i;
        if (row < size)
        {
#pragma unroll
            for (int j = 0; j < REG_TILE_N; j++)
            {
                const int col = col_base + j;
                if (col < size)
                {
                    C[row * size + col] = acc[i][j];
                }
            }
        }
    }
}

void solution_patch(const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C, int size)
{
    size_t bytes = size * size * sizeof(float);

    float *d_A, *d_B, *d_C;
    checkCudaError(cudaMalloc(&d_A, bytes), "cudaMalloc A");
    checkCudaError(cudaMalloc(&d_B, bytes), "cudaMalloc B");
    checkCudaError(cudaMalloc(&d_C, bytes), "cudaMalloc C");

    checkCudaError(cudaMemcpy(d_A, A.data(), bytes, cudaMemcpyHostToDevice), "cudaMemcpy A");
    checkCudaError(cudaMemcpy(d_B, B.data(), bytes, cudaMemcpyHostToDevice), "cudaMemcpy B");

    // Choose which kernel to use:

    // Option 1: Most optimized with register blocking (best for large matrices)
    dim3 blockSize1(BLOCK_COLS, BLOCK_ROWS);
    dim3 gridSize1(
        (size + BLOCK_COLS * REG_TILE_N - 1) / (BLOCK_COLS * REG_TILE_N),
        (size + BLOCK_ROWS * REG_TILE_M - 1) / (BLOCK_ROWS * REG_TILE_M));
    optimized_tiled_matrix_multiply<<<gridSize1, blockSize1>>>(d_A, d_B, d_C, size);

    // Option 2: Simpler but still fast (uncomment to use instead)
    // constexpr int TILE_V2 = 64;
    // constexpr int THREADS_V2 = 16;
    // dim3 blockSize2(THREADS_V2, THREADS_V2);
    // dim3 gridSize2((size + TILE_V2 - 1) / TILE_V2, (size + TILE_V2 - 1) / TILE_V2);
    // tiled_matrix_multiply_v2<<<gridSize2, blockSize2>>>(d_A, d_B, d_C, size);

    checkCudaError(cudaGetLastError(), "kernel launch");
    checkCudaError(cudaDeviceSynchronize(), "kernel execution");

    checkCudaError(cudaMemcpy(C.data(), d_C, bytes, cudaMemcpyDeviceToHost), "cudaMemcpy result");

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}