/* Example: Temperature monitoring (hwmon and GPU) */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../src/optkit_c.h"

/* Simple CPU workload to generate heat */
static double workload_cpu(int iterations)
{
    double result = 0.0;
    for (int i = 0; i < iterations; i++)
    {
        for (int j = 0; j < 100000; j++)
        {
            result += (double)i * (double)j / 1.1;
        }
    }
    return result;
}

int main(void)
{
    if (optkit_init(1, "run_temperature") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "Init failed: %s\n", err);
        return 1;
    }

    /* Start hwmon temperature profiling */
    printf("=== Hardware Monitor Temperature ===\n");
    if (optkit_temperature_hwmon_start("hwmon_temp") == OPTKIT_STATUS_OK)
    {
        /* Run workload */
        double result = workload_cpu(1000);

        optkit_temperature_hwmon_stop();
        printf("Hwmon temperature profiling complete. Result: %f\n", result);
    }
    else
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "hwmon_start failed: %s\n", err);
    }

    /* GPU temperature monitoring */
    printf("\n=== GPU Temperature ===\n");
    optkit_gpu_vendor_t vendor = OPTKIT_GPU_VENDOR_NVIDIA;

    if (optkit_query_gpu_init(vendor) == OPTKIT_STATUS_OK)
    {
        int8_t is_exists = 0;
        if (optkit_query_gpu_is_device_exists(vendor, &is_exists) == OPTKIT_STATUS_OK && is_exists)
        {
            if (optkit_temperature_gpu_start("gpu_temp") == OPTKIT_STATUS_OK)
            {
                /* Monitor for a few seconds */
                printf("Monitoring GPU temperature...\n");
                sleep(2);

                /* Query current temperature */
                double gpu_temp = 0.0, mem_temp = 0.0;
                if (optkit_query_gpu_get_device_temperature(vendor, 0, &gpu_temp, &mem_temp) == OPTKIT_STATUS_OK)
                {
                    printf("Current GPU temp: %.1f°C, Memory: %.1f°C\n", gpu_temp, mem_temp);
                }

                optkit_temperature_gpu_stop();
                printf("GPU temperature profiling complete\n");
            }
        }
        else
        {
            printf("No NVIDIA GPU found\n");
        }

        optkit_query_gpu_shutdown(vendor);
    }
    else
    {
        printf("NVIDIA GPU init failed\n");
    }

    optkit_finalize();
    return 0;
}
