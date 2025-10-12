### Watch the Video

[![Watch the video](https://img.youtube.com/vi/uRmQSHsZoxE/0.jpg)](https://www.youtube.com/watch?v=uRmQSHsZoxE)

> Click the image above to play the video.



This lab assignment focuses on improving performance by eliminating false sharing. In this lab, we
have several threads that modify data located close together in memory in parallel. This causes a lot
of overhead, because the individual cores must transfer cache lines containing the modified data amongst
themselves to satisfy cache coherence.

Your task here is to eliminate the false sharing by making sure that each thread will access a separate
cache line.

Expected speedup: at least 60%.

Authored-by: Jakub Beránek (@Kobzol)

## Performance Analysis Results ##

When we first run the Topdown L1 & L2 Analysis we get the following result for the current solution
```bash
Block: solution [2472.84ms] Measured
        SMT_STALLS_1: 9387198362
        OPS_SOURCE_DISPATCHED_FROM_DECODER: 10151127992
        RETIRED_OPS: 9942020640
        BACKEND_STALLS_1: 888045309232
        DISPATCH_SLOTS: 151395176100
        DISPATCH_STALLS_1: 942177820
            bad_speculation__%: 0.023020
            Retiring__%: 1.094489
            backend_bound__%: 97.762396
            smt_contention__%: 1.033410
            frontend_bound__%: 0.103722
            
## Here we see it is a backend issue so we run l2 analysis
Block: solution [2594.91ms] Measured
        OPS_SOURCE_DISPATCHED_FROM_DECODER: 5404281896
        BRANCH_MISP_RETIRED: 35589
        RETIRED_OPS: 5360996936
        RETIRED_MICROCODE_OPS: 645701368
        CYCLES_NO_RETIRE_NOT_COMPLETE: 75576686211
        DISPATCH_STALLS_1: 496044361
        RESYNCS: 772231
        CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE: 76278727732
        BACKEND_STALLS_1: 480661840162
        DISPATCH_SLOTS: 81738008765
        DISPATCH_STALLS_1_0x6: 25174210
            bad_speculation_mispredicts__%: 0.000389
            bad_speculation_pipeline_restarts__%: 0.008437
            retiring_fastpath__%: nan
            retiring_microcode__%: nan
            backend_bound_memory__%: 97.106599
            backend_bound_cpu__%: 0.902036
            frontend_bound_latency__%: 0.030799
            frontend_bound_bw__%: 0.070347

## with l2, it is clear that our problem is at backend_bound_memory__% with 97%
```

It is expected to see high cache misses when false sharing occurs specifically on L1 and L2 based on the size. That's why in topdown analysis backend-memory is lighted up. 
Let's confirm this theory, here's the MPKI (miss per kilo instruction) results (both problem and solution.)
Based on my measurements, here're the findings

```bash
Block: solution [2769.26ms] Measured
        L3_MISSES: 74
        L2_MISSES: 137622891
        INST_RETIRED: 8922035844
        L1_MISSES: 137763470
        l3_mpki: 0.000008
        l2_mpki: 15.425055
        l1_mpki: 15.440811
Block: patch solution [112.339996ms] Measured
        L3_MISSES: 0
        L2_MISSES: 25700629
        INST_RETIRED: 7291291411
        L1_MISSES: 25271516
        l3_mpki: 0.000000
        l2_mpki: 3.524839
        l1_mpki: 3.465986
Validation Successful
First Duration (ms): 2781.245117
Second Duration (ms): 113.551003
Speedup: 24.493356x
```

Similarly, upon solving the issue, here's the l2 results comperatively. Check how backend_bound_memory__% metric goes down.


```bash
Block: solution [2551.37ms] Measured
        OPS_SOURCE_DISPATCHED_FROM_DECODER: 5395863137
        BRANCH_MISP_RETIRED: 37234
        RETIRED_OPS: 5293917064
        RETIRED_MICROCODE_OPS: 622605096
        CYCLES_NO_RETIRE_NOT_COMPLETE: 75844730960
        DISPATCH_STALLS_1: 481231697
        RESYNCS: 767687
        CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE: 76337412633
        BACKEND_STALLS_1: 479408304320
        DISPATCH_SLOTS: 81504572993
        DISPATCH_STALLS_1_0x6: 26213351
            bad_speculation_mispredicts__%: 0.000964
            bad_speculation_pipeline_restarts__%: 0.019882
            retiring_fastpath__%: nan
            retiring_microcode__%: nan
            backend_bound_memory__%: 97.400302
            backend_bound_cpu__%: 0.632705
            frontend_bound_latency__%: 0.032162
            frontend_bound_bw__%: 0.066244

Block: patch solution [114.998001ms] Measured
        OPS_SOURCE_DISPATCHED_FROM_DECODER: 4226392760
        BRANCH_MISP_RETIRED: 4406
        RETIRED_OPS: 3827002325
        RETIRED_MICROCODE_OPS: 66942464
        CYCLES_NO_RETIRE_NOT_COMPLETE: 1072787416
        DISPATCH_STALLS_1: 34862194
        RESYNCS: 5016
        CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE: 1102622913
        BACKEND_STALLS_1: 7428057043
        DISPATCH_SLOTS: 2263453287
        DISPATCH_STALLS_1_0x6: 1728433
            bad_speculation_mispredicts__%: 1.375233
            bad_speculation_pipeline_restarts__%: 1.565631
            retiring_fastpath__%: nan
            retiring_microcode__%: nan
            backend_bound_memory__%: 53.215617
            backend_bound_cpu__%: 1.479990
            frontend_bound_latency__%: 0.076363
            frontend_bound_bw__%: 0.180341
        
```
