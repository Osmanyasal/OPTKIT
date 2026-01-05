#include <cuda_runtime.h>
#include <stdio.h>

// CUDA kernel for vector addition
#ifndef VECTOR_LEN
#define VECTOR_LEN 1024 * 1024
#endif

extern "C" void test_gpu(int vectorLen = VECTOR_LEN);

extern "C" __device__ void completed()
{
    // This function can be used to signal completion of the kernel
    // For this simple example, it does nothing
    printf("Kernel execution completed.\n");
}
extern "C" __global__ void VectorAdd(const float *A, const float *B, float *C, int N)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < N)
        C[idx] = A[idx] + B[idx];

    __syncthreads();
    if (idx == 0)
        completed();
}

void test_gpu(int vectorLen)
{
    printf("Running vector addition of length %d\n", vectorLen);
    size_t size = vectorLen * sizeof(float);

    // Host memory allocation
    float *h_A = (float *)malloc(size);
    float *h_B = (float *)malloc(size);
    float *h_C = (float *)malloc(size);

    // Initialize vectors
    for (int i = 0; i < vectorLen; ++i)
    {
        h_A[i] = rand() / (float)RAND_MAX;
        h_B[i] = rand() / (float)RAND_MAX;
    }

    // Device memory allocation
    float *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, size);
    cudaMalloc((void **)&d_B, size);
    cudaMalloc((void **)&d_C, size);

    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    int threadsPerBlock = 128;
    int blocksPerGrid = (vectorLen + threadsPerBlock - 1) / threadsPerBlock;

    // Launch the kernel
    VectorAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, vectorLen);
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(h_A);
    free(h_B);
    free(h_C);

    printf("Vector addition completed.\n");
}