#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iomanip>
#include <x86intrin.h>
#include <memory>
#include "intel_priv.hh"
#include "utils/utils.hh" 
#include "core/profiling/pmu/pmu_event_manager.hh"
#include "core/freq/query_frequency.hh"
#include "core/event_recepies/tma_metrics.hh"
#include "core/profiling/pmu/block_group_profiler.hh"
#include "core/profiling/pmu/block_profiler.hh"

namespace optkit::core::recepies
{

    /**
     * @brief Implemented for set of events of most Intel architectures. To customize for your specific architecture, check the /src/core/events/intel/ directory and override any method as needed.
     *  Before implementation, please check the paper @link https://www.researchgate.net/publication/269302126_A_Top-Down_method_for_performance_analysis_and_counters_architecture?enrichId=rgreq-f4d54a2d8c4d31b1c1c4e8e30346bb95-XXX&enrichSource=Y292ZXJQYWdlOzI2OTMwMjEyNjtBUzo0MTc3MDM3NjEzMzQyNzNAMTQ3NjU5OTc0NTc1OQ%3D%3D&el=1_x_3&_esc=publicationCoverPdf @endlink
     */
    class TMAnalysis
    {

    public:
        TMAnalysis(const char *block_name, const char *event_name, bool verbose = true, const ProfilerConfig &config = ProfilerConfig{false, true, false, 0, -1});
        void begin_monitoring(L1Metric metric);
        void begin_monitoring(L2Metric metric);
        void begin_monitoring(L3Metric metric);
        virtual ~TMAnalysis();

    protected:
        using L1AnaliseFunc = std::map<L1Metric, double> (TMAnalysis::*)();
        using L2AnaliseFunc = std::map<L2Metric, double> (TMAnalysis::*)();
        using L3AnaliseFunc = std::map<L3Metric, double> (TMAnalysis::*)();

        L1AnaliseFunc analise_method_L1 = nullptr;
        L2AnaliseFunc analise_method_L2 = nullptr;
        L3AnaliseFunc analise_method_L3 = nullptr;

        virtual std::map<L1Metric, double> L1__analise();
        virtual std::map<L2Metric, double> L1__backend__analise();
        virtual std::map<L2Metric, double> L1__retiring__analise();
        virtual std::map<L2Metric, double> L1__bad_speculation__analise();
        virtual std::map<L2Metric, double> L1__frontend_bound__analise();

        virtual std::map<L1Metric, double> L2__analise();
        virtual std::map<L3Metric, double> L2__memory_bound__analise();
        virtual std::map<L3Metric, double> L2__core__analise();

        // virtual std::map<L3Metric, double> L2__core_bound__analise();
        // virtual std::map<L3Metric, double> L2__base__analise();
        // virtual std::map<L3Metric, double> L2__fetch_latency__analise();
        // virtual std::map<L3Metric, double> L2__fetch_bandwidth__analise();

        virtual std::vector<std::pair<uint64_t, std::string>> L1__default__recipie();
        virtual std::vector<std::pair<uint64_t, std::string>> L2__default__recipie();

        // these 2 returns l1 backend-bound
        virtual std::vector<std::pair<uint64_t, std::string>> L2__backend__memory();
        virtual std::vector<std::pair<uint64_t, std::string>> L2__backend__core();

        // following 2 returns l1 bad-specualtion
        virtual std::vector<std::pair<uint64_t, std::string>> L2__bad_speculation__branch_mispredict();
        virtual std::vector<std::pair<uint64_t, std::string>> L2__bad_speculation__machine_clears();

        // following 2 returns l1 retiring
        virtual std::vector<std::pair<uint64_t, std::string>> L2__retiring__base();
        virtual std::vector<std::pair<uint64_t, std::string>> L2__retiring__micro_sequencer();

        // following 2 returns l1 frontend-bound
        virtual std::vector<std::pair<uint64_t, std::string>> L2__frontend__fetch_latency();
        virtual std::vector<std::pair<uint64_t, std::string>> L2__frontend__fetch_bandwidth();

        // followign returns l2 memory bound
        virtual std::vector<std::pair<uint64_t, std::string>> L3__memory__ext_memory();
        virtual std::vector<std::pair<uint64_t, std::string>> L3__memory__l1();
        virtual std::vector<std::pair<uint64_t, std::string>> L3__memory__l2();
        virtual std::vector<std::pair<uint64_t, std::string>> L3__memory__l3();


        virtual std::vector<std::pair<uint64_t, std::string>> L3__core__divider();
        virtual std::vector<std::pair<uint64_t, std::string>> L3__core__exec_port_utils();

        virtual void choose_profiler() final;

    protected:
        uint64_t start_time;
        uint64_t delta_time;

        std::unique_ptr<core::BaseProfiler<std::vector<uint64_t>>> profiler_ref;
        std::vector<std::pair<uint64_t, std::string>> recipie_to_monitor;

    private:
        const char *block_name;
        const char *event_name;
        bool verbose;
        ProfilerConfig profiler_config;
    };

} // namespace optkit::core::recepies
