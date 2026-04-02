#include "core/metrics/performance/cpu/riscv/event_mapper.hh"
#if OPTKIT_ENV_CPU_RISCV
namespace optkit::metrics::performance::cpu::riscv
{

    const std::unordered_map<performance::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {

        // Pipeline and Stalls
        // {performance::cpu::CoreEvents::DISPATCH_SLOTS, {0x0}},       // CPU_CYCLES
        {performance::cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x0}}, // CPU_CYCLES
        {performance::cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {}}, // not exists
        // {performance::cpu::CoreEvents::RESOURCE_STALLS, {0x0}}, // STALL_BACKEND -- No operation has been sent for execution due to the backend (including Lack of execution units, Memory dependencies, Cache misses Register dependencies, Store buffer full Load/store queue full)
        // {performance::cpu::CoreEvents::RECOVERY_CYCLES, {}},

        // Instruction Events
        {performance::cpu::CoreEvents::INST_RETIRED, {0x0}}, // INST_RETIRED
        // {performance::cpu::CoreEvents::UOPS_ISSUED, {}},
        // {performance::cpu::CoreEvents::UOPS_EXECUTED, {}},
        // {performance::cpu::CoreEvents::UOPS_RETIRED, {}},
        // {performance::cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {}},

        // Branch Prediction
        {performance::cpu::CoreEvents::BRANCH_INST_RETIRED, {0x0}},
        {performance::cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x0}},
        // {performance::cpu::CoreEvents::MACHINE_CLEARS, 0x0},

        // Cache Events
        // {performance::cpu::CoreEvents::L1_MISSES, {0x0}}, // L1D_CACHE_REFILL
        // {performance::cpu::CoreEvents::L1_HITS, {}},// do L1D_CACHE_ACCESSES - L1_MISSES

        // {performance::cpu::CoreEvents::L2_MISSES, {0x0}}, // L2D_CACHE_REFILL
        // {performance::cpu::CoreEvents::L2_HITS, {}},// do L2_CACHE_ACCESSES - L2_MISSES

        // {performance::cpu::CoreEvents::L3_MISSES, {0x0}}, // LL_CACHE_MISS_RD
        // {performance::cpu::CoreEvents::L3_HITS, {}},  // do L3_CACHE_ACCESSES - L3_MISSES

        // Memory Events
        // {performance::cpu::CoreEvents::MEM_INST_RETIRED, {0x0}},  // MEM_ACCESS
        // {performance::cpu::CoreEvents::MEM_LOAD_RETIRED, {0x0}},  // MEM_ACCESS_RD
        // {performance::cpu::CoreEvents::MEM_STORE_RETIRED, {0x0}}, // MEM_ACCESS_WR

        // {performance::cpu::CoreEvents::ITLB_MISSES, {0x0, 0x0}},          // L1I_TLB_REFILL + ITLB_WALK
        // {performance::cpu::CoreEvents::ITLB_MISSES_GOES_PAGE_WALK, {0x0}}, // ITLB_WALK -- Accesses to the instruction TLB that caused a page walk. Counts any instruction which causes L2D_TLB_REFILL to count

        // {performance::cpu::CoreEvents::DTLB_MISSES, {0x0, 0x0}},          // L1D_TLB_REFILL + DTLB_WALK
        // {performance::cpu::CoreEvents::DTLB_MISSES_GOES_PAGE_WALK, {0x0}}, // DTLB_WALK -- Accesses to the data TLB that caused a page walk. Counts any data access which causes L2D_TLB_REFILL to count

        //     {performance::cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {}},

        // FP/Vector
        // {performance::cpu::CoreEvents::FP_ARITH_INST_RETIRED, 0x0},   // arm doesn't distinquish
        // {performance::cpu::CoreEvents::RETIRED_FLOPS_ANY, {0x0, 0x0}}, // FP_SCALE_OPS_SPEC + FP_FIXED_OPS_SPEC
        // {performance::cpu::CoreEvents::RETIRED_VECTOR, {0x0}},            // FP_SCALE_OPS_SPEC
    };

    const std::unordered_map<metrics::performance::cpu::riscv::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {
 
    };
    std::vector<uint64_t> EventMapper::get(std::string event)
    {
         
        OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
        return {};
    }
}
#endif // OPTKIT_ENV_CPU_RISCV