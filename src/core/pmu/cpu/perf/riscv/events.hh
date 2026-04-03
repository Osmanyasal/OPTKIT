#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT && OPTKIT_ENV_CPU_RISCV

#include <cstdint>
#include <cstring>
#include <linux/perf_event.h>

namespace optkit::pmu::cpu::perf::riscv
{
    struct EventDefinition
    {
        const char *name;
        uint32_t type;
        uint64_t config;
    };

    inline constexpr EventDefinition instructions{"instructions", PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS};

    inline constexpr uint64_t make_hw_cache_config(uint64_t cache_id, uint64_t op_id, uint64_t result_id)
    {
        return cache_id | (op_id << 8U) | (result_id << 16U);
    }

    inline constexpr EventDefinition llc_load_misses{
        "LLC-load-misses",
        PERF_TYPE_HW_CACHE,
        make_hw_cache_config(PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_MISS)};

    inline constexpr EventDefinition llc_store_misses{
        "LLC-store-misses",
        PERF_TYPE_HW_CACHE,
        make_hw_cache_config(PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_MISS)};

    inline perf_event_attr make_event_attr(const EventDefinition &event)
    {
        perf_event_attr attr;
        std::memset(&attr, 0, sizeof(attr));
        attr.type = event.type;
        attr.size = sizeof(attr);
        attr.config = event.config;
        attr.disabled = 1;
        attr.inherit = 1;
        attr.enable_on_exec = 1;
        attr.exclude_guest = 1;
        return attr;
    }

    inline perf_event_attr make_instructions_attr()
    {
        perf_event_attr attr = make_event_attr(instructions);
        attr.exclude_kernel = 1;
        attr.exclude_hv = 1;
        return attr;
    }

    inline perf_event_attr make_llc_load_misses_attr()
    {
        return make_event_attr(llc_load_misses);
    }

    inline perf_event_attr make_llc_store_misses_attr()
    {
        return make_event_attr(llc_store_misses);
    }
} // namespace optkit::pmu::cpu::perf::riscv

#endif