#include "core/metrics/cpu/intel/event_wrapper.hh"

namespace optkit::core::metrics::cpu::intel
{

    const std::unordered_map<cpu::CoreEvents, std::vector<uint64_t>> IntelEventWrapper::core_event_map = {

        // Pipeline and Stalls
        {cpu::CoreEvents::UNHALTED_CORE_CYCLES, {0x0}},
        {cpu::CoreEvents::UNHALTED_REFERENCE_CYCLES, {0x0}},
        {cpu::CoreEvents::RESOURCE_STALLS, {0x0}},
        {cpu::CoreEvents::RECOVERY_CYCLES, {0x0}},

        // Instruction Events
        {cpu::CoreEvents::INST_RETIRED, {0x0}},
        {cpu::CoreEvents::UOPS_ISSUED, {0x0}},
        {cpu::CoreEvents::UOPS_EXECUTED, {0x0}},
        {cpu::CoreEvents::UOPS_RETIRED, {0x0}},
        {cpu::CoreEvents::IDQ_UOPS_NOT_DELIVERED, {0x0}},

        // Branch Prediction
        {cpu::CoreEvents::BRANCH_INST_RETIRED, {0x0}},
        {cpu::CoreEvents::BRANCH_MISP_RETIRED, {0x0}},
        {cpu::CoreEvents::MACHINE_CLEARS, {0x0}},

        // Cache Events
        {cpu::CoreEvents::L1_MISSES, {0x0}},
        {cpu::CoreEvents::L1_HITS, {0x0}},
        {cpu::CoreEvents::L2_MISSES, {0x0}},
        {cpu::CoreEvents::L2_HITS, {0x0}},
        {cpu::CoreEvents::L3_MISSES, {0x0}},
        {cpu::CoreEvents::L3_HITS, {0x0}},

        // Memory Events
        {cpu::CoreEvents::MEM_INST_RETIRED, {0x0}},
        {cpu::CoreEvents::MEM_LOAD_RETIRED, {0x0}},
        {cpu::CoreEvents::MEM_STORE_RETIRED, {0x0}},
        {cpu::CoreEvents::DTLB_MISSES, {0x0}},
        {cpu::CoreEvents::ITLB_MISSES, {0x0}},
        {cpu::CoreEvents::SW_LOAD_PREFETCH_ACCESS, {0x0}},

        // FP/Vector
        {cpu::CoreEvents::FP_ARITH_INST_RETIRED, {0x0}},
        {cpu::CoreEvents::FP_ARITH_INST_VECTOR_RETIRED, {0x0}},
    };


    const std::unordered_map<cpu::intel::CoreEvents, std::vector<uint64_t>> IntelEventWrapper::intel_event_map = {

    };
}