#include "core/metrics/cpu/amd/event_mapper.hh"
#if OPTKIT_ENV_CPU_AMD
namespace optkit::core::metrics::cpu::amd
{

        const std::unordered_map<metrics::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

            // Pipeline and Stalls
            {cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x0076}},
            // {cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
            {cpu::CoreEvents::RESOURCE_STALLS, {0xffae}}, // focus on stall_1-> since it is directly related to resource like FP unit etc. (backend side of the cpu)
            // {cpu::CoreEvents::RECOVERY_CYCLES, {}},

            // Instruction Events
            {cpu::CoreEvents::INST_RETIRED, {0x00c0}},
            // {cpu::CoreEvents::UOPS_ISSUED, {}},
            // {cpu::CoreEvents::UOPS_EXECUTED, {}},
            // {cpu::CoreEvents::UOPS_RETIRED, {}},
            // {cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

            // Branch Prediction
            {cpu::CoreEvents::BRANCH_INST_RETIRED, {0x00c2}},
            {cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x00c3}},
// {cpu::CoreEvents::MACHINE_CLEARS, 0x0},

// Cache Events
#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::CoreEvents::L1_MISSES, {0x5f44}},
        // {cpu::CoreEvents::L1_HITS, {}},
#else
        // {cpu::CoreEvents::L1_MISSES, {}},
        // {cpu::CoreEvents::L1_HITS, {}},
#endif

            {cpu::CoreEvents::L2_MISSES, {0x0964, 0xff71, 0xff72}},
            {cpu::CoreEvents::L2_HITS, {0xf664, 0xff70}},

            {cpu::CoreEvents::L3_MISSES, {0x0104}},
            // {cpu::CoreEvents::L3_HITS, {}},  -- do L3_CACHE_ACCESSES - L3_MISSES

            // Memory Events
            {cpu::CoreEvents::MEM_INST_RETIRED, {0x0729}},  // LS_DISPATCH, ldopst: single op that performs both load and store. purest: single op performs pure store. pureld: single op performs pure load.
            {cpu::CoreEvents::MEM_LOAD_RETIRED, {0x0129}},  // LS_DISPATCH, pureld: single op only performs load (ldopst is disregarded.)
            {cpu::CoreEvents::MEM_STORE_RETIRED, {0x0229}}, // LS_DISPATCH purest: signle op only performs store (ldopst is disregarded)

#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::CoreEvents::ITLB_MISSES, {0x0084, 0x0f85}},        // L1 ITLB misses (hits l2 or goes for page walk)
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0f85}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#else
            {cpu::CoreEvents::ITLB_MISSES, {0x0084, 0x0785}},        // L1 ITLB misses (hits l2 or goes for page walk)
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0785}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#endif

            {cpu::CoreEvents::DTLB_MISSES, {0xff45}},                // L1 DTLB miss (hits l2 or goes for page walk)
            {cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0xf045}}, // L1 DTLB miss (miss l2 and goes for page walk)
            {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0xFF59}},

        // FP/Vector
        // {cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // amd doesn't distinquish
#if OPTKIT_ENV_CPU_MICROARCH_ZEN || OPTKIT_ENV_CPU_MICROARCH_ZEN2 || OPTKIT_ENV_CPU_MICROARCH_ZEN3
            {cpu::CoreEvents::RETIRED_VECTOR, {0xff03}},    // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0xff03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
            {cpu::CoreEvents::RETIRED_VECTOR, {0x1f03}},    // TODO: fix this, you need to seperate like intel to get all vector ops. fp_pack_ops_retired.all -- Retired packed floating-point ops of all types.
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x1f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.

#elif OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::CoreEvents::RETIRED_VECTOR, {0x0f03}},    // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x0f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.
#else
            // Default case (zen5) for newer architectures or if not defined
            {cpu::CoreEvents::RETIRED_VECTOR, {0x0f03}},    // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x0f03}}, // fp_ret_sse_avx_ops.all -- Retired SSE and AVX floating-point ops of all types, counts packed and scalar ops.
#endif
        };

        const std::unordered_map<metrics::cpu::amd::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

            {cpu::amd::NativeEvents::RETIRED_OPS, {0x00c1}},

#if OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::amd::NativeEvents::RETIRED_MICROCODE_OPS, {0x1000000c2}},
            {cpu::amd::NativeEvents::DISPATCH_STALLS_1, {0x1000001a0}}, // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {cpu::amd::NativeEvents::DISPATCH_STALLS_1_0x6, {0x1060001a0}},
            {cpu::amd::NativeEvents::BACKEND_STALLS_1, {0x100001ea0}}, // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {cpu::amd::NativeEvents::SMT_STALLS_1, {0x1000060a0}},     // Differs from PFM, exceeds 8 bits, but matches AMD docs and produces correct results with `perf stat`.
            {cpu::amd::NativeEvents::OPS_SOURCE_DISPATCHED_FROM_DECODER, {0x07aa}},
            {cpu::amd::NativeEvents::RESYNCS, {0x0096}},
            {cpu::amd::NativeEvents::CYCLES_NO_RETIRE_NOT_COMPLETE, {0xa2d6}},
            {cpu::amd::NativeEvents::CYCLES_NO_RETIRE_LOAD_NOT_COMPLETE, {0x02d6}},
#endif

#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4 || OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::amd::NativeEvents::L3_CACHE_ACCESSES, {0xff04}},
            {cpu::amd::NativeEvents::L2_CACHE_ACCESSES, {0xf960, 0xff70, 0xff71, 0xff72}},
#endif

#if OPTKIT_ENV_CPU_MICROARCH_ZEN5
            {cpu::amd::NativeEvents::SCALAR_SINGLE_FLOPS, {0x4003}}, // fp_ret_sse_avx_ops.scalar_single -- Retired SSE and AVX scalar single-precision floating-point ops.
            {cpu::amd::NativeEvents::PACKED_SINGLE_FLOPS, {0x6003}}, // fp_ret_sse_avx_ops.packed_scalar -- Retired SSE and AVX packed scalar floating-point ops.
            {cpu::amd::NativeEvents::SCALAR_DOUBLE_FLOPS, {0x8003}}, // fp_ret_sse_avx_ops.scalar_double -- Retired SSE and AVX scalar double-precision floating-point ops.
            {cpu::amd::NativeEvents::PACKED_DOUBLE_FLOPS, {0xa003}}, // fp_ret_sse_avx_ops.packed_double -- Retired SSE and AVX packed double-precision floating-point ops.
#endif

        };
}
#endif // OPTKIT_ENV_CPU_AMD