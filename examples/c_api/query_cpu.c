/* Example: CPU queries - system, PMU, RAPL */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../src/optkit_c.h"

int main(void)
{
    if (optkit_init(0, "query_cpu") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "Init failed: %s\n", err);
        return 1;
    }

    /* System queries */
    printf("=== System Queries ===\n");

    int16_t num_sockets;
    if (optkit_query_system_num_sockets(&num_sockets) == OPTKIT_STATUS_OK)
    {
        printf("num_sockets: %d\n", num_sockets);
    }

    int16_t num_cores;
    if (optkit_query_system_num_logical_cores(&num_cores) == OPTKIT_STATUS_OK)
    {
        printf("num_logical_cores: %d\n", num_cores);
    }

    int root_enabled;
    if (optkit_query_system_is_root_priv_enabled(&root_enabled) == OPTKIT_STATUS_OK)
    {
        printf("is_root_priv_enabled: %d\n", root_enabled);
    }

    int32_t paranoid;
    if (optkit_query_system_paranoid(&paranoid) == OPTKIT_STATUS_OK)
    {
        printf("perf_event_paranoid: %d\n", (int)paranoid);
    }

    int smt_enabled;
    if (optkit_query_system_is_smt_enabled(&smt_enabled) == OPTKIT_STATUS_OK)
    {
        printf("smt_enabled: %d\n", smt_enabled);
    }

    int turbo_enabled;
    if (optkit_query_system_is_turbo_enabled(&turbo_enabled) == OPTKIT_STATUS_OK)
    {
        printf("turbo_enabled: %d\n", turbo_enabled);
    }

    char *packages = NULL;
    if (optkit_query_system_detect_cpu_packages_str(&packages) == OPTKIT_STATUS_OK)
    {
        printf("cpu_packages:\n%s\n", packages);
        optkit_free(packages);
    }

    /* PMU queries */
    printf("\n=== PMU Queries ===\n");

    int32_t pmu_ids[32];
    size_t pmu_count = 0;
    if (optkit_query_pmu_avail_pmu_ids(pmu_ids, 32, &pmu_count) == OPTKIT_STATUS_OK)
    {
        printf("Available PMU IDs (first 8): ");
        for (size_t i = 0; i < pmu_count && i < 8; i++)
        {
            printf("%d ", (int)pmu_ids[i]);
        }
        printf("\n");

        if (pmu_count > 0)
        {
            char *pmu_info = NULL;
            if (optkit_query_pmu_pmu_info_str(pmu_ids[0], &pmu_info) == OPTKIT_STATUS_OK)
            {
                printf("First PMU info:\n%s\n", pmu_info);
                optkit_free(pmu_info);
            }
        }
    }

    /* RAPL queries */
    printf("\n=== RAPL Queries ===\n");

    int32_t methods = optkit_query_rapl_avail_read_methods();
    printf("RAPL read methods bitmask: %d\n", (int)methods);

    printf("RAPL perf available: %d\n", optkit_query_rapl_is_perf_avail());
    printf("RAPL sysfs available: %d\n", optkit_query_rapl_is_sysfs_avail());
    printf("RAPL msr available: %d\n", optkit_query_rapl_is_msr_avail());

    char *rapl_info = NULL;
    if (optkit_query_rapl_domain_info_str(&rapl_info) == OPTKIT_STATUS_OK)
    {
        printf("RAPL domains:\n%s\n", rapl_info);
        optkit_free(rapl_info);
    }

    optkit_finalize();
    return 0;
}
