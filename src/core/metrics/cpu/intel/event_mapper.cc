#include "core/metrics/cpu/intel/event_mapper.hh"
#if OPTKIT_ENV_CPU_AMD

#define INTEL_X86_EDGE_BIT 18
#define INTEL_X86_ANY_BIT 21
#define INTEL_X86_INV_BIT 23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE (1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY (1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV (1 << INTEL_X86_INV_BIT)

namespace optkit::core::metrics::cpu::intel
{

        const std::unordered_map<metrics::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

            {cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x003c}},

#if OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_SPR
            {cpu::CoreEvents::RESOURCE_STALLS, {0x0aa2}}, // RESOURCE_STALLS
#elif OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_SKL
            {cpu::CoreEvents::RESOURCE_STALLS, {0x01a2}}, // RESOURCE_STALLS
#endif

            // #if OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_SKL
            //             {cpu::CoreEvents::RECOVERY_CYCLES, {}},
            // #elif OPTKIT_ENV_CPU_MICROARCH_SPR
            //             {cpu::CoreEvents::RECOVERY_CYCLES, {}},
            // #else

            // Instruction Events
            {cpu::CoreEvents::INST_RETIRED, {0x00c0}}, // INSTRUCTION_RETIRED

            // Branch Prediction
            {cpu::CoreEvents::BRANCH_INST_RETIRED, {0x00c4}}, // BR_INST_RETIRED
            {cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x00c5}}, // BR_MISP_RETIRED

            // Cache Events
            {cpu::CoreEvents::L1_HITS, {0x01d1}},   // MEM_LOAD_RETIRED__L1_HIT
            {cpu::CoreEvents::L1_MISSES, {0x08d1}}, // MEM_LOAD_RETIRED__L1_MISS
            {cpu::CoreEvents::L2_HITS, {0x02d1}},   // MEM_LOAD_RETIRED__L2_HIT
            {cpu::CoreEvents::L2_MISSES, {0x10d1}}, // MEM_LOAD_RETIRED__L2_MISS
            {cpu::CoreEvents::L3_HITS, {0x04d1}},   // MEM_LOAD_RETIRED__L3_HIT
            {cpu::CoreEvents::L3_MISSES, {0x20d1}}, // MEM_LOAD_RETIRED__L3_MISS

            // Memory Events
            {cpu::CoreEvents::MEM_INST_RETIRED, {0x83d0}},  // MEM_INST_RETIRED__ANY
            {cpu::CoreEvents::MEM_LOAD_RETIRED, {0x81d0}},  // MEM_INST_RETIRED__ALL_LOADS
            {cpu::CoreEvents::MEM_STORE_RETIRED, {0x82d0}}, // MEM_INST_RETIRED__ALL_STORES

        // ITLB Miss Events
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {cpu::CoreEvents::ITLB_MISSES, {0x0e11, 0x2011}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e11}}, // ITLB_MISSES__WALK_COMPLETED
#elif OPTKIT_ENV_CPU_MICROARCH_ICL
            {cpu::CoreEvents::ITLB_MISSES, {0x0e85, 0x2085}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e85}}, // ITLB_MISSES__WALK_COMPLETED
#else
            {cpu::CoreEvents::ITLB_MISSES, {0x185, 0x2085}},        // ITLB_MISSES__MISS_CAUSES_A_WALK, ITLB_MISSES__STLB_HIT
            {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x185}}, // ITLB_MISSES__MISS_CAUSES_A_WALK
#endif

        // DTLB Miss Events
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {cpu::CoreEvents::DTLB_MISSES, {0x2013, 0x2012, 0x0e13, 0x0e12}}, // DTLB_STORE_MISSES__STLB_HIT, DTLB_LOAD_MISSES__STLB_HIT,  DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED
            {cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e13, 0x0e12}},  // DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_HSW
            {cpu::CoreEvents::DTLB_MISSES, {0x2008, 0x2049, 0x0e08, 0x0e49}}, // DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__STLB_HIT,DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__STLB_HIT, DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED
            {cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e08, 0x0e49}},  // DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED
#endif

// Software Prefetch Access
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0f40}}, // SW_PREFETCH_ACCESS
#elif OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL
            {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0f32}}, // SW_PREFETCH_ACCESS
#else
        //     {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {}}, // SW_PREFETCH_ACCESS
#endif

            // FP/Vector
            // {cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},
            //     {cpu::CoreEvents::RETIRED_SSE_AVX_FLOPS_ANY, {}}, // check native events.
            //     {cpu::CoreEvents::RETIRED_FLOPS_ANY, {}},         // check native events.
        };
        const std::unordered_map<metrics::cpu::intel::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {
            {intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL, {0x02c4}},

            {intel::NativeEvents::RESOURCE_STALLS_SB, {0x08a2}},
            {intel::NativeEvents::UOPS_CORE_CYCLES_THREAD, {0x01b1}},
            {intel::NativeEvents::UOPS_CORE_CYCLES_GE_1, {0x00b1 | 0x0200ull | (0x1 << INTEL_X86_CMASK_BIT)}},

#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {intel::NativeEvents::UOPS_ISSUED, {0x1ae}},
            {intel::NativeEvents::IDQ_MS_UOPS, {0x2079}},
#else
            {intel::NativeEvents::UOPS_ISSUED, {0x10e}},
            {intel::NativeEvents::IDQ_MS_UOPS, {0x3079}},
#endif
            {intel::NativeEvents::UOPS_RETIRED_SLOTS, {0x02c2}},
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE, {0x019c}},

#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0, {0x009c | 0x0100ull | (0x6 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::INT_MISC_RECOVERY_CYCLES, {0x01ad}},
#elif OPTKIT_ENV_CPU_MICROARCH_SKL
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0, {0x009c | 0x100 | (4 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::INT_MISC_RECOVERY_CYCLES, {0xd | 0x100 | INTEL_X86_MOD_ANY}},
#elif OPTKIT_ENV_CPU_MICROARCH_ICL
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0, {0x009c | 0x0100ull | (0x5 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::INT_MISC_RECOVERY_CYCLES, {0x010d}},
#elif OPTKIT_ENV_CPU_MICROARCH_HSV
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0, {0x009c | 0x100 | (4 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::INT_MISC_RECOVERY_CYCLES, {(0xd | 0x300 | (1 << INTEL_X86_CMASK_BIT))}},
#endif

            // fp events
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE, {0x02c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE, {0x01c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE, {0x08c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE, {0x04c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE, {0x20c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE, {0x10c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE, {0x80c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE, {0x40c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR, {0x03c7}}, // All scalar
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR, {0xfcc7}}, // All vector

            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {(0x00c3 | (0x0100ull | (0x1 << INTEL_X86_CMASK_BIT) | (0x1 << INTEL_X86_EDGE_BIT)))}},

            {intel::NativeEvents::STALLS_L1D_MISS, {}},
            {intel::NativeEvents::STALLS_L2_MISS, {}},
            {intel::NativeEvents::STALLS_L3_MISS, {}},

#if OPTKIT_ENV_CPU_MICROARCH_SPR

#endif
        };
}

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV

#endif