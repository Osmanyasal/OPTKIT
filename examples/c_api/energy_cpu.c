/* Example: CPU energy profiling (RAPL) */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../src/optkit_c.h"

/* Simple compute workload */
static double workload_flops(int iterations, int size)
{
    double *a = malloc(size * size * sizeof(double));
    double *b = malloc(size * size * sizeof(double));

    if (!a || !b)
    {
        free(a);
        free(b);
        return 0.0;
    }

    for (int i = 0; i < size * size; i++)
    {
        a[i] = 1.1;
        b[i] = 1.2;
    }

    double total = 0.0;
    for (int iter = 0; iter < iterations; iter++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                double sum = 0.0;
                for (int k = 0; k < size; k++)
                {
                    sum += a[i * size + k] * b[k * size + j];
                }
                total += sum;
            }
        }
    }

    free(a);
    free(b);
    return total;
}

int main(void)
{
    if (optkit_init(1, "energy_cpu") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "Init failed: %s\n", err);
        return 1;
    }

    /* Start CPU energy profiling */
    if (optkit_energy_cpu_start("cpu_energy") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "energy_cpu_start failed: %s\n", err);
        optkit_finalize();
        return 1;
    }

    /* Run workload */
    double total = workload_flops(20, 256);

    /* Stop profiling */
    optkit_energy_cpu_stop();

    printf("CPU energy example complete. Total: %f\n", total);

    optkit_finalize();
    return 0;
}
