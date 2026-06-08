#include "core/metrics/performance/cpu/arm/event_mapper.hh"
#if OPTKIT_ENV_CPU_ARM
namespace optkit::metrics::performance::cpu::arm
{

    const std::unordered_map<performance::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        // Pipeline and Stalls
        {performance::cpu::CoreEvents::DISPATCH_SLOTS, {0x11}},       // CPU_CYCLES
        {performance::cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x11}}, // CPU_CYCLES
        // {performance::cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
        {performance::cpu::CoreEvents::RESOURCE_STALLS, {0xffae}}, // STALL_BACKEND -- No operation has been sent for execution due to the backend (including Lack of execution units, Memory dependencies, Cache misses Register dependencies, Store buffer full Load/store queue full)
        // {performance::cpu::CoreEvents::RECOVERY_CYCLES, {}},

        // Instruction Events
        {performance::cpu::CoreEvents::INST_RETIRED, {0x08}}, // INST_RETIRED
        // {performance::cpu::CoreEvents::UOPS_ISSUED, {}},
        // {performance::cpu::CoreEvents::UOPS_EXECUTED, {}},
        // {performance::cpu::CoreEvents::UOPS_RETIRED, {}},
        // {performance::cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

        // Branch Prediction
        {performance::cpu::CoreEvents::BRANCH_INST_RETIRED, {0x21}},
        {performance::cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x22}},
        // {performance::cpu::CoreEvents::MACHINE_CLEARS, 0x0},

        // Cache Events
        {performance::cpu::CoreEvents::L1_MISSES, {0x03}}, // L1D_CACHE_REFILL
        // {performance::cpu::CoreEvents::L1_HITS, {}},// do L1D_CACHE_ACCESSES - L1_MISSES

        {performance::cpu::CoreEvents::L2_MISSES, {0x17}}, // L2D_CACHE_REFILL
        // {performance::cpu::CoreEvents::L2_HITS, {}},// do L2_CACHE_ACCESSES - L2_MISSES

        {performance::cpu::CoreEvents::L3_MISSES, {0x37}}, // LL_CACHE_MISS_RD
        // {performance::cpu::CoreEvents::L3_HITS, {}},  // do L3_CACHE_ACCESSES - L3_MISSES

        // Memory Events
        {performance::cpu::CoreEvents::MEM_INST_RETIRED, {0x13}},  // MEM_ACCESS
        {performance::cpu::CoreEvents::MEM_LOAD_RETIRED, {0x66}},  // MEM_ACCESS_RD
        {performance::cpu::CoreEvents::MEM_STORE_RETIRED, {0X67}}, // MEM_ACCESS_WR

        {performance::cpu::CoreEvents::ITLB_MISSES, {0x02, 0x35}},          // L1I_TLB_REFILL + ITLB_WALK
        {performance::cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x35}}, // ITLB_WALK -- Accesses to the instruction TLB that caused a page walk. Counts any instruction which causes L2D_TLB_REFILL to count

        {performance::cpu::CoreEvents::DTLB_MISSES, {0x05, 0x34}},          // L1D_TLB_REFILL + DTLB_WALK
        {performance::cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x34}}, // DTLB_WALK -- Accesses to the data TLB that caused a page walk. Counts any data access which causes L2D_TLB_REFILL to count

    //     {performance::cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {}},

    // FP/Vector
    // {performance::cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // arm doesn't distinquish

#if !OPTKIT_ENV_CPU_ARM_N1
        {performance::cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x80c0, 0x80c1}}, // FP_SCALE_OPS_SPEC + FP_FIXED_OPS_SPEC
        {performance::cpu::CoreEvents::RETIRED_VECTOR, {0x80c0}},            // FP_SCALE_OPS_SPEC
#endif
    };

    const std::unordered_map<performance::cpu::arm::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {

        {performance::cpu::arm::NativeEvents::RETIRED_OPS, {0x00c1}},
        {performance::cpu::arm::NativeEvents::L1D_CACHE_INVAL, {0x44}},

        // fe-bound
        {performance::cpu::arm::NativeEvents::STALL_FRONTEND, {0x23}},
        {performance::cpu::arm::NativeEvents::STALL_BACKEND, {0x24}},

#if !OPTKIT_ENV_CPU_ARM_N1
        {performance::cpu::arm::NativeEvents::STALL_SLOT, {0x3f}},
        {performance::cpu::arm::NativeEvents::STALL_SLOT_FRONTEND, {0x3e}},
        {performance::cpu::arm::NativeEvents::STALL_SLOT_BACKEND, {0x3d}},
        {performance::cpu::arm::NativeEvents::OP_RETIRED, {0x3a}},
        {performance::cpu::arm::NativeEvents::OP_SPEC, {0x3b}},

#endif

#if OPTKIT_ENV_CPU_ARM_N3 || OPTKIT_ENV_CPU_ARM_V3
        {performance::cpu::arm::NativeEvents::STALL_FRONTEND_FLUSH, {0x8162}},
#endif

        {performance::cpu::arm::NativeEvents::L1D_CACHE_ACCESSES, {0x04}}, // L1D_CACHE
        {performance::cpu::arm::NativeEvents::L2_CACHE_ACCESSES, {0x16}},  // L2D_CACHE
        {performance::cpu::arm::NativeEvents::L3_CACHE_ACCESSES, {0x36}},  // LL_CACHE_RD
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
        else if (event == "STALL_FRONTEND")
            return EventMapper::get(NativeEvents::STALL_FRONTEND);
        else if (event == "STALL_BACKEND")
            return EventMapper::get(NativeEvents::STALL_BACKEND);
        else if (event == "STALL_SLOT")
            return EventMapper::get(NativeEvents::STALL_SLOT);
        else if (event == "STALL_SLOT_FRONTEND")
            return EventMapper::get(NativeEvents::STALL_SLOT_FRONTEND);
        else if (event == "STALL_SLOT_BACKEND")
            return EventMapper::get(NativeEvents::STALL_SLOT_BACKEND);
        else if (event == "OP_RETIRED")
            return EventMapper::get(NativeEvents::OP_RETIRED);
        else if (event == "OP_SPEC")
            return EventMapper::get(NativeEvents::OP_SPEC);
        else if (event == "STALL_FRONTEND_FLUSH")
            return EventMapper::get(NativeEvents::STALL_FRONTEND_FLUSH);
        else if (event == "L1D_CACHE_ACCESSES")
            return EventMapper::get(NativeEvents::L1D_CACHE_ACCESSES);
        else if (event == "L2_CACHE_ACCESSES")
            return EventMapper::get(NativeEvents::L2_CACHE_ACCESSES);
        else if (event == "L3_CACHE_ACCESSES")
            return EventMapper::get(NativeEvents::L3_CACHE_ACCESSES);

        OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
        return {};
    }
}
#endif // OPTKIT_ENV_CPU_ARM