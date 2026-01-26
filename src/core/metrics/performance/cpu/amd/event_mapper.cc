#include "core/metrics/performance/cpu/amd/event_mapper.hh"
#if OPTKIT_ENV_CPU_AMD
namespace optkit::metrics::performance::amd
{

        const std::unordered_map<metrics::performance::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

            // Pipeline and Stalls
            {performance::CoreEvents::UNHALTED_CORE_CYCLES, {0x0076}},
            // {performance::::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
            {performance::CoreEvents::RESOURCE_STALLS, {0xffae}}, // focus on stall_1-> since it is directly related to resource like FP unit etc. (backend side of the performance::)
            // {performance::::CoreEvents::RECOVERY_CYCLES, {}},

            // Instruction Events
            {performance::CoreEvents::INST_RETIRED, {0x00c0}},
            // {performance::::CoreEvents::UOPS_ISSUED, {}},
            // {performance::::CoreEvents::UOPS_EXECUTED, {}},
            // {performance::::CoreEvents::UOPS_RETIRED, {}},
            // {performance::::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

            // Branch Prediction
            {performance::CoreEvents::BRANCH_INST_RETIRED, {0x00c2}},
            {performance::CoreEvents::BRANCH_MISP_RETIRED, {0x00c3}},
// {performance::::CoreEvents::MACHINE_CLEARS, 0x0},

// Cache Events
#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::CoreEvents::L1_MISSES, {0x5f44}},
        // {performance::::CoreEvents::L1_HITS, {}},
#else
        // {performance::::CoreEvents::L1_MISSES, {}},
        // {performance::::CoreEvents::L1_HITS, {}},
#endif

            {performance::CoreEvents::L2_MISSES, {0x0964, 0xff71, 0xff72}},
            {performance::CoreEvents::L2_HITS, {0xf664, 0xff70}},

            {performance::CoreEvents::L3_MISSES, {0x0104}},
            // {performance::::CoreEvents::L3_HITS, {}},  -- do L3_CACHE_ACCESSES - L3_MISSES

            // Memory Events
            {performance::CoreEvents::MEM_INST_RETIRED, {0x0729}},  // LS_DISPATCH, ldopst: single op that performs both load and store. purest: single op performs pure store. pureld: single op performs pure load.
            {performance::CoreEvents::MEM_LOAD_RETIRED, {0x0129}},  // LS_DISPATCH, pureld: single op only performs load (ldopst is disregarded.)
            {performance::CoreEvents::MEM_STORE_RETIRED, {0x0229}}, // LS_DISPATCH purest: signle op only performs store (ldopst is disregarded)

#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::CoreEvents::ITLB_MISSES, {0x0084, 0x0f85}},        // L1 ITLB misses (hits l2 or goes for page walk)
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0f85}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#else
            {performance::CoreEvents::ITLB_MISSES, {0x0084, 0x0785}},        // L1 ITLB misses (hits l2 or goes for page walk)
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0785}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#endif

            {performance::CoreEvents::DTLB_MISSES, {0xff45}},                // L1 DTLB miss (hits l2 or goes for page walk)
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0xf045}}, // L1 DTLB miss (miss l2 and goes for page walk)
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0xFF59}},

        // FP/Vector
        // {performance::::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // amd doesn't distinquish
#if OPTKIT_ENV_CPU_MICROARCH_ZEN || OPTKIT_ENV_CPU_MICROARCH_ZEN2 || OPTKIT_ENV_CPU_MICROARCH_ZEN3
            {performance::CoreEvents::RETIRED_VECTOR, {0xff03}},    // sse_avx
            {performance::CoreEvents::RETIRED_FLOPS_ANY, {0xff03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
            {performance::CoreEvents::RETIRED_VECTOR, {0x1f03}},    // TODO: fix this, you need to seperate like intel to get all vector ops. fp_pack_ops_retired.all -- Retired packed floating-point ops of all types.
            {performance::CoreEvents::RETIRED_FLOPS_ANY, {0x1f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::CoreEvents::RETIRED_VECTOR, {0x0f03}},    // sse_avx
            {performance::CoreEvents::RETIRED_FLOPS_ANY, {0x0f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.
#else
            // Default case (zen5) for newer architectures or if not defined
            {performance::CoreEvents::RETIRED_VECTOR, {0x0f03}},    // sse_avx
            {performance::CoreEvents::RETIRED_FLOPS_ANY, {0x0f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.
#endif
        };

        const std::unordered_map<metrics::performance::amd::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

            {performance::amd::NativeEvents::RETIRED_OPS, {0x00c1}},

#if OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::amd::NativeEvents::RETIRED_MICROCODE_OPS, {0x1000000c2}},
            {performance::amd::NativeEvents::DISPATCH_STALLS_1, {0x1000001a0}}, // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {performance::amd::NativeEvents::DISPATCH_STALLS_1_0x6, {0x1060001a0}},
            {performance::amd::NativeEvents::BACKEND_STALLS_1, {0x100001ea0}}, // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {performance::amd::NativeEvents::SMT_STALLS_1, {0x1000060a0}},     // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {performance::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER, {0x07aa}},
            {performance::amd::NativeEvents::RESYNCS, {0x0096}},
            {performance::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE, {0xa2d6}},
            {performance::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE, {0x02d6}},
#endif

#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::amd::NativeEvents::L3_CACHE_ACCESSES, {0xff04}},
            {performance::amd::NativeEvents::L2_CACHE_ACCESSES, {0xf960, 0xff70, 0xff71, 0xff72}},
#endif

#if OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {performance::amd::NativeEvents::SCALAR_SINGLE_FLOPS, {0x4003}}, // fp_ret_sse_avx_ops.scalar_single -- Retired SSE and AVX scalar single-precision floating-point ops.
            {performance::amd::NativeEvents::PACKED_SINGLE_FLOPS, {0x6003}}, // fp_ret_sse_avx_ops.packed_scalar -- Retired SSE and AVX packed scalar floating-point ops.
            {performance::amd::NativeEvents::SCALAR_DOUBLE_FLOPS, {0x8003}}, // fp_ret_sse_avx_ops.scalar_double -- Retired SSE and AVX scalar double-precision floating-point ops.
            {performance::amd::NativeEvents::PACKED_DOUBLE_FLOPS, {0xa003}}, // fp_ret_sse_avx_ops.packed_double -- Retired SSE and AVX packed double-precision floating-point ops.
#endif

        };

        std::vector<uint64_t> EventMapper::get(std::string event)
        {
                if (event == "DISPATCH_SLOTS")
                        return EventMapper::get(CoreEvents::DISPATCH_SLOTS);
                else if (event == "UNHALTED_CORE_CYCLES")
                        return EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES);
                else if (event == "RESOURCE_STALLS")
                        return EventMapper::get(CoreEvents::RESOURCE_STALLS);
                else if (event == "INST_RETIRED")
                        return EventMapper::get(CoreEvents::INST_RETIRED);
                else if (event == "BRANCH_INST_RETIRED")
                        return EventMapper::get(CoreEvents::BRANCH_INST_RETIRED);
                else if (event == "BRANCH_MISP_RETIRED")
                        return EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED);
                else if (event == "L1_MISSES")
                        return EventMapper::get(CoreEvents::L1_MISSES);
                else if (event == "L1_HITS")
                        return EventMapper::get(CoreEvents::L1_HITS);
                else if (event == "L2_MISSES")
                        return EventMapper::get(CoreEvents::L2_MISSES);
                else if (event == "L2_HITS")
                        return EventMapper::get(CoreEvents::L2_HITS);
                else if (event == "L3_MISSES")
                        return EventMapper::get(CoreEvents::L3_MISSES);
                else if (event == "L3_HITS")
                        return EventMapper::get(CoreEvents::L3_HITS);
                else if (event == "MEM_INST_RETIRED")
                        return EventMapper::get(CoreEvents::MEM_INST_RETIRED);
                else if (event == "MEM_LOAD_RETIRED")
                        return EventMapper::get(CoreEvents::MEM_LOAD_RETIRED);
                else if (event == "MEM_STORE_RETIRED")
                        return EventMapper::get(CoreEvents::MEM_STORE_RETIRED);
                else if (event == "DTLB_MISSES")
                        return EventMapper::get(CoreEvents::DTLB_MISSES);
                else if (event == "ITLB_MISSES")
                        return EventMapper::get(CoreEvents::ITLB_MISSES);
                else if (event == "DTLB_MISSES_GOES_PAGE_WALK")
                        return EventMapper::get(CoreEvents::DTLB_MISSES_GOES_PAGE_WALK);
                else if (event == "ITLB_MISSES_GOES_PAGE_WALK")
                        return EventMapper::get(CoreEvents::ITLB_MISSES_GOES_PAGE_WALK);
                else if (event == "SW_LOAD_PREFETCH_ACCESS")
                        return EventMapper::get(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
                else if (event == "RETIRED_FLOPS_ANY")
                        return EventMapper::get(CoreEvents::RETIRED_FLOPS_ANY);
                else if (event == "RETIRED_VECTOR")
                        return EventMapper::get(CoreEvents::RETIRED_VECTOR);
                else if (event == "RETIRED_OPS")
                        return EventMapper::get(NativeEvents::RETIRED_OPS);
                else if (event == "RETIRED_MICROCODE_OPS")
                        return EventMapper::get(NativeEvents::RETIRED_MICROCODE_OPS);
                else if (event == "DISPATCH_STALLS_1")
                        return EventMapper::get(NativeEvents::DISPATCH_STALLS_1);
                else if (event == "DISPATCH_STALLS_1_0x6")
                        return EventMapper::get(NativeEvents::DISPATCH_STALLS_1_0x6);
                else if (event == "BACKEND_STALLS_1")
                        return EventMapper::get(NativeEvents::BACKEND_STALLS_1);
                else if (event == "SMT_STALLS_1")
                        return EventMapper::get(NativeEvents::SMT_STALLS_1);
                else if (event == "OPS_SOURCE_DISPATCHED_FROM_DECODER")
                        return EventMapper::get(NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER);
                else if (event == "RESYNCS")
                        return EventMapper::get(NativeEvents::RESYNCS);
                else if (event == "CYCLES_NO_RETIRE_NOT_COMPLETE")
                        return EventMapper::get(NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE);
                else if (event == "CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE")
                        return EventMapper::get(NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE);
                else if (event == "L3_CACHE_ACCESSES")
                        return EventMapper::get(NativeEvents::L3_CACHE_ACCESSES);
                else if (event == "L2_CACHE_ACCESSES")
                        return EventMapper::get(NativeEvents::L2_CACHE_ACCESSES);
                else if (event == "SCALAR_SINGLE_FLOPS")
                        return EventMapper::get(NativeEvents::SCALAR_SINGLE_FLOPS);
                else if (event == "PACKED_SINGLE_FLOPS")
                        return EventMapper::get(NativeEvents::PACKED_SINGLE_FLOPS);
                else if (event == "SCALAR_DOUBLE_FLOPS")
                        return EventMapper::get(NativeEvents::SCALAR_DOUBLE_FLOPS);
                else if (event == "PACKED_DOUBLE_FLOPS")
                        return EventMapper::get(NativeEvents::PACKED_DOUBLE_FLOPS);

                OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
                return {};
        }
}
#endif // OPTKIT_ENV_CPU_AMD