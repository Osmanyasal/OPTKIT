#include <stdio.h>
#include <stdint.h>

#include "../../src/optkit_c.h"

int main(void)
{
    if (optkit_init(0, "c_api_demo") != OPTKIT_STATUS_OK)
    {
        const char *msg = NULL;
        optkit_last_error_message(&msg);
        fprintf(stderr, "optkit_init failed: %s\n", msg);
        return 1;
    }

    int16_t num_sockets;
    optkit_query_system_num_sockets(&num_sockets);
    printf("num_sockets=%d\n", num_sockets);

    int16_t logical_cores;
    optkit_query_system_num_logical_cores(&logical_cores);
    printf("num_logical_cores=%d\n", logical_cores);

    int32_t paranoid = 0;
    if (optkit_query_system_paranoid(&paranoid) == OPTKIT_STATUS_OK)
        printf("perf_event_paranoid=%d\n", (int)paranoid);

    char *packages = NULL;
    if (optkit_query_system_detect_cpu_packages_str(&packages) == OPTKIT_STATUS_OK)
    {
        printf("cpu_packages=\n%s\n", packages);
        free(packages);
    }

    // A tiny example profiling scope (nested start/stop supported)
    if (optkit_disk_start("disk_scope") != OPTKIT_STATUS_OK)
    {
        const char *msg = NULL;
        optkit_last_error_message(&msg);
        fprintf(stderr, "disk_start failed: %s\n", msg);
    }

    // ... do work here ...

    optkit_disk_stop();

    optkit_finalize();
    return 0;
}
