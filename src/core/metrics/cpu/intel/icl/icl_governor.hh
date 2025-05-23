#pragma once

#include <vector>
#include <string>
#include "core/pmu/cpu/perf/events/intel/icl.hh"
#include "core/frequency/base_governor.hh"
#include "core/metrics/intel/icl/icl_recepies.hh"

namespace optkit::core::governors::intel::icl
{
    class Governor final : public core::frequency::BaseGovernor
    {
    public:
        Governor(bool data_collector_mode = false);
        virtual ~Governor() { BaseGovernor::current_governor = nullptr; }
        virtual void snapshot_pmus() override { this->pmu_record = interested_events.read(); }

        OPT_FORCE_INLINE virtual double compute_intensity() override
        {
            double flops_executed = pmu_record[1];
            double l3_misses = pmu_record[3];
            double compute_intensity = flops_executed / (l3_misses + 1);
            return compute_intensity;
        }

        OPT_FORCE_INLINE virtual double cache_intensity() override
        {
            double total_instructions = pmu_record[0];
            double cache_hits_misses = pmu_record[2];
            double cache_intensity = cache_hits_misses / total_instructions;
            return cache_intensity;
        }

        OPT_FORCE_INLINE virtual double dram_intensity() override
        {
            double total_instructions = pmu_record[0];
            double l3_misses = pmu_record[3];
            double dram_intensity = l3_misses / total_instructions;
            return dram_intensity;
        }

        OPT_FORCE_INLINE void disalbe_callback_trigger() override
        {
            core_cycle_watcher.disable();
            interested_events.disable();
        }
        OPT_FORCE_INLINE void enable_callback_trigger() override
        {
            core_cycle_watcher.enable();
            interested_events.enable();
        }

    private:
        // cycle watcher gets extended to other cores. we need to fix this to a single core only!!!
        optkit::core::pmu::BlockProfiler core_cycle_watcher;
        optkit::core::pmu::BlockGroupProfiler interested_events;
    };
}
