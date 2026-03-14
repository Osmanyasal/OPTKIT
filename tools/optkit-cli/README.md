
### Features

OPTKIT - Performance and Energy Profiling & Optimization Tool

A command-line interface mirroring the usability of linux/perf, providing scriptable access to the full suite of OPTKIT monitoring metrics without requiring code changes for any application.


```bash 

USAGE:
    optkit <COMMAND> [OPTIONS] [-- <PROGRAM>]

COMMANDS:
    topology [cpu|gpu]              Show system topology
    list <TYPE> [cpu|gpu]           List available components
    stat [OPTIONS] -- <PROGRAM>     Run single-shot profiling (like perf stat)
    msrmod [OPTIONS]                Read/write MSRs via /dev/cpu/<cpu>/msr_safe

TOPOLOGY:
    optkit topology                 Show complete system topology
    optkit topology cpu             Show CPU topology only
    optkit topology gpu             Show GPU topology only

LIST:
    optkit list [cpu|gpu] pmu                   List available PMU info
    optkit list [cpu|gpu|disk|memory]           List available events + metrics
    optkit list [cpu|gpu|disk|memory] events    List available events
    optkit list [cpu|gpu|disk|memory] metrics   List available metrics

PROFILING (stat):
    Single execution profiling - runs program once and collects metrics
    
    optkit stat -- <program>                                                          Default profiling
    optkit stat -e <event> -- <program>                                               Profile specific event
    optkit stat -m <metric> -- <program>                                              Profile specific metric
    optkit stat -e <event> -m <metric> -- <program>                                   Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -- <program>        Profile event + metric
    optkit stat -e <event> -m <metric> -T <sampling_period_in_ms> -S 0 -- <program>   Profile event + metric + socket 0

BENCHMARKING (--bench):
    Multiple execution analysis - runs program multiple times with different configurations
    
    optkit stat --bench freq-scaling -- <program>            Frequency scaling analysis
    optkit stat --bench core-scaling -- <program>            Core scaling analysis 

    Options can be interleaved:
    optkit stat --bench freq-scaling -e cycles -m ipc -- <program> 

MSRMOD:
    Read/write a Model-Specific Register (MSR) using msr-safe (msr_safe device).

        optkit msrmod -r -c <cpu> -a <msr_addr>
        optkit msrmod -w -c <cpu> -a <msr_addr> -v <value>

        Notes:
            - Requires msr-safe (and its `msr_safe` devices) to be available.
            - Requires sufficient permissions.

EXAMPLES:
    # Topology queries
    optkit topology
    optkit topology cpu
    optkit topology gpu

    # List capabilities
    optkit list all
    optkit list cpu all
    optkit list cpu events
    optkit list cpu metrics
    optkit list gpu all
    optkit list gpu events
    optkit list gpu metrics

    # Single-shot profiling (executes once)
    optkit stat -- ./my_program
    optkit stat -e cycles -e instructions -- ./app
    optkit stat -m ipc -m cache-miss-rate -- ./benchmark

    # Benchmark analysis (executes multiple times)
    optkit stat --bench freq-scaling -- ./compute_heavy
    optkit stat --bench core-scaling -- ./parallel_app
    optkit stat --affinity scatter -- ./threaded_app
    optkit stat --affinity numa -- ./parallel_workload

    # Interleaved options (benchmark + specific profiling)
    optkit stat --bench freq-scaling -e cycles -m ipc -- ./program --input data.txt
    optkit stat -e cache-misses -m energy --bench core-scaling -- ./app
    optkit stat --affinity compact -e instructions -m ipc -- ./multithreaded

    # MSR access (requires msr module + permissions)
    optkit msrmod -r -c 0 -a 0x1b1
    optkit msrmod -w -c 0 -a 0x1b1 -v 0x1234

NOTE:
    - 'stat' without --bench or --affinity: Single execution, collects specified events/metrics
    - 'stat' with --bench or --affinity: Multiple executions with varying configurations
      (e.g., different frequencies, core counts, affinity patterns)

```
