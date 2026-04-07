#pragma once

#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_PERF_EVENT && OPTKIT_ENV_CPU_RISCV

#include <cstdint>
#include <cstring>
#include <linux/perf_event.h>
#include <string>

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

    inline constexpr EventDefinition llc_load{
        "LLC-load",
        PERF_TYPE_HW_CACHE,
        make_hw_cache_config(PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_ACCESS)};

    inline constexpr EventDefinition llc_store{
        "LLC-store",
        PERF_TYPE_HW_CACHE,
        make_hw_cache_config(PERF_COUNT_HW_CACHE_LL, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_RESULT_ACCESS)};

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
        return make_event_attr(instructions);
    }

    inline perf_event_attr make_llc_load_attr()
    {
        return make_event_attr(llc_load);
    }

    inline perf_event_attr make_llc_store_attr()
    {
        return make_event_attr(llc_store);
    }

    inline perf_event_attr make_llc_load_misses_attr()
    {
        return make_event_attr(llc_load_misses);
    }

    inline perf_event_attr make_llc_store_misses_attr()
    {
        return make_event_attr(llc_store_misses);
    }

    inline bool try_resolve_event_attr(const std::string &event_name, uint64_t event_config, perf_event_attr &attr)
    {
        if (event_name == instructions.name || event_config == instructions.config)
        {
            attr = make_instructions_attr();
            return true;
        }

        if (event_name == llc_load.name || event_config == llc_load.config)
        {
            attr = make_llc_load_attr();
            return true;
        }

        if (event_name == llc_store.name || event_config == llc_store.config)
        {
            attr = make_llc_store_attr();
            return true;
        }

        if (event_name == llc_load_misses.name || event_config == llc_load_misses.config)
        {
            attr = make_llc_load_misses_attr();
            return true;
        }

        if (event_name == llc_store_misses.name || event_config == llc_store_misses.config)
        {
            attr = make_llc_store_misses_attr();
            return true;
        }

        return false;
    }

    inline void apply_event_attr(perf_event_attr &attr, const std::string &event_name, uint64_t event_config)
    {
        perf_event_attr resolved_attr;
        if (!try_resolve_event_attr(event_name, event_config, resolved_attr))
            return;

        attr.type = resolved_attr.type;
        attr.size = resolved_attr.size;
        attr.config = resolved_attr.config;
        attr.disabled = resolved_attr.disabled;
        attr.inherit = resolved_attr.inherit;
        attr.enable_on_exec = resolved_attr.enable_on_exec;
        attr.exclude_user = resolved_attr.exclude_user;
        attr.exclude_kernel = resolved_attr.exclude_kernel;
        attr.exclude_hv = resolved_attr.exclude_hv;
        attr.exclude_idle = resolved_attr.exclude_idle;
        attr.exclude_host = resolved_attr.exclude_host;
        attr.exclude_guest = resolved_attr.exclude_guest;
    }
} // namespace optkit::pmu::cpu::perf::riscv

#endif