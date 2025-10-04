#include "core/metrics/performance/cpu/arm/event_mapper.hh"
#if OPTKIT_ENV_CPU_ARM
namespace optkit::metrics::performance::arm
{

    const std::unordered_map<metrics::performance::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        // Pipeline and Stalls
        {performance::CoreEvents::DISPATCH_SLOTS, {0x11}},       // CPU_CYCLES
        {performance::CoreEvents::UNHALTED_CORE_CYCLES, {0x11}}, // CPU_CYCLES
        // {performance::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
        {performance::CoreEvents::RESOURCE_STALLS, {0xffae}}, // STALL_BACKEND -- No operation has been sent for execution due to the backend (including Lack of execution units, Memory dependencies, Cache misses Register dependencies, Store buffer full Load/store queue full)
        // {performance::CoreEvents::RECOVERY_CYCLES, {}},

        // Instruction Events
        {performance::CoreEvents::INST_RETIRED, {0x08}}, // INST_RETIRED
        // {performance::CoreEvents::UOPS_ISSUED, {}},
        // {performance::CoreEvents::UOPS_EXECUTED, {}},
        // {performance::CoreEvents::UOPS_RETIRED, {}},
        // {performance::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

        // Branch Prediction
        {performance::CoreEvents::BRANCH_INST_RETIRED, {0x21}},
        {performance::CoreEvents::BRANCH_MISP_RETIRED, {0x22}},
        // {performance::CoreEvents::MACHINE_CLEARS, 0x0},

        // Cache Events
        {performance::CoreEvents::L1_MISSES, {0x03}}, // L1D_CACHE_REFILL
        // {performance::CoreEvents::L1_HITS, {}},// do L1D_CACHE_ACCESSES - L1_MISSES

        {performance::CoreEvents::L2_MISSES, {0x17}}, // L2D_CACHE_REFILL
        // {performance::CoreEvents::L2_HITS, {}},// do L2_CACHE_ACCESSES - L2_MISSES

        {performance::CoreEvents::L3_MISSES, {0x37}}, // LL_CACHE_MISS_RD
        // {performance::CoreEvents::L3_HITS, {}},  // do L3_CACHE_ACCESSES - L3_MISSES

        // Memory Events
        {performance::CoreEvents::MEM_INST_RETIRED, {0x13}},  // MEM_ACCESS
        {performance::CoreEvents::MEM_LOAD_RETIRED, {0x66}},  // MEM_ACCESS_RD
        {performance::CoreEvents::MEM_STORE_RETIRED, {0X67}}, // MEM_ACCESS_WR

        {performance::CoreEvents::ITLB_MISSES, {0x02, 0x35}},          // L1I_TLB_REFILL + ITLB_WALK
        {performance::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x35}}, // ITLB_WALK -- Accesses to the instruction TLB that caused a page walk. Counts any instruction which causes L2D_TLB_REFILL to count

        {performance::CoreEvents::DTLB_MISSES, {0x05, 0x34}},          // L1D_TLB_REFILL + DTLB_WALK
        {performance::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x34}}, // DTLB_WALK -- Accesses to the data TLB that caused a page walk. Counts any data access which causes L2D_TLB_REFILL to count

    //     {performance::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {}},

    // FP/Vector
    // {performance::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // arm doesn't distinquish

#if !OPTKIT_ENV_CPU_ARM_N1
        {performance::CoreEvents::RETIRED_FLOPS_ANY, {0x80c0, 0x80c1}}, // FP_SCALE_OPS_SPEC + FP_FIXED_OPS_SPEC
        {performance::CoreEvents::RETIRED_VECTOR, {0x80c0}},            // FP_SCALE_OPS_SPEC
#endif
    };

    const std::unordered_map<metrics::performance::arm::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

        {performance::arm::NativeEvents::RETIRED_OPS, {0x00c1}},

        // fe-bound
        {performance::arm::NativeEvents::STALL_FRONTEND, {0x23}},
        {performance::arm::NativeEvents::STALL_BACKEND, {0x24}},

#if !OPTKIT_ENV_CPU_ARM_N1
        {performance::arm::NativeEvents::STALL_SLOT, {0x3f}},
        {performance::arm::NativeEvents::STALL_SLOT_FRONTEND, {0x3e}},
        {performance::arm::NativeEvents::STALL_SLOT_BACKEND, {0x3d}},
        {performance::arm::NativeEvents::OP_RETIRED, {0x3a}},
        {performance::arm::NativeEvents::OP_SPEC, {0x3b}},

#endif

#if OPTKIT_ENV_CPU_ARM_N3 || OPTKIT_ENV_CPU_ARM_V3
        {performance::arm::NativeEvents::STALL_FRONTEND_FLUSH, {0x8162}},
#endif

        {performance::arm::NativeEvents::L1D_CACHE_ACCESSES, {0x04}}, // L1D_CACHE
        {performance::arm::NativeEvents::L2_CACHE_ACCESSES, {0x16}},  // L2D_CACHE
        {performance::arm::NativeEvents::L3_CACHE_ACCESSES, {0x36}},  // LL_CACHE_RD
    };

}
#endif // OPTKIT_ENV_CPU_ARM