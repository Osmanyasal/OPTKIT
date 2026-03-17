#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../src/optkit.hh"

void call_me()
{
        printf("Hello from call_me()!\n");
        int i = 0;
        for (; i < 100; i++);
        printf("i= %d\n", i);
}
int main(int argc, char **argv)
{
    OPTKIT_INIT();

    auto mb = optkit::metrics::MetricBuilder<uint64_t>{};
    mb.add("inst_retired", {optkit::metrics::performance::cpu_mapper::get(optkit::metrics::performance::cpu_events::INST_RETIRED)});

    {
        OPTKIT_CPU_EVENTS("main_automatic", mb);
        call_me();
    }

    {
        OPTKIT_CPU_EVENTS_DISTINCT_CORES("main_automatic_distinct_cores", mb);
        call_me();
    }

    return 0;
}