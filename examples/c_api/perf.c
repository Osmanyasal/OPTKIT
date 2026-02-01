/* Example: Performance profiling using optkit_perf_start/stop */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../src/optkit_c.h"

/* Optimized matmul workload: i-k-j order */
static double workload_flops(int iterations, int size)
{
    double *a = malloc(size * size * sizeof(double));
    double *b = malloc(size * size * sizeof(double));
    double *c = malloc(size * size * sizeof(double));

    if (!a || !b || !c)
    {
        free(a);
        free(b);
        free(c);
        return 0.0;
    }

    /* Initialize matrices */
    for (int i = 0; i < size * size; i++)
    {
        a[i] = 1.1;
        b[i] = 1.2;
        c[i] = 0.0;
    }

    double total = 0.0;
    for (int iter = 0; iter < iterations; iter++)
    {
        /* PERFORM LOOP INTERCHANGE (i-k-j)
           Note: We must zero out C manually now, because we are
           accumulating values rather than assigning a fresh sum.
        */

        // Reset C for this iteration
        for (int i = 0; i < size * size; i++)
            c[i] = 0.0;

        for (int i = 0; i < size; i++)
        {
            for (int k = 0; k < size; k++)
            {
                // Access 'a' once per inner loop (Invariant for the j-loop)
                double r = a[i * size + k];

                for (int j = 0; j < size; j++)
                {
                    // Sequential Access on 'c' and 'b' (Stride-1)
                    c[i * size + j] += r * b[k * size + j];
                }
            }
        }
        total += c[0];
    }

    free(a);
    free(b);
    free(c);
    return total;
}

int main(void)
{
    int8_t is_init = 0;
    optkit_is_initialized(&is_init);

    if (!is_init)
    {
        if (optkit_init(1, "run_perf_example") != OPTKIT_STATUS_OK)
        {
            const char *err;
            optkit_last_error_message(&err);
            fprintf(stderr, "Init failed: %s\n", err);
            return 1;
        }
    }

    /* Start performance profiling with metrics */
    // Note: use 'optkit-cli list cpu' to see available metrics and event names
    const char *metrics[] = {"topdown_l1", "topdown_l2_be", "topdown_l2_fe"};
    optkit_status_t status = optkit_perf_start(
        "perf_block",
        metrics, 3,
        NULL, 0);

    if (status != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "perf_start failed: %s\n", err);
        optkit_finalize();
        return 1;
    }

    /* Run workload */
    double total = workload_flops(10, 2048);

    /* Stop profiling */
    optkit_perf_stop();

    printf("Perf example complete. Total: %f\n", total);

    optkit_finalize();
    return 0;
}
