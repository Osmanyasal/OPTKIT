#include "core/metrics/cpu/arm/event_mapper.hh"
#if OPTKIT_ENV_CPU_ARM
namespace optkit::core::metrics::cpu::arm
{

    const std::unordered_map<metrics::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        // Pipeline and Stalls
        {cpu::CoreEvents::DISPATCH_SLOTS, {0x11}},       // CPU_CYCLES
        {cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x11}}, // CPU_CYCLES
        // {cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
        {cpu::CoreEvents::RESOURCE_STALLS, {0xffae}}, // STALL_BACKEND -- No operation has been sent for execution due to the backend (including Lack of execution units, Memory dependencies, Cache misses Register dependencies, Store buffer full Load/store queue full)
        // {cpu::CoreEvents::RECOVERY_CYCLES, {}},

        // Instruction Events
        {cpu::CoreEvents::INST_RETIRED, {0x08}}, // INST_RETIRED
        // {cpu::CoreEvents::UOPS_ISSUED, {}},
        // {cpu::CoreEvents::UOPS_EXECUTED, {}},
        // {cpu::CoreEvents::UOPS_RETIRED, {}},
        // {cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

        // Branch Prediction
        {cpu::CoreEvents::BRANCH_INST_RETIRED, {0x21}},
        {cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x22}},
        // {cpu::CoreEvents::MACHINE_CLEARS, 0x0},

        // Cache Events
        {cpu::CoreEvents::L1_MISSES, {0x03}}, // L1D_CACHE_REFILL
        // {cpu::CoreEvents::L1_HITS, {}},// do L1D_CACHE_ACCESSES - L1_MISSES

        {cpu::CoreEvents::L2_MISSES, {0x17}}, // L2D_CACHE_REFILL
        // {cpu::CoreEvents::L2_HITS, {}},// do L2_CACHE_ACCESSES - L2_MISSES

        {cpu::CoreEvents::L3_MISSES, {0x37}}, // LL_CACHE_MISS_RD
        // {cpu::CoreEvents::L3_HITS, {}},  // do L3_CACHE_ACCESSES - L3_MISSES

        // Memory Events
        {cpu::CoreEvents::MEM_INST_RETIRED, {0x13}},  // MEM_ACCESS
        {cpu::CoreEvents::MEM_LOAD_RETIRED, {0x66}},  // MEM_ACCESS_RD
        {cpu::CoreEvents::MEM_STORE_RETIRED, {0X67}}, // MEM_ACCESS_WR

        {cpu::CoreEvents::ITLB_MISSES, {0x02, 0x35}},          // L1I_TLB_REFILL + ITLB_WALK
        {cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x35}}, // ITLB_WALK -- Accesses to the instruction TLB that caused a page walk. Counts any instruction which causes L2D_TLB_REFILL to count

        {cpu::CoreEvents::DTLB_MISSES, {0x05, 0x34}},          // L1D_TLB_REFILL + DTLB_WALK
        {cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x34}}, // DTLB_WALK -- Accesses to the data TLB that caused a page walk. Counts any data access which causes L2D_TLB_REFILL to count

    //     {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {}},

    // FP/Vector
    // {cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // arm doesn't distinquish

#if !OPTKIT_ENV_CPU_ARM_N1
        {cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x80c0, 0x80c1}}, // FP_SCALE_OPS_SPEC + FP_FIXED_OPS_SPEC
        {cpu::CoreEvents::RETIRED_VECTOR, {0x80c0}},            // FP_SCALE_OPS_SPEC
#endif
    };

    const std::unordered_map<metrics::cpu::arm::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

        {cpu::arm::NativeEvents::RETIRED_OPS, {0x00c1}},

        // fe-bound
        {cpu::arm::NativeEvents::STALL_FRONTEND, {0x23}},
        {cpu::arm::NativeEvents::STALL_BACKEND, {0x24}},

#if !OPTKIT_ENV_CPU_ARM_N1
        {cpu::arm::NativeEvents::STALL_SLOT, {0x3f}},
        {cpu::arm::NativeEvents::STALL_SLOT_FRONTEND, {0x3e}},
        {cpu::arm::NativeEvents::STALL_SLOT_BACKEND, {0x3d}},
        {cpu::arm::NativeEvents::OP_RETIRED, {0x3a}},
        {cpu::arm::NativeEvents::OP_SPEC, {0x3b}},

#endif

#if OPTKIT_ENV_CPU_ARM_N3 || OPTKIT_ENV_CPU_ARM_V3
        {cpu::arm::NativeEvents::STALL_FRONTEND_FLUSH, {0x8162}},
#endif

        {cpu::arm::NativeEvents::L1D_CACHE_ACCESSES, {0x04}}, // L1D_CACHE
        {cpu::arm::NativeEvents::L2_CACHE_ACCESSES, {0x16}},  // L2D_CACHE
        {cpu::arm::NativeEvents::L3_CACHE_ACCESSES, {0x36}},  // LL_CACHE_RD
    };

}
#endif