#include "core/metrics/cpu/amd/event_mapper.hh"
#if OPTKIT_ENV_CPU_AMD
namespace optkit::core::metrics::cpu::amd
{

        const std::unordered_map<metrics::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

            // Pipeline and Stalls
            {cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x76}},
            // {cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
            {cpu::CoreEvents::RESOURCE_STALLS, {0xffae}}, // focus on stall_1-> since it is directly related to resource like FP unit etc. (backend side of the cpu)
            // {cpu::CoreEvents::RECOVERY_CYCLES, {}},

            // Instruction Events
            {cpu::CoreEvents::INST_RETIRED, {0xc0}},
            // {cpu::CoreEvents::UOPS_ISSUED, {}},
            // {cpu::CoreEvents::UOPS_EXECUTED, {}},
            // {cpu::CoreEvents::UOPS_RETIRED, {}},
            // {cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

            // Branch Prediction
            {cpu::CoreEvents::BRANCH_INST_RETIRED, {0xc2}},
            {cpu::CoreEvents::BRANCH_MISP_RETIRED, {0xc3}},
// {cpu::CoreEvents::MACHINE_CLEARS, 0x0},

// Cache Events
#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4
            {cpu::CoreEvents::L1_MISSES, {0x5f44}},
            {cpu::CoreEvents::L1_HITS, {}},
#else
            {cpu::CoreEvents::L1_MISSES, {}},
            {cpu::CoreEvents::L1_HITS, {}},
#endif
            {cpu::CoreEvents::L2_MISSES, {0x0964, 0x1f71, 0x1f72}},
            {cpu::CoreEvents::L2_HITS, {0xf664, 0x1f70}},
            {cpu::CoreEvents::L3_MISSES, {0x1f72}},
            {cpu::CoreEvents::L3_HITS, {0x1f71}},

            // Memory Events
            {cpu::CoreEvents::MEM_INST_RETIRED, {0x729}},  // ls_dispatch, ldopst: single op that performs both load and store. purest: single op performs pure store. pureld: single op performs pure load.
            {cpu::CoreEvents::MEM_LOAD_RETIRED, {0x129}},  // ls_dispatch, pureld: single op only performs load (ldopst is disregarded.)
            {cpu::CoreEvents::MEM_STORE_RETIRED, {0x229}}, // ls_dispatch purest: signle op only performs store (ldopst is disregarded)

#if OPTKIT_ENV_CPU_MICROARCH_ZEN3 || OPTKIT_ENV_CPU_MICROARCH_ZEN4
            {cpu::CoreEvents::ITLB_MISSES, {0x84, 0x0f85}},          // L1 ITLB misses (hits l2 or goes for page walk)
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0f85}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#else
            {cpu::CoreEvents::ITLB_MISSES, {0x84, 0x0785}},          // L1 ITLB misses (hits l2 or goes for page walk)
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0785}}, // L1 ITLB miss  (miss l2 and goes for page walk)
#endif

            {cpu::CoreEvents::DTLB_MISSES, {0xff45}},                // L1 DTLB miss (hits l2 or goes for page walk)
            {cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0xf045}}, // L1 DTLB miss (miss l2 and goes for page walk)
            {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x7fb}},

        // FP/Vector
        // {cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // amd doesn't distinquish
#if OPTKIT_ENV_CPU_MICROARCH_ZEN
            {cpu::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY, {0xff03}}, // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0xff03}}, // sse_avx
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN2 || OPTKIT_ENV_CPU_MICROARCH_ZEN3
            {cpu::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY, {0xf03}}, // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0xf03}}, // sse_avx
#elif OPTKIT_ENV_CPU_MICROARCH_ZEN4
            {cpu::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY, {0x1f03}}, // sse_avx
            {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x1f03}}, // sse_avx
#endif
        };
}
#endif