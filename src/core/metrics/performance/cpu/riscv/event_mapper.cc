#include "core/metrics/performance/cpu/riscv/event_mapper.hh"

#if OPTKIT_ENV_CPU_RISCV

#include "core/pmu/cpu/perf/riscv/events.hh"

namespace optkit::metrics::performance::cpu::riscv
{
    const std::unordered_map<performance::cpu::CoreEvents, std::vector<uint64_t>> EventMapper::core_event_map = {
        {performance::cpu::CoreEvents::INST_RETIRED, {PERF_COUNT_HW_INSTRUCTIONS}},

    };

    const std::unordered_map<performance::cpu::riscv::NativeEvents, std::vector<uint64_t>> EventMapper::native_event_map = {
        {performance::cpu::riscv::NativeEvents::LLC_LOAD_MISSES,
         {
             optkit::pmu::cpu::perf::riscv::make_hw_cache_config(
                 PERF_COUNT_HW_CACHE_LL,
                 PERF_COUNT_HW_CACHE_OP_READ,
                 PERF_COUNT_HW_CACHE_RESULT_MISS),
         }},
        {performance::cpu::riscv::NativeEvents::LLC_STORE_MISSES,
         {
             optkit::pmu::cpu::perf::riscv::make_hw_cache_config(
                 PERF_COUNT_HW_CACHE_LL,
                 PERF_COUNT_HW_CACHE_OP_WRITE,
                 PERF_COUNT_HW_CACHE_RESULT_MISS),
         }},
    };

    std::vector<uint64_t> EventMapper::get(std::string event)
    {
        if (event == "INST_RETIRED")
            return EventMapper::get(CoreEvents::INST_RETIRED);
        else if (event == "LLC-load-misses")
            return {optkit::pmu::cpu::perf::riscv::llc_load_misses.config};
        else if (event == "LLC-store-misses")
            return {optkit::pmu::cpu::perf::riscv::llc_store_misses.config};

        OPTKIT_CORE_WARN("EventMapper: No event found for event string: {}", event);
        return {};
    }
}

#endif // OPTKIT_ENV_CPU_RISCV