#include <omp.h>
#include <cmath>
#include "optkit.hh"

// CUDA kernel for vector addition
#ifndef VECTOR_LEN
#define VECTOR_LEN 1024 * 1024
#endif

extern "C" void test_gpu(int vectorLen = VECTOR_LEN);

int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT(false);
    {
        OPTKIT_GPU_EVENTS("gpu_kernel_activity");
        test_gpu();
    }
    return 0;
}
