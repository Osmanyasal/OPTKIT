#include "core/metrics/performance/cpu/intel/event_mapper.hh"
#if OPTKIT_ENV_CPU_INTEL

#define INTEL_X86_EDGE_BIT 18
#define INTEL_X86_ANY_BIT 21
#define INTEL_X86_INV_BIT 23
#define INTEL_X86_CMASK_BIT 24
#define INTEL_X86_MOD_EDGE (1 << INTEL_X86_EDGE_BIT)
#define INTEL_X86_MOD_ANY (1 << INTEL_X86_ANY_BIT)
#define INTEL_X86_MOD_INV (1 << INTEL_X86_INV_BIT)

namespace optkit::metrics::performance::intel
{

        const std::unordered_map<performance::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

            {performance::CoreEvents::UNHALTED_CORE_CYCLES, {0x003c}},

#if OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_SPR
            {performance::CoreEvents::RESOURCE_STALLS, {0x0aa2}}, // RESOURCE_STALLS
#elif OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_SKL
            {performance::CoreEvents::RESOURCE_STALLS, {0x01a2}}, // RESOURCE_STALLS
#endif

            // #if OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_SKL
            //             {performance::CoreEvents::RECOVERY_CYCLES, {}},
            // #elif OPTKIT_ENV_CPU_MICROARCH_SPR
            //             {performance::CoreEvents::RECOVERY_CYCLES, {}},
            // #else

            // Instruction Events
            {performance::CoreEvents::INST_RETIRED, {0x00c0}}, // INSTRUCTION_RETIRED

// Branch Prediction
#if OPTKIT_ENV_CPU_MICROARCH_SNB
            {performance::CoreEvents::BRANCH_INST_RETIRED, {0x04c4}}, // BR_INST_RETIRED
            {performance::CoreEvents::BRANCH_MISP_RETIRED, {0x04c5}}, // BR_MISP_RETIRED
#elif OPTKIT_ENV_CPU_MICROARCH_NHM
            {performance::CoreEvents::BRANCH_INST_RETIRED, {0x00c4}}, // BR_INST_RETIRED
            {performance::CoreEvents::BRANCH_MISP_RETIRED, {0x7f89}}, // BR_MISP_RETIRED
#else
            {performance::CoreEvents::BRANCH_INST_RETIRED, {0x00c4}}, // BR_INST_RETIRED
            {performance::CoreEvents::BRANCH_MISP_RETIRED, {0x00c5}}, // BR_MISP_RETIRED
#endif

        // Cache Events
#if OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM

            {performance::CoreEvents::L1_HITS, {0x01cb}},   // MEM_LOAD_RETIRED__L1D_HIT
            {performance::CoreEvents::L1_MISSES, {0x40cb}}, // MEM_LOAD_RETIRED__MASK__NHM_MEM_LOAD_RETIRED__HIT_LFB

            {performance::CoreEvents::L2_HITS, {0x02cb}},   // MEM_LOAD_RETIRED__L2_HIT
            {performance::CoreEvents::L2_MISSES, {0xaa24}}, // L2_RQSTS__MASK__NHM_L2_RQSTS__MISS

            {performance::CoreEvents::L3_HITS, {0x04cb}},   // MEM_LOAD_RETIRED__L3_UNSHARED_HIT
            {performance::CoreEvents::L3_MISSES, {0x10cb}}, // MEM_LOAD_RETIRED__L3_MISS

#elif OPTKIT_ENV_CPU_MICROARCH_SNB

            {performance::CoreEvents::L1_HITS, {0x01d1}},   // MEM_LOAD_RETIRED__L1_HIT
            {performance::CoreEvents::L1_MISSES, {0x0148}}, // L1D_PEND_MISS

            {performance::CoreEvents::L2_HITS, {0x02d1}},   // MEM_LOAD_RETIRED__L2_HIT
            {performance::CoreEvents::L2_MISSES, {0xa824}}, // L2_RQSTS | CODE_RD_MISS | PF_MISS | RFO_MISS

            {performance::CoreEvents::L3_HITS, {0x04d1}},   // MEM_LOAD_RETIRED__L3_HIT
            {performance::CoreEvents::L3_MISSES, {0x20d1}}, // MEM_LOAD_RETIRED__L3_MISS

#elif OPTKIT_ENV_CPU_MICROARCH_KNL

            {performance::CoreEvents::L1_HITS, {0x0180}},   // ICACHE__MASK__KNL_ICACHE__HIT
            {performance::CoreEvents::L1_MISSES, {0x0104}}, // MEM_LOAD_RETIRED__L1_MISS

            {performance::CoreEvents::L2_HITS, {0x0204}},   // MEM_LOAD_RETIRED__L2_HIT
            {performance::CoreEvents::L2_MISSES, {0x0404}}, // MEM_LOAD_RETIRED__L2_MISS

            //     {performance::CoreEvents::L3_HITS, {}},         // MEM_LOAD_RETIRED__L3_HIT (added as Native event calculation, check L1MPKI)
            {performance::CoreEvents::L3_MISSES, {0x412e}}, // MEM_LOAD_RETIRED__L3_MISS
#else

            {performance::CoreEvents::L1_HITS, {0x01d1}},   // MEM_LOAD_RETIRED__L1_HIT
            {performance::CoreEvents::L1_MISSES, {0x08d1}}, // MEM_LOAD_RETIRED__L1_MISS

            {performance::CoreEvents::L2_HITS, {0x02d1}},   // MEM_LOAD_RETIRED__L2_HIT
            {performance::CoreEvents::L2_MISSES, {0x10d1}}, // MEM_LOAD_RETIRED__L2_MISS

            {performance::CoreEvents::L3_HITS, {0x04d1}},   // MEM_LOAD_RETIRED__L3_HIT
            {performance::CoreEvents::L3_MISSES, {0x20d1}}, // MEM_LOAD_RETIRED__L3_MISS
#endif

        // Memory Events
#if OPTKIT_ENV_CPU_MICROARCH_KNL
            {performance::CoreEvents::MEM_INST_RETIRED, {0xc004}},  // MEM_INST_RETIRED__ANY
            {performance::CoreEvents::MEM_LOAD_RETIRED, {0x4004}},  // MEM_INST_RETIRED__ALL_LOADS
            {performance::CoreEvents::MEM_STORE_RETIRED, {0x8004}}, // MEM_INST_RETIRED__ALL_STORES

#elif OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM
            {performance::CoreEvents::MEM_INST_RETIRED, {0x030b}},  // MEM_INST_RETIRED__ANY
            {performance::CoreEvents::MEM_LOAD_RETIRED, {0x010b}},  // MEM_INST_RETIRED__ALL_LOADS
            {performance::CoreEvents::MEM_STORE_RETIRED, {0x020b}}, // MEM_INST_RETIRED__ALL_STORES
#else
            {performance::CoreEvents::MEM_INST_RETIRED, {0x83d0}},  // MEM_INST_RETIRED__ANY
            {performance::CoreEvents::MEM_LOAD_RETIRED, {0x81d0}},  // MEM_INST_RETIRED__ALL_LOADS
            {performance::CoreEvents::MEM_STORE_RETIRED, {0x82d0}}, // MEM_INST_RETIRED__ALL_STORES
#endif

        // ITLB Miss Events
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {performance::CoreEvents::ITLB_MISSES, {0x0e11, 0x2011}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e11}}, // ITLB_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_SKL
            {performance::CoreEvents::ITLB_MISSES, {0x0e85, 0x2085}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e85}}, // ITLB_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_BDW
            {performance::CoreEvents::ITLB_MISSES, {0x0e85, 0x6085}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e85}}, // ITLB_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM || OPTKIT_ENV_CPU_MICROARCH_SNB || OPTKIT_ENV_CPU_MICROARCH_IVB
            {performance::CoreEvents::ITLB_MISSES, {0x0285, 0x1085}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0285}}, // ITLB_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_KNL
            {performance::CoreEvents::ITLB_MISSES, {0x0804, 0x5 | 0x300 | INTEL_X86_MOD_EDGE | (1ULL << INTEL_X86_CMASK_BIT)}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x5 | 0x300 | INTEL_X86_MOD_EDGE | (1ULL << INTEL_X86_CMASK_BIT)}}, // ITLB_MISSES__WALK_COMPLETED
#else
            // back to SPR (since it is the latest)
            // other events has the common as "else", but since all were different, we can use the SPR as the latest one for future default.
            {performance::CoreEvents::ITLB_MISSES, {0x0e11, 0x2011}},        // ITLB_MISSES__WALK_COMPLETED, ITLB_MISSES__STLB_HIT
            {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0e11}}, // ITLB_MISSES__WALK_COMPLETED
#endif

        // DTLB Miss Events
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {performance::CoreEvents::DTLB_MISSES, {0x2013, 0x2012, 0x0e13, 0x0e12}}, // DTLB_STORE_MISSES__STLB_HIT, DTLB_LOAD_MISSES__STLB_HIT,  DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e13, 0x0e12}},  // DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL
            {performance::CoreEvents::DTLB_MISSES, {0x2008, 0x0e08, 0x2049, 0x0e49}}, // DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__STLB_HIT,DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__STLB_HIT, DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e08, 0x0e49}},  // DTLB_STORE_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__SKL_DTLB_LOAD_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_HSW || OPTKIT_ENV_CPU_MICROARCH_BDW
            {performance::CoreEvents::DTLB_MISSES, {0x6049, 0x0e49, 0x6008, 0x0e08}}, // DTLB_STORE_MISSES__MASK__HSW_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__HSW_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_STORE_MISSES__MASK__HSW_DTLB_LOAD_MISSES__STLB_HIT,DTLB_LOAD_MISSES__MASK__HSW_DTLB_LOAD_MISSES__STLB_HIT
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e49, 0x0e08}},  // DTLB_STORE_MISSES__MASK__HSW_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__MASK__HSW_DTLB_LOAD_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_WSM || OPTKIT_ENV_CPU_MICROARCH_SNB
            {performance::CoreEvents::DTLB_MISSES, {0x1008, 0x0208, 0x1049, 0x0249}}, // DTLB_LOAD_MISSES__MASK__NHM_DTLB_LOAD_MISSES__STLB_HIT, NHM_DTLB_LOAD_MISSES__WALK_COMPLETED, DTLB_MISSES__MASK__NHM_DTLB_MISSES__STLB_HIT, NHM_DTLB_MISSES__WALK_COMPLETED
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0208, 0x0249}},  // NHM_DTLB_LOAD_MISSES__WALK_COMPLETED, NHM_DTLB_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_IVB
            {performance::CoreEvents::DTLB_MISSES, {0x8208, 0x045f, 0x1049, 0x0249}}, // DTLB_STORE_MISSES__STLB_HIT, DTLB_LOAD_MISSES__STLB_HIT,  DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x8208, 0x0249}},  // DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED

#elif OPTKIT_ENV_CPU_MICROARCH_KNL
            {performance::CoreEvents::DTLB_MISSES, {0x0804, 0x300 | INTEL_X86_MOD_EDGE | (1ULL << INTEL_X86_CMASK_BIT)}},              // MEM_UOPS_RETIRED__MASK__KNL_MEM_UOPS_RETIRED__DTLB_MISS_LOADS
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x5 | 0x300 | INTEL_X86_MOD_EDGE | (1ULL << INTEL_X86_CMASK_BIT)}}, // DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED

#else
            // back to SPR (since it is the latest)
            // other events has the common as "else", but since all were different, we can use the SPR as the latest one for future default.
            {performance::CoreEvents::DTLB_MISSES, {0x2013, 0x2012, 0x0e13, 0x0e12}}, // DTLB_STORE_MISSES__STLB_HIT, DTLB_LOAD_MISSES__STLB_HIT,  DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED
            {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0e13, 0x0e12}},  // DTLB_STORE_MISSES__WALK_COMPLETED, DTLB_LOAD_MISSES__WALK_COMPLETED
#endif

// Software Prefetch Access
#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0f40}}, // SW_PREFETCH_ACCESS
#elif OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0f32}}, // SW_PREFETCH_ACCESS
#elif OPTKIT_ENV_CPU_MICROARCH_HSW
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x81d0}}, // MEM_UOPS_RETIRED.ALL_LOADS Errata: HSD29, HSM30

#elif OPTKIT_ENV_CPU_MICROARCH_BDW || /* Broadwell */    \
    OPTKIT_ENV_CPU_MICROARCH_ICL ||   /* Ice Lake */     \
    OPTKIT_ENV_CPU_MICROARCH_HSW ||   /* Haswell */      \
    OPTKIT_ENV_CPU_MICROARCH_NHM ||   /* Nehalem */      \
    OPTKIT_ENV_CPU_MICROARCH_WSM ||   /* Westmere */     \
    OPTKIT_ENV_CPU_MICROARCH_SNB ||   /* Sandy Bridge */ \
    OPTKIT_ENV_CPU_MICROARCH_IVB      /* Ivy Bridge */
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x014c}}, // LOAD_HIT_PRE.SW_PF

#else
                                                                                     // back to SPR (since it is the latest)
                                                                                     // other events has the common as "else", but since all were different, we can use the SPR as the latest one for future default.
            {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0f40}}, // SW_PREFETCH_ACCESS
#endif

            // FP/Vector
            // {performance::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},
            //     {performance::CoreEvents::RETIRED_VECTOR, {}}, // check native events.
            //     {performance::CoreEvents::RETIRED_FLOPS_ANY, {}},         // check native events.
        };

        const std::unordered_map<performance::intel::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

#if OPTKIT_ENV_CPU_MICROARCH_KNL
            {intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL, {0xf9c4}},
#else
            {intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL, {0x02c4}},
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_KNL // NOT KNL
            {intel::NativeEvents::RESOURCE_STALLS_SB, {0x08a2}},
            {intel::NativeEvents::UOPS_CORE_CYCLES_THREAD, {0x01b1}},
            {intel::NativeEvents::UOPS_CORE_CYCLES_GE_1, {0x00b1 | 0x0200ull | (0x1 << INTEL_X86_CMASK_BIT)}},
#endif

#if OPTKIT_ENV_CPU_MICROARCH_SPR
            {intel::NativeEvents::UOPS_ISSUED, {0x1ae}},
            {intel::NativeEvents::IDQ_MS_UOPS, {0x2079}},
#else
            {intel::NativeEvents::UOPS_ISSUED, {0x10e}},

#if !OPTKIT_ENV_CPU_MICROARCH_WSM && \
    !OPTKIT_ENV_CPU_MICROARCH_SNB && \
    !OPTKIT_ENV_CPU_MICROARCH_KNL
            {intel::NativeEvents::IDQ_MS_UOPS, {0x3079}},
#endif
#endif

            {intel::NativeEvents::UOPS_RETIRED_SLOTS, {0x02c2}},

#if !OPTKIT_ENV_CPU_MICROARCH_NHM && \
    !OPTKIT_ENV_CPU_MICROARCH_WSM && \
    !OPTKIT_ENV_CPU_MICROARCH_KNL
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE, {0x019c}},
#endif

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

#elif OPTKIT_ENV_CPU_MICROARCH_BDW || OPTKIT_ENV_CPU_MICROARCH_SNB || OPTKIT_ENV_CPU_MICROARCH_IVB
            {intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0, {0x009c | 0x100 | (4 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::INT_MISC_RECOVERY_CYCLES, {(0xd | 0x300 | (1 << INTEL_X86_CMASK_BIT) | INTEL_X86_MOD_ANY)}},
#endif

        // fp events
#if OPTKIT_ENV_CPU_MICROARCH_SPR || OPTKIT_ENV_CPU_MICROARCH_SKL || OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_BDW
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE, {0x02c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE, {0x01c7}},

            {intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE, {0x08c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE, {0x04c7}},

            {intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE, {0x20c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE, {0x10c7}},

#if !OPTKIT_ENV_CPU_MICROARCH_BDW
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE, {0x80c7}},
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE, {0x40c7}},
#endif
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR, {0x03c7}}, // All scalar
            {intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR, {0xfcc7}}, // All vector
#endif

#if OPTKIT_ENV_CPU_MICROARCH_SPR || /* Sapphire Rapids */ \
    OPTKIT_ENV_CPU_MICROARCH_SKL || /* Skylake */         \
    OPTKIT_ENV_CPU_MICROARCH_ICL    /* Ice Lake */
            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {(0x00c3 | (0x0100ull | (0x1 << INTEL_X86_CMASK_BIT) | (0x1 << INTEL_X86_EDGE_BIT)))}},

#elif OPTKIT_ENV_CPU_MICROARCH_HSW || \
    OPTKIT_ENV_CPU_MICROARCH_BWD ||   \
    OPTKIT_ENV_CPU_MICROARCH_SNB ||   \
    OPTKIT_ENV_CPU_MICROARCH_IVB
            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {(0x00c3 | 0x100 | INTEL_X86_MOD_EDGE | (1 << INTEL_X86_CMASK_BIT))}},
#elif OPTKIT_ENV_CPU_MICROARCH_NHM
            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {0x17c3}}, // MACHINE_CLEARS_ALL
#elif OPTKIT_ENV_CPU_MICROARCH_WSM
            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {0x07c3}}, // MACHINE_CLEARS_ALL
#else
            // back to SPR (since it is the latest)
            // other events has the common as "else", but since all were different, we can use the SPR as the latest one for future default.
            {intel::NativeEvents::MACHINE_CLEARS_COUNT, {(0x00c3 | (0x0100ull | (0x1 << INTEL_X86_CMASK_BIT) | (0x1 << INTEL_X86_EDGE_BIT)))}},
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_WSM && \
    !OPTKIT_ENV_CPU_MICROARCH_SNB && \
    !OPTKIT_ENV_CPU_MICROARCH_KNL
            {intel::NativeEvents::STALLS_L1D_MISS, {0x00a3 | 0x0c00ull | (0xc << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::STALLS_L2_MISS, {0x00a3 | 0x0500ull | (0x5 << INTEL_X86_CMASK_BIT)}},
            {intel::NativeEvents::STALLS_L3_MISS, {0x00a3 | 0x0600ull | (0x6 << INTEL_X86_CMASK_BIT)}},
#endif

#if !OPTKIT_ENV_CPU_MICROARCH_SNB
            {intel::NativeEvents::L3_DEMAND_REFERENCES, {0x4f2e}}, // L3_DEMAND_REFERENCES
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
                else if (event == "BR_INST_RETIRED_NEAR_CALL")
                        return EventMapper::get(NativeEvents::BR_INST_RETIRED_NEAR_CALL);
                else if (event == "L2_DEMAND_REFERENCES")
                        return EventMapper::get(NativeEvents::L2_DEMAND_REFERENCES);
                else if (event == "RESOURCE_STALLS_SB")
                        return EventMapper::get(NativeEvents::RESOURCE_STALLS_SB);
                else if (event == "UOPS_CORE_CYCLES_THREAD")
                        return EventMapper::get(NativeEvents::UOPS_CORE_CYCLES_THREAD);
                else if (event == "UOPS_CORE_CYCLES_GE_1")
                        return EventMapper::get(NativeEvents::UOPS_CORE_CYCLES_GE_1);
                else if (event == "L3_DEMAND_REFERENCES")
                        return EventMapper::get(NativeEvents::L3_DEMAND_REFERENCES);
                else if (event == "UOPS_ISSUED")
                        return EventMapper::get(NativeEvents::UOPS_ISSUED);
                else if (event == "UOPS_EXECUTED")
                        return EventMapper::get(NativeEvents::UOPS_EXECUTED);
                else if (event == "UOPS_RETIRED_SLOTS")
                        return EventMapper::get(NativeEvents::UOPS_RETIRED_SLOTS);
                else if (event == "INT_MISC_RECOVERY_CYCLES")
                        return EventMapper::get(NativeEvents::INT_MISC_RECOVERY_CYCLES);
                else if (event == "IDQ_MS_UOPS")
                        return EventMapper::get(NativeEvents::IDQ_MS_UOPS);
                else if (event == "IDQ_UOPS_NOT_DELIVERED_CORE")
                        return EventMapper::get(NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
                else if (event == "IDQ_UOPS_NOT_DELIVERED_CYCLES_0")
                        return EventMapper::get(NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0);
                else if (event == "MACHINE_CLEARS_COUNT")
                        return EventMapper::get(NativeEvents::MACHINE_CLEARS_COUNT);
                else if (event == "STALLS_L1D_MISS")
                        return EventMapper::get(NativeEvents::STALLS_L1D_MISS);
                else if (event == "STALLS_L2_MISS")
                        return EventMapper::get(NativeEvents::STALLS_L2_MISS);
                else if (event == "STALLS_L3_MISS")
                        return EventMapper::get(NativeEvents::STALLS_L3_MISS);
                else if (event == "FP_ARITH_INST_RETIRED_SCALAR_SINGLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
                else if (event == "FP_ARITH_INST_RETIRED_SCALAR_DOUBLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
                else if (event == "FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                else if (event == "FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                else if (event == "FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                else if (event == "FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                else if (event == "FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);
                else if (event == "FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                else if (event == "FP_ARITH_INST_RETIRED_SCALAR")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_SCALAR);
                else if (event == "FP_ARITH_INST_RETIRED_VECTOR")
                        return EventMapper::get(NativeEvents::FP_ARITH_INST_RETIRED_VECTOR);

                OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
                return {};
        }
}

#undef INTEL_X86_EDGE_BIT
#undef INTEL_X86_ANY_BIT
#undef INTEL_X86_INV_BIT
#undef INTEL_X86_CMASK_BIT
#undef INTEL_X86_MOD_EDGE
#undef INTEL_X86_MOD_ANY
#undef INTEL_X86_MOD_INV

#endif // OPTKIT_ENV_CPU_INTEL