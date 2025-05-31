#pragma once

#include "core/pmu/cpu/msr/profiler_config.hh"
#include "utils/environment_config.hh"
#if OPTKIT_ENV_LIB_MSE_SAFE

#include <iostream>
#include <vector>
#include "utils/utils.hh"
#include "utils/base_profiler.hh"
#include "core/pmu/cpu/msr/profiler_config.hh"
#include "core/pmu/cpu/pmu_event_manager.hh"
#include "core/pmu/cpu/pmu_utils.hh"

namespace optkit::core::pmu::cpu::msr
{
    class BlockProfiler : public BaseProfiler<std::vector<uint64_t>>
    {
    public:
        BlockProfiler(const char *block_name, const char *event_name, const std::vector<std::pair<uint64_t, std::string>> &raw_events, bool verbose = true, const MSRProfilerConfig &config = MSRProfilerConfig{true, true, false, 0, -1});
        virtual ~BlockProfiler();
        /**
         * @brief Disables this block profiler and associated events
         *
         */
        virtual void disable() override;

        /**
         * @brief Enables this block profiler and associated events
         *
         */
        virtual void enable() override;

        /**
         * @brief converts buffer to json
         *
         */
        virtual std::string convert_buffer_to_json() override;

        /**
         * @brief Reads the values of all raw_events.
         *
         * @return std::vector<uint64_t> contains each raw_events' pmu data.
         */
        virtual std::vector<uint64_t> read() override;

    public:
        /**
         * @brief fd_list holds pmu events being monitor by this BlockProfiler Object.
         * when created the same file description must be registered global fd_stack
         *
         */
        std::vector<int32_t> fd_list;
        MSRProfilerConfig profiler_config;

    private:
        std::vector<std::pair<uint64_t, std::string>> raw_events;
    };

} // namespace optkit::core::pmu::cpu::msr

#endif