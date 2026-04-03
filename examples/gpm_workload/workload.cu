#include <cuda_runtime.h>
#include <stdio.h>
#include <stdint.h>

#define REPEAT10(x) x x x x x x x x x x

#define NBLOCKS 250
#define NTHREADS 512
#define NL 100
#define NKREPS 250

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

__global__ void workload(float *ar, uint64_t *timer)
{
    uint64_t t0, t;
    int tid = threadIdx.x + blockIdx.x * blockDim.x;

    asm volatile("mov.u64 %0, %%globaltimer;\n\t" : "=l"(t0) : : "memory");

    asm volatile(".reg .f32 regz;\n\t"
                 ".reg .f32 regc;\n\t"
                 ".reg .u64 idx;\n\t"
                 ".reg .pred jmp;\n\t"

                 "ld.global.ca.f32 regz, [%0];\n\t"
                 "xor.b32 regc, regc, regc;\n\t"
                 "mov.u64 idx, 0;\n\t"

                 "loop:\n\t"
                 REPEAT10(
                 REPEAT10(
                 REPEAT10(
                 REPEAT10(
                 "fma.rn.f32 regz, regz, regz, regc;\n\t"
                 )
                 )
                 )
                 )

                "st.global.f32 [%0], regz;\n\t"

                "add.u64 idx, idx, 1;\n\t"
                "setp.lt.u64 jmp, idx, " QUOTE(NL) ";\n\t"
                "@jmp bra loop;\n\t"
                :: "l"(ar + tid) : "memory");

    asm volatile("mov.u64 %0, %%globaltimer;\n\t" : "=l"(t) : : "memory");

    timer[tid] = t - t0;
}

extern "C" void run_workload()
{
    float *ar;
    uint64_t *timer;

    cudaMallocHost((void **)&ar, NBLOCKS * NTHREADS * sizeof(float));
    cudaMallocHost((void **)&timer, NBLOCKS * NTHREADS * NKREPS * sizeof(uint64_t));

    float inc = 2.0f / (float)(NBLOCKS * NTHREADS);
    for (int i = 0; i < NTHREADS * NBLOCKS; i++)
        ar[i] = -2.25f + i * inc;

    for (int i = 0; i < NKREPS; i++)
        workload<<<NBLOCKS, NTHREADS>>>(ar, timer + i * NBLOCKS * NTHREADS);

    cudaDeviceSynchronize();

    cudaFreeHost(ar);
    cudaFreeHost(timer);

    printf("GPU workload completed (%d kernel launches).\n", NKREPS);
}
