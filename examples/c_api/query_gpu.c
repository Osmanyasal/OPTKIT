/* Example: GPU queries - NVIDIA/AMD device information */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../src/optkit_c.h"

static void query_nvidia_gpu(void)
{
    printf("\n=== NVIDIA GPU Queries ===\n");

    optkit_gpu_vendor_t vendor = OPTKIT_GPU_VENDOR_NVIDIA;

    /* Initialize GPU query */
    if (optkit_query_gpu_init(vendor) != OPTKIT_STATUS_OK)
    {
        printf("NVIDIA GPU init failed (driver may not be installed)\n");
        return;
    }

    /* Check if devices exist */
    int is_exists = 0;
    optkit_query_gpu_is_device_exists(vendor, &is_exists);
    if (!is_exists)
    {
        printf("No NVIDIA devices found\n");
        optkit_query_gpu_shutdown(vendor);
        return;
    }

    /* Get device count */
    uint32_t count = 0;
    if (optkit_query_gpu_get_device_count(vendor, &count) == OPTKIT_STATUS_OK)
    {
        printf("NVIDIA GPU count: %u\n", count);

        /* Query first device */
        if (count > 0)
        {
            char *name = NULL;
            if (optkit_query_gpu_get_device_name(vendor, 0, &name) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 name: %s\n", name);
                optkit_free(name);
            }

            double power = 0.0;
            if (optkit_query_gpu_get_device_power(vendor, 0, &power) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 power: %.2f W\n", power);
            }

            double gpu_temp = 0.0, mem_temp = 0.0;
            if (optkit_query_gpu_get_device_temperature(vendor, 0, &gpu_temp, &mem_temp) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 temperature: GPU=%.1f°C, Memory=%.1f°C\n", gpu_temp, mem_temp);
            }

            char *basic_info = NULL;
            if (optkit_query_gpu_get_basic_info_str(vendor, 0, &basic_info) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 basic info:\n%s\n", basic_info);
                optkit_free(basic_info);
            }

            char *memory_info = NULL;
            if (optkit_query_gpu_get_memory_info_str(vendor, 0, &memory_info) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 memory info:\n%s\n", memory_info);
                optkit_free(memory_info);
            }
        }
    }

    optkit_query_gpu_shutdown(vendor);
}

static void query_amd_gpu(void)
{
    printf("\n=== AMD GPU Queries ===\n");

    optkit_gpu_vendor_t vendor = OPTKIT_GPU_VENDOR_AMD;

    if (optkit_query_gpu_init(vendor) != OPTKIT_STATUS_OK)
    {
        printf("AMD GPU init failed (driver may not be installed)\n");
        return;
    }

    int is_exists = 0;
    optkit_query_gpu_is_device_exists(vendor, &is_exists);
    if (!is_exists)
    {
        printf("No AMD devices found\n");
        optkit_query_gpu_shutdown(vendor);
        return;
    }

    uint32_t count = 0;
    if (optkit_query_gpu_get_device_count(vendor, &count) == OPTKIT_STATUS_OK)
    {
        printf("AMD GPU count: %u\n", count);

        if (count > 0)
        {
            char *name = NULL;
            if (optkit_query_gpu_get_device_name(vendor, 0, &name) == OPTKIT_STATUS_OK)
            {
                printf("Device 0 name: %s\n", name);
                optkit_free(name);
            }
        }
    }

    optkit_query_gpu_shutdown(vendor);
}

int main(void)
{
    if (optkit_init(0, "query_gpu") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "Init failed: %s\n", err);
        return 1;
    }

    query_nvidia_gpu();
    query_amd_gpu();

    optkit_finalize();
    return 0;
}
