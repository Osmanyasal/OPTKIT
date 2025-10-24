#include "solution.hh"
#include "constants.hh"
#include <cuda_runtime.h>
#include <stdexcept>

// Naive CUDA kernel - poor memory access patterns
__global__ void naive_matrix_multiply(const float *A, const float *B, float *C, int size)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < size && col < size)
    {
        float sum = 0.0f;

        // Each thread reads entire row of A and entire column of B
        // This creates poor memory coalescing and cache utilization
        for (int k = 0; k < size; k++)
        {
            sum += A[row * size + k] * B[k * size + col]; // Poor memory access pattern!
        }

        C[row * size + col] = sum;
    }
}

// Naive GPU matrix multiplication implementation
void solution(const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C, int size)
{
    size_t bytes = size * size * sizeof(float);

    // Allocate device memory
    float *d_A, *d_B, *d_C;
    checkCudaError(cudaMalloc(&d_A, bytes), "cudaMalloc A");
    checkCudaError(cudaMalloc(&d_B, bytes), "cudaMalloc B");
    checkCudaError(cudaMalloc(&d_C, bytes), "cudaMalloc C");

    // Copy data to device
    checkCudaError(cudaMemcpy(d_A, A.data(), bytes, cudaMemcpyHostToDevice), "cudaMemcpy A");
    checkCudaError(cudaMemcpy(d_B, B.data(), bytes, cudaMemcpyHostToDevice), "cudaMemcpy B");

    // Configure kernel launch parameters
    dim3 blockSize(THREADS_PER_BLOCK_X, THREADS_PER_BLOCK_Y);
    dim3 gridSize((size + blockSize.x - 1) / blockSize.x, (size + blockSize.y - 1) / blockSize.y);

    // Launch naive kernel
    naive_matrix_multiply<<<gridSize, blockSize>>>(d_A, d_B, d_C, size);
    checkCudaError(cudaGetLastError(), "kernel launch");
    checkCudaError(cudaDeviceSynchronize(), "kernel execution");

    // Copy result back to host
    checkCudaError(cudaMemcpy(C.data(), d_C, bytes, cudaMemcpyDeviceToHost), "cudaMemcpy result");

    // Clean up device memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}