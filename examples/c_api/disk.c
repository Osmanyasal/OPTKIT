/* Example: Disk I/O profiling */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/optkit_c.h"

/* Simple I/O workload */
static void workload_disk_io(const char *filename, int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        FILE *fp = fopen(filename, "w");
        if (fp)
        {
            char buffer[4096];
            memset(buffer, 'A' + (i % 26), sizeof(buffer));
            fwrite(buffer, 1, sizeof(buffer), fp);
            fclose(fp);
        }

        fp = fopen(filename, "r");
        if (fp)
        {
            char buffer[4096];
            fread(buffer, 1, sizeof(buffer), fp);
            fclose(fp);
        }
    }

    remove(filename);
}

int main(void)
{
    if (optkit_init(1, "disk_example") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "Init failed: %s\n", err);
        return 1;
    }

    /* Start disk I/O profiling */
    if (optkit_disk_start("disk_io") != OPTKIT_STATUS_OK)
    {
        const char *err;
        optkit_last_error_message(&err);
        fprintf(stderr, "disk_start failed: %s\n", err);
        optkit_finalize();
        return 1;
    }

    /* Run I/O workload */
    workload_disk_io("/tmp/optkit_test.tmp", 100);

    /* Stop profiling */
    optkit_disk_stop();

    printf("Disk I/O example complete\n");

    optkit_finalize();
    return 0;
}
