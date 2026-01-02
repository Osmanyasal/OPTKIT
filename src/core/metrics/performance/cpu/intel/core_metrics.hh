#pragma once
#include "utils/deployment/deployment_config.hh"
#if OPTKIT_ENV_CPU_INTEL

#include "utils/metric_builder.hh"
#include "core/metrics/performance/cpu/core_metrics.hh"
#include "core/metrics/performance/cpu/intel/event_mapper.hh"
#include "core/metrics/performance/cpu/intel/native_events.hh"
#include <vector>
// Intel (decode width)
#if OPTKIT_ENV_CPU_MICROARCH_P6 || OPTKIT_ENV_CPU_MICROARCH_WSM ||  \
    OPTKIT_ENV_CPU_MICROARCH_NHM || OPTKIT_ENV_CPU_MICROARCH_SNB || \
    OPTKIT_ENV_CPU_MICROARCH_IVB || OPTKIT_ENV_CPU_MICROARCH_HSW || \
    OPTKIT_ENV_CPU_MICROARCH_BDW || OPTKIT_ENV_CPU_MICROARCH_SKL || \
    OPTKIT_ENV_CPU_MICROARCH_KBL || OPTKIT_ENV_CPU_MICROARCH_CFL || \
    OPTKIT_ENV_CPU_MICROARCH_CML
#define SUPERSCALAR_WIDE 4 // 4-wide decode (P6 to Skylake)
#elif OPTKIT_ENV_CPU_MICROARCH_ICL || OPTKIT_ENV_CPU_MICROARCH_TGL || \
    OPTKIT_ENV_CPU_MICROARCH_RKL || OPTKIT_ENV_CPU_MICROARCH_ADL ||   \
    OPTKIT_ENV_CPU_MICROARCH_RPL || OPTKIT_ENV_CPU_MICROARCH_MTL ||   \
    OPTKIT_ENV_CPU_MICROARCH_SPR || OPTKIT_ENV_CPU_MICROARCH_EMR ||   \
    OPTKIT_ENV_CPU_MICROARCH_GRN
#define SUPERSCALAR_WIDE 6 // 6-wide decode (Ice Lake to Meteor Lake)
#else
#define SUPERSCALAR_WIDE 6 // Fallback (modern Intel default)
#endif

/**
 * @brief AMD CoreEvent implementation for Zen+ architecture.
 *
 * This implementation is based on performance events available on Intel SPR,SKL,ICL,HSW CPUs.
 * Event compatibility with other Intel architectures is not guaranteed.
 *
 * https://en.wikichip.org/wiki/intel/cpuid
 *
 * Perf imlementation:
 * https://github.com/torvalds/linux/tree/master/tools/perf/pmu-events/arch/x86
 */

// Warn: to use template initialisation for a certain type, they must be in the same namespace. so do NOT change it.
namespace optkit::metrics::performance
{
    /**
     * @class IntelMetricsImpl
     * @brief Interface for retrieving AMD CPU performance metrics. Each metric method returns the set of events required for its computation.
     *
     */
    class IntelMetricsImpl
    {
    };

    template <>
    class CoreMetrics<IntelMetricsImpl>
    {
    public:
        // Return all supported metric names that can be passed to get_metric().
        static const std::vector<std::string> &get_all_metrics()
        {
            static const std::vector<std::string> names = {
                // Common metrics
                "cpu_max_capacity_based_utilization",
                "l1_mpki",
                "l2_mpki",
                "l3_mpki",
                "l2_hit_ratio",
                "l3_hit_ratio",
                "branch_mispr_ratio",
                "itlb_mpki",
                "dtlb_mpki",
                "tlb_mpki",
                "ipc",
                "ilp",
                // removed: mlp (not implemented)
                // removed: load_miss_latency (not implemented)
                // removed: dram_bandwidth_gbs (not implemented)
                "ip_call",
                "ip_branch",
                "ip_mem_load",
                "ip_mem_store",
                "ip_mispredict",
                "ip_flop",
                "ip_avx_any_flop",
                "ip_arith",
                "ip_arith_scalar_sp",
                "ip_arith_scalar_dp",
                "ip_arith_avx128",
                "ip_arith_avx256",
                "ip_arith_avx512",
                "ip_arith_vector_any",
                // removed: scalarp_arith_vector (not implemented)
                "ip_swpf",
                "ai",
                "gflops",
                "carm",
                // Topdown
                "frontend_bound",
                "bad_speculation",
                "retiring",
                "backend_bound",
                // removed: smt_contention (not implemented)
                "frontend_bound_latency",
                "frontend_bound_bw",
                "bad_speculation_mispredicts",
                "bad_speculation_pipeline_restarts",
                "backend_bound_memory",
                "backend_bound_cpu",
                "retiring_fastpath",
                "retiring_microcode",

                // Aggregates
                "topdown_l1",
                "topdown_l2",
                "topdown_l2_fe",
                "topdown_l2_be",
                "topdown_l2_retiring",
                "topdown_l2_bad_spec",
                //"all_topdown",
                //"all_mpki",
                //"all_cache_hit_ratio",
                //"all_stlb_mpki",
                //"all_latency_and_parallelism",
                //"all_dram_bandwidth",
                //"all_ip_metrics",
                //"all_branch_metrics",
                //"all_metrics"
            };
            return names;
        }

        // Fetch a metric by its method name (e.g., "l2_mpki").
        // Returns a const reference to a static MetricBuilder.
        static const MetricBuilder<uint64_t> &get_metric(const std::string &metric_name)
        {

            if (metric_name == "cpu_max_capacity_based_utilization")
                return cpu_max_capacity_based_utilization();
            if (metric_name == "l1_hit_ratio")
                return l1_hit_ratio();
            if (metric_name == "l2_hit_ratio")
                return l2_hit_ratio();
            if (metric_name == "l3_hit_ratio")
                return l3_hit_ratio();
            if (metric_name == "l1_mpki")
                return l1_mpki();
            if (metric_name == "l2_mpki")
                return l2_mpki();
            if (metric_name == "l3_mpki")
                return l3_mpki();
            if (metric_name == "branch_mispr_ratio")
                return branch_mispr_ratio();
            if (metric_name == "itlb_mpki")
                return itlb_mpki();
            if (metric_name == "dtlb_mpki")
                return dtlb_mpki();
            if (metric_name == "tlb_mpki")
                return tlb_mpki();
            if (metric_name == "ipc")
                return ipc();
            if (metric_name == "ilp")
                return ilp();
            if (metric_name == "mlp")
                return mlp();
            if (metric_name == "load_miss_latency")
                return load_miss_latency();
            if (metric_name == "dram_bandwidth_gbs")
                return dram_bandwidth_gbs();
            if (metric_name == "ip_call")
                return ip_call();
            if (metric_name == "ip_branch")
                return ip_branch();
            if (metric_name == "ip_mem_load")
                return ip_mem_load();
            if (metric_name == "ip_mem_store")
                return ip_mem_store();
            if (metric_name == "ip_mispredict")
                return ip_mispredict();
            if (metric_name == "ip_flop")
                return ip_flop();
            if (metric_name == "gflops")
                return gflops();
            if (metric_name == "ai")
                return ai();
            if (metric_name == "ip_avx_any_flop")
                return ip_avx_any_flop();
            if (metric_name == "ip_arith")
                return ip_arith();
            if (metric_name == "ip_arith_scalar_sp")
                return ip_arith_scalar_sp();
            if (metric_name == "ip_arith_scalar_dp")
                return ip_arith_scalar_dp();
            if (metric_name == "ip_arith_avx128")
                return ip_arith_avx128();
            if (metric_name == "ip_arith_avx256")
                return ip_arith_avx256();
            if (metric_name == "ip_arith_avx512")
                return ip_arith_avx512();
            if (metric_name == "scalar_arithp_vector")
                return scalar_arithp_vector();
            if (metric_name == "ip_arith_vector_any")
                return ip_arith_vector_any();
            if (metric_name == "scalarp_arith_vector")
                return scalarp_arith_vector();
            if (metric_name == "ip_swpf")
                return ip_swpf();

            // Topdown
            if (metric_name == "frontend_bound")
                return frontend_bound();
            if (metric_name == "bad_speculation")
                return bad_speculation();
            if (metric_name == "retiring")
                return retiring();
            if (metric_name == "backend_bound")
                return backend_bound();
            if (metric_name == "smt_contention")
                return smt_contention();

            if (metric_name == "frontend_bound_latency")
                return frontend_bound_latency();
            if (metric_name == "frontend_bound_bw")
                return frontend_bound_bw();
            if (metric_name == "bad_speculation_mispredicts")
                return bad_speculation_mispredicts();
            if (metric_name == "bad_speculation_pipeline_restarts")
                return bad_speculation_pipeline_restarts();
            if (metric_name == "backend_bound_memory")
                return backend_bound_memory();
            if (metric_name == "backend_bound_cpu")
                return backend_bound_cpu();
            if (metric_name == "retiring_fastpath")
                return retiring_fastpath();
            if (metric_name == "retiring_microcode")
                return retiring_microcode();

            // Aggregates / Roofline
            if (metric_name == "carm")
                return carm();
            if (metric_name == "topdown_l1")
                return topdown_l1();
            if (metric_name == "topdown_l2")
                return topdown_l2();
            if (metric_name == "topdown_l2_fe")
                return topdown_l2_fe();
            if (metric_name == "topdown_l2_be")
                return topdown_l2_be();
            if (metric_name == "topdown_l2_retiring")
                return topdown_l2_retiring();
            if (metric_name == "topdown_l2_bad_spec")
                return topdown_l2_bad_spec();
            if (metric_name == "all_topdown")
                return all_topdown();
            if (metric_name == "all_mpki")
                return all_mpki();
            if (metric_name == "all_cache_hit_ratio")
                return all_cache_hit_ratio();
            if (metric_name == "all_stlb_mpki")
                return all_stlb_mpki();
            if (metric_name == "all_latency_and_parallelism")
                return all_latency_and_parallelism();
            if (metric_name == "all_dram_bandwidth")
                return all_dram_bandwidth();
            if (metric_name == "all_ip_metrics")
                return all_ip_metrics();
            if (metric_name == "all_branch_metrics")
                return all_branch_metrics();
            if (metric_name == "all_metrics")
                return all_metrics();

            OPTKIT_CORE_WARN("Requested unknown AMD metric: {}", metric_name);
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // CPU Utilization
        static const MetricBuilder<uint64_t> &cpu_max_capacity_based_utilization()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                return MetricBuilder<uint64_t>{}
                    .add(unhalted_core_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("cpu_max_capacity_based_utilization__%",
                           [unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               static const double max_cycles = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * frequency::cpu::Query::get_cpuinfo_max_freq() * 1000; // KHz to Hz
                               uint64_t unhalted_core_cycles = get_event_count(counts, unhalted_core_cycles_name);
                               double duration_sec = get_event_count(counts, "duration_microsec") / 1.0e6;

                               // Avoid div by zero
                               if (duration_sec == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100.0 * static_cast<double>(unhalted_core_cycles) / (max_cycles * duration_sec);
                           });
            }();
            return metric;
        } ///< 100 * (UNHALTED_CLK_CYCLES / (OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS * max_freq_khz * 1000  * duration_sec)))

        // Native Metric implementations (not included in CoreMetrics)
        static const MetricBuilder<uint64_t> &l1_hit_ratio()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
                std::string l1_hits_name = to_string(CoreEvents::L1_HITS);
                return MetricBuilder<uint64_t>{}
                    .add(l1_misses_name, intel::EventMapper::get(CoreEvents::L1_MISSES))
                    .add(l1_hits_name, intel::EventMapper::get(CoreEvents::L1_HITS))
                    .build("l1_hit_ratio__%",
                           [l1_hits_name, l1_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l1_hits = get_event_count(counts, l1_hits_name);
                               uint64_t l1_misses = get_event_count(counts, l1_misses_name);
                               uint64_t l1_cache_accesses = l1_hits + l1_misses;
                               // Avoid div by zero
                               if (l1_cache_accesses == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(l1_hits) / static_cast<double>(l1_cache_accesses));
                           });
            }();
            return metric;
        }
        // Native Metric implementations (not included in CoreMetrics)
        static const MetricBuilder<uint64_t> &l2_hit_ratio()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string l2_hits_name = to_string(CoreEvents::L2_HITS);
                return MetricBuilder<uint64_t>{}
                    .add(l2_misses_name, intel::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(l2_hits_name, intel::EventMapper::get(CoreEvents::L2_HITS))
                    .build("l2_hit_ratio__%",
                           [l2_hits_name, l2_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l2_hits = get_event_count(counts, l2_hits_name);
                               uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                               uint64_t l2_cache_accesses = l2_hits + l2_misses;
                               // Avoid div by zero
                               if (l2_cache_accesses == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(l2_hits) / static_cast<double>(l2_cache_accesses));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_hit_ratio()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
#if OPTKIT_ENV_CPU_MICROARCH_KNL
                std::string l3_hits_name = to_string(intel::NativeEvents::L3_DEMAND_REFERENCES);
#else
                std::string l3_hits_name = to_string(CoreEvents::L3_HITS);
#endif
                return MetricBuilder<uint64_t>{}
                    .add(l3_misses_name, intel::EventMapper::get(CoreEvents::L3_MISSES))
#if OPTKIT_ENV_CPU_MICROARCH_KNL
                    .add(l3_hits_name, intel::EventMapper::get(intel::NativeEvents::L3_DEMAND_REFERENCES))
#else
                    .add(l3_hits_name, intel::EventMapper::get(CoreEvents::L3_HITS))
#endif
                    .build("l3_hit_ratio__%",
                           [l3_hits_name, l3_misses_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l3_hits = get_event_count(counts, l3_hits_name);
                               uint64_t l3_misses = get_event_count(counts, l3_misses_name);
#if OPTKIT_ENV_CPU_MICROARCH_KNL
                               l3_hits = l3_hits - l3_misses; // L3_DEMANDS - L3_MISSES
#endif

                               uint64_t l3_cache_accesses = l3_hits + l3_misses;
                               if (l3_cache_accesses == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100.0 * static_cast<double>(l3_hits) / static_cast<double>(l3_cache_accesses);
                           });
            }();
            return metric;
        }

    public:
        // CoreMetrics Implementation

        // Cache miss per kilo instruction (MPKI)
        static const MetricBuilder<uint64_t> &l1_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l1_misses_name = to_string(CoreEvents::L1_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(l1_misses_name, intel::EventMapper::get(CoreEvents::L1_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l1_mpki",
                           [l1_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l1_misses = get_event_count(counts, l1_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l1_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l2_mpki()
        {
            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l2_misses_name = to_string(CoreEvents::L2_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(l2_misses_name, intel::EventMapper::get(CoreEvents::L2_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l2_mpki",
                           [l2_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l2_misses = get_event_count(counts, l2_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l2_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &l3_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string l3_misses_name = to_string(CoreEvents::L3_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(l3_misses_name, intel::EventMapper::get(CoreEvents::L3_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("l3_mpki",
                           [l3_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t l3_misses = get_event_count(counts, l3_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(l3_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // Branch
        static const MetricBuilder<uint64_t> &branch_mispr_ratio()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(branch_inst_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("branch_mispr_ratio",
                           [branch_misp_retired_name, branch_inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t branch_misp = get_event_count(counts, branch_misp_retired_name);
                               uint64_t branch_inst = get_event_count(counts, branch_inst_retired_name);

                               // Avoid div by zero
                               if (branch_inst == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(branch_misp) / static_cast<double>(branch_inst);
                           });
            }();
            return metric;
        }

        // ITLB MPKI metrics
        static const MetricBuilder<uint64_t> &itlb_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(itlb_misses_name, intel::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("itlb_mpki",
                           [itlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // DTLB MPKI metrics
        static const MetricBuilder<uint64_t> &dtlb_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(dtlb_misses_name, intel::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("dtlb_mpki",
                           [dtlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, dtlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * static_cast<double>(itlb_misses) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // TLB MPKI metrics
        static const MetricBuilder<uint64_t> &tlb_mpki()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dtlb_misses_name = to_string(CoreEvents::DTLB_MISSES);
                std::string itlb_misses_name = to_string(CoreEvents::ITLB_MISSES);
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);

                return MetricBuilder<uint64_t>{}
                    .add(itlb_misses_name, intel::EventMapper::get(CoreEvents::DTLB_MISSES))
                    .add(dtlb_misses_name, intel::EventMapper::get(CoreEvents::ITLB_MISSES))
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .build("tlb_mpki",
                           [itlb_misses_name, dtlb_misses_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t itlb_misses = get_event_count(counts, itlb_misses_name);
                               uint64_t dtlb_misses = get_event_count(counts, dtlb_misses_name);
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);

                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 1000.0 * (static_cast<double>(dtlb_misses) + static_cast<double>(itlb_misses)) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        // Latency and parallelism metrics
        static const MetricBuilder<uint64_t> &load_miss_latency()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &ipc()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string unhalted_core_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(unhalted_core_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .build("ipc", [inst_retired_name, unhalted_core_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                           uint64_t inst_retired = get_event_count(counts,inst_retired_name);
                           uint64_t unhalted_core_cycles = get_event_count(counts,unhalted_core_cycles_name);

                           if (unhalted_core_cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                            return static_cast<double>(inst_retired) / static_cast<double>(unhalted_core_cycles); });
            }();
            return metric;
        }

        // #if !OPTKIT_ENV_CPU_MICROARCH_KNL // NOT KNL
        static const MetricBuilder<uint64_t> &ilp()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string uops_cycles_thread_name = to_string(intel::NativeEvents::UOPS_CORE_CYCLES_THREAD);
                std::string core_cycles_ge_1_name = to_string(intel::NativeEvents::UOPS_CORE_CYCLES_GE_1);
                return MetricBuilder<uint64_t>{}
                    .add(uops_cycles_thread_name, intel::EventMapper::get(intel::NativeEvents::UOPS_CORE_CYCLES_THREAD))
                    .add(core_cycles_ge_1_name, intel::EventMapper::get(intel::NativeEvents::UOPS_CORE_CYCLES_GE_1))
                    .build("ILP__%",
                           [uops_cycles_thread_name, core_cycles_ge_1_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t uops_cycles_thread = get_event_count(counts, uops_cycles_thread_name);
                               uint64_t core_cycles_ge_1 = get_event_count(counts, core_cycles_ge_1_name);

                               // Avoid div by zero
                               if (core_cycles_ge_1 == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(uops_cycles_thread)) / ((Query::is_smt_enabled() ? 2 : 1) * static_cast<double>(core_cycles_ge_1));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &mlp()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // DRAM bandwidth
        static const MetricBuilder<uint64_t> &dram_bandwidth_gbs()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Instruction per event
        static const MetricBuilder<uint64_t> &ip_call()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_retired_near_call_name = to_string(intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_retired_near_call_name, intel::EventMapper::get(intel::NativeEvents::BR_INST_RETIRED_NEAR_CALL))
                    .build("ip_call",
                           [inst_retired_name, inst_retired_near_call_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_retired_near_call = get_event_count(counts, inst_retired_near_call_name);

                               // Avoid div by zero
                               if (inst_retired_near_call == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_retired_near_call);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_branch()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_inst_retired_name = to_string(CoreEvents::BRANCH_INST_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_inst_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_INST_RETIRED))
                    .build("ip_branch",
                           [branch_inst_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t branch_inst_retired = get_event_count(counts, branch_inst_retired_name);

                               // Avoid div by zero
                               if (branch_inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(branch_inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mem_load()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_load_retired_name = to_string(CoreEvents::MEM_LOAD_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_load_retired_name, intel::EventMapper::get(CoreEvents::MEM_LOAD_RETIRED))
                    .build("IpLoad",
                           [mem_load_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t mem_load_retired = get_event_count(counts, mem_load_retired_name);

                               // Avoid div by zero
                               if (mem_load_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(mem_load_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mem_store()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string mem_store_retired_name = to_string(CoreEvents::MEM_STORE_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(mem_store_retired_name, intel::EventMapper::get(CoreEvents::MEM_STORE_RETIRED))
                    .build("IpStore",
                           [mem_store_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t mem_store_retired = get_event_count(counts, mem_store_retired_name);

                               // Avoid div by zero
                               if (mem_store_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(mem_store_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_mispredict()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .build("ip_mispredict",
                           [branch_misp_retired_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);

                               // Avoid div by zero
                               if (branch_misp_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(branch_misp_retired);
                           });
            }();
            return metric;
        }

        // Floating-point operation metrics
        static const MetricBuilder<uint64_t> &ip_flop()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string retired_scalar_sp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
                std::string retired_scalar_dp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(retired_scalar_sp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                    .add(retired_scalar_dp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("ip_flop",
                           [inst_retired_name, retired_scalar_sp_any_name, retired_scalar_dp_any_name,
                            inst_packed_128_double_name, inst_packed_128_single_name,
                            inst_packed_256_double_name, inst_packed_256_single_name,
                            inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t retired_scalar_sp_any = get_event_count(counts, retired_scalar_sp_any_name);
                               uint64_t retired_scalar_dp_any = get_event_count(counts, retired_scalar_dp_any_name);
                               uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                               uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                               uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                               uint64_t total_flops =
                                   (retired_scalar_sp_any * 1) +
                                   (retired_scalar_dp_any * 1) +
                                   (inst_packed_128_double * 2) +
                                   (inst_packed_128_single * 4) +
                                   (inst_packed_256_double * 4) +
                                   (inst_packed_256_single * 8) +
                                   (inst_packed_512_double * 8) +
                                   (inst_packed_512_single * 16);

                               // Avoid div by zero
                               if (total_flops == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &gflops()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string retired_scalar_sp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
                std::string retired_scalar_dp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

                return MetricBuilder<uint64_t>{}
                    .add(retired_scalar_sp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                    .add(retired_scalar_dp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("gflops",
                           [retired_scalar_sp_any_name, retired_scalar_dp_any_name,
                            inst_packed_128_double_name, inst_packed_128_single_name,
                            inst_packed_256_double_name, inst_packed_256_single_name,
                            inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               double duration_sec = get_event_count(counts, "duration_microsec") / 1.0e6;
                               uint64_t retired_scalar_sp_any = get_event_count(counts, retired_scalar_sp_any_name);
                               uint64_t retired_scalar_dp_any = get_event_count(counts, retired_scalar_dp_any_name);
                               uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                               uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                               uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                               uint64_t total_flops =
                                   (retired_scalar_sp_any * 1) +
                                   (retired_scalar_dp_any * 1) +
                                   (inst_packed_128_double * 2) +
                                   (inst_packed_128_single * 4) +
                                   (inst_packed_256_double * 4) +
                                   (inst_packed_256_single * 8) +
                                   (inst_packed_512_double * 8) +
                                   (inst_packed_512_single * 16);

                               // Avoid div by zero
                               if (duration_sec == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(total_flops) / duration_sec / 1.0e9;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ai()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string mem_inst_retired_name = to_string(CoreEvents::MEM_INST_RETIRED);
                std::string retired_scalar_sp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
                std::string retired_scalar_dp_any_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

                return MetricBuilder<uint64_t>{}
                    .add(mem_inst_retired_name, intel::EventMapper::get(CoreEvents::MEM_INST_RETIRED))
                    .add(retired_scalar_sp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                    .add(retired_scalar_dp_any_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("ai",
                           [mem_inst_retired_name, retired_scalar_sp_any_name, retired_scalar_dp_any_name,
                            inst_packed_128_double_name, inst_packed_128_single_name,
                            inst_packed_256_double_name, inst_packed_256_single_name,
                            inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t mem_inst_retired = get_event_count(counts, mem_inst_retired_name);
                               uint64_t retired_scalar_sp_any = 1 * get_event_count(counts, retired_scalar_sp_any_name);
                               uint64_t retired_scalar_dp_any = 1 * get_event_count(counts, retired_scalar_dp_any_name);
                               uint64_t inst_packed_128_double = 2 * get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = 4 * get_event_count(counts, inst_packed_128_single_name);
                               uint64_t inst_packed_256_double = 4 * get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = 8 * get_event_count(counts, inst_packed_256_single_name);
                               uint64_t inst_packed_512_double = 8 * get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = 16 * get_event_count(counts, inst_packed_512_single_name);

                               uint64_t total_flops =
                                   retired_scalar_sp_any +
                                   retired_scalar_dp_any +
                                   inst_packed_128_double +
                                   inst_packed_128_single +
                                   inst_packed_256_double +
                                   inst_packed_256_single +
                                   inst_packed_512_double +
                                   inst_packed_512_single;
                               double sp_ratio = (retired_scalar_sp_any + inst_packed_128_single + inst_packed_256_single + inst_packed_512_single) / static_cast<double>(total_flops);
                               double dp_ratio = (retired_scalar_dp_any + inst_packed_128_double + inst_packed_256_double + inst_packed_512_double) / static_cast<double>(total_flops);
                               double mem_bytes = mem_inst_retired * (4 * sp_ratio + 8 * dp_ratio);
                               //    std::cout << "total_flops:" << total_flops << "\n";
                               //    std::cout << "mem bytes:" << mem_bytes << "\n";
                               // Avoid div by zero
                               if (mem_bytes == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(total_flops) / mem_bytes;
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_avx_any_flop()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("ip_avx_any_flop",
                           [inst_retired_name,
                            inst_packed_128_double_name, inst_packed_128_single_name,
                            inst_packed_256_double_name, inst_packed_256_single_name,
                            inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                               uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                               uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                               uint64_t total_flops =
                                   (inst_packed_128_double * 2) +
                                   (inst_packed_128_single * 4) +
                                   (inst_packed_256_double * 4) +
                                   (inst_packed_256_single * 8) +
                                   (inst_packed_512_double * 8) +
                                   (inst_packed_512_single * 16);

                               // Avoid div by zero
                               if (total_flops == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(total_flops);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_retired_scalar_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR);
                std::string inst_retired_vector_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_retired_scalar_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR))
                    .add(inst_retired_vector_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR))
                    .build("ip_arith",
                           [inst_retired_name, inst_retired_scalar_name, inst_retired_vector_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_retired_scalar = get_event_count(counts, inst_retired_scalar_name);
                               uint64_t inst_retired_vector = get_event_count(counts, inst_retired_vector_name);
                               uint64_t arith = inst_retired / (inst_retired_scalar + inst_retired_vector);
                               // Avoid div by zero
                               if (inst_retired == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_retired);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_scalar_sp()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_scalar_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_scalar_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_SINGLE))
                    .build("ip_arith_scalar_sp",
                           [inst_scalar_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_scalar_single = get_event_count(counts, inst_scalar_single_name);

                               // Avoid div by zero
                               if (inst_scalar_single == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_single);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_scalar_dp()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_scalar_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_scalar_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR_DOUBLE))
                    .build("ip_arith_scalar_dp",
                           [inst_scalar_double_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_scalar_double = get_event_count(counts, inst_scalar_double_name);

                               // Avoid div by zero
                               if (inst_scalar_double == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_scalar_double);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx128()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .build("ip_arith_avx128",
                           [inst_packed_128_double_name, inst_packed_128_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);

                               uint64_t inst_packed_128 = inst_packed_128_double + inst_packed_128_single;
                               // Avoid div by zero
                               if (inst_packed_128 == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_128);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx256()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .build("ip_arith_avx256",
                           [inst_packed_256_double_name, inst_packed_256_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);

                               uint64_t inst_packed_256 = inst_packed_256_double + inst_packed_256_single;
                               // Avoid div by zero
                               if (inst_packed_256 == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_256);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_avx512()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("ip_arith_avx512",
                           [inst_packed_512_double_name, inst_packed_512_single_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                               uint64_t inst_packed_512 = inst_packed_512_double + inst_packed_512_single;
                               // Avoid div by zero
                               if (inst_packed_512 == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(inst_packed_512);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &scalar_arithp_vector()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_vector_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR);
                std::string inst_scalar_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR);
                return MetricBuilder<uint64_t>{}
                    .add(inst_vector_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_VECTOR))
                    .add(inst_scalar_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_SCALAR))
                    .build("scalar_arithp_vector",
                           [inst_vector_name, inst_scalar_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_vector = get_event_count(counts, inst_vector_name);
                               uint64_t inst_scalar = get_event_count(counts, inst_scalar_name);
                               // Avoid div by zero
                               if (inst_vector == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_scalar) / static_cast<double>(inst_vector);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &ip_arith_vector_any()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string inst_packed_128_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE);
                std::string inst_packed_128_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE);
                std::string inst_packed_256_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE);
                std::string inst_packed_256_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE);
                std::string inst_packed_512_double_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE);
                std::string inst_packed_512_single_name = to_string(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE);

                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(inst_packed_128_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE))
                    .add(inst_packed_128_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_128B_PACKED_SINGLE))
                    .add(inst_packed_256_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_DOUBLE))
                    .add(inst_packed_256_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_256B_PACKED_SINGLE))
                    .add(inst_packed_512_double_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_DOUBLE))
                    .add(inst_packed_512_single_name, intel::EventMapper::get(intel::NativeEvents::FP_ARITH_INST_RETIRED_512B_PACKED_SINGLE))
                    .build("IpArithAVXAny",
                           [inst_retired_name,
                            inst_packed_128_double_name, inst_packed_128_single_name,
                            inst_packed_256_double_name, inst_packed_256_single_name,
                            inst_packed_512_double_name, inst_packed_512_single_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t inst_packed_128_double = get_event_count(counts, inst_packed_128_double_name);
                               uint64_t inst_packed_128_single = get_event_count(counts, inst_packed_128_single_name);
                               uint64_t inst_packed_256_double = get_event_count(counts, inst_packed_256_double_name);
                               uint64_t inst_packed_256_single = get_event_count(counts, inst_packed_256_single_name);
                               uint64_t inst_packed_512_double = get_event_count(counts, inst_packed_512_double_name);
                               uint64_t inst_packed_512_single = get_event_count(counts, inst_packed_512_single_name);

                               uint64_t total_avx_instr =
                                   (inst_packed_128_double) +
                                   (inst_packed_128_single) +
                                   (inst_packed_256_double) +
                                   (inst_packed_256_single) +
                                   (inst_packed_512_double) +
                                   (inst_packed_512_single);

                               // Avoid div by zero
                               if (total_avx_instr == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(total_avx_instr);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &scalarp_arith_vector()
        {
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        // Software prefetch
        static const MetricBuilder<uint64_t> &ip_swpf()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string inst_retired_name = to_string(CoreEvents::INST_RETIRED);
                std::string sw_load_prefetch_name = to_string(CoreEvents::SW_LOAD_PREFETCH_ACCESS);
                return MetricBuilder<uint64_t>{}
                    .add(inst_retired_name, intel::EventMapper::get(CoreEvents::INST_RETIRED))
                    .add(sw_load_prefetch_name, intel::EventMapper::get(CoreEvents::SW_LOAD_PREFETCH_ACCESS))
                    .build("ip_swpf",
                           [sw_load_prefetch_name, inst_retired_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t inst_retired = get_event_count(counts, inst_retired_name);
                               uint64_t sw_load_prefetch = get_event_count(counts, sw_load_prefetch_name);

                               // Avoid div by zero
                               if (sw_load_prefetch == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return static_cast<double>(inst_retired) / static_cast<double>(sw_load_prefetch);
                           });
            }();
            return metric;
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &frontend_bound()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(no_ops_from_frontend_name, intel::EventMapper::get(performance::intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                    .build("frontend_bound__%",
                           [dispatch_slots_name, no_ops_from_frontend_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &bad_speculation()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .add(uops_retired_slots_name, intel::EventMapper::get(performance::intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                    .build("bad_speculation__%",
                           [dispatch_slots_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(recovery_cycles)) / (static_cast<double>(dispatch_slots));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &retiring()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .build("Retiring__%",
                           [dispatch_slots_name, uops_retired_slots_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &
        smt_contention()
        {
            // std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
            // std::string smt_stalls_name = to_string(intel::NativeEvents::SMT_STALLS_1);
            // return MetricBuilder<uint64_t>{}
            //     .add(smt_stalls_name, intel::EventMapper::get(performance::intel::NativeEvents::SMT_STALLS_1))
            //     .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
            //     .build("smt_contention",
            //            [dispatch_slots_name, smt_stalls_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
            //            {
            //                uint64_t smt_stalls = get_event_count(counts,smt_stalls_name);
            //                uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts,dispatch_slots_name);

            //                // Avoid div by zero
            //                if (dispatch_slots == 0)
            //                    return std::numeric_limits<double>::quiet_NaN();
            //                return 100 * static_cast<double>(smt_stalls) / (static_cast<double>(dispatch_slots));
            //            });
            static const MetricBuilder<uint64_t> empty{};
            return empty;
        }

        static const MetricBuilder<uint64_t> &backend_bound()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(no_ops_from_frontend_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                    .build("backend_bound__%",
                           [dispatch_slots_name, no_ops_from_frontend_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                               double retiring = static_cast<double>(uops_retired_slots) / (static_cast<double>(dispatch_slots));
                               double frontend_bound = static_cast<double>(no_ops_from_frontend) / (static_cast<double>(dispatch_slots));
                               double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(recovery_cycles)) / (static_cast<double>(dispatch_slots));
                               // Avoid div by zero
                               return 100 * (1 - (frontend_bound + bad_speculation + retiring));
                           });
            }();
            return metric;
        }
        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &frontend_bound_latency()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string clocks_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string no_ops_from_frontend_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE);
                std::string uops_not_delivered_cycles_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0);

                return MetricBuilder<uint64_t>{}
                    .add(clocks_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(no_ops_from_frontend_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CORE))
                    .add(uops_not_delivered_cycles_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0))
                    .build("frontend_bound_latency__%",
                           [clocks_name, no_ops_from_frontend_name, uops_not_delivered_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t clocks = get_event_count(counts, clocks_name);
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * clocks;
                               uint64_t no_ops_from_frontend = get_event_count(counts, no_ops_from_frontend_name);
                               uint64_t uops_not_delivered_cycles = get_event_count(counts, uops_not_delivered_cycles_name);

                               double frontend_bw = (static_cast<double>(uops_not_delivered_cycles) / static_cast<double>(clocks));
                               // Avoid div by zero
                               if (dispatch_slots == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(no_ops_from_frontend) / static_cast<double>(dispatch_slots) - frontend_bw);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &frontend_bound_bw()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string clocks_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string uops_not_delivered_cycles_name = to_string(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0);

                return MetricBuilder<uint64_t>{}
                    .add(clocks_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(uops_not_delivered_cycles_name, intel::EventMapper::get(intel::NativeEvents::IDQ_UOPS_NOT_DELIVERED_CYCLES_0))
                    .build("frontend_bound_bw__%",
                           [clocks_name, uops_not_delivered_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t clocks = get_event_count(counts, clocks_name);
                               uint64_t uops_not_delivered_cycles = get_event_count(counts, uops_not_delivered_cycles_name);
                               // Avoid div by zero
                               if (clocks == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * (static_cast<double>(uops_not_delivered_cycles) / static_cast<double>(clocks));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &bad_speculation_mispredicts()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string machine_clears_count_name = to_string(intel::NativeEvents::MACHINE_CLEARS_COUNT);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(machine_clears_count_name, intel::EventMapper::get(intel::NativeEvents::MACHINE_CLEARS_COUNT))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                    .build("bad_speculation_mispredicts__%",
                           [dispatch_slots_name, branch_misp_retired_name, machine_clears_count_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                               uint64_t machine_clears_count = get_event_count(counts, machine_clears_count_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || (branch_misp_retired + machine_clears_count) == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));

                               return 100 * bad_speculation * (static_cast<double>(branch_misp_retired / (branch_misp_retired + machine_clears_count)));
                           });
            }();
            return metric;
        }
        static const MetricBuilder<uint64_t> &bad_speculation_pipeline_restarts()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string branch_misp_retired_name = to_string(CoreEvents::BRANCH_MISP_RETIRED);
                std::string machine_clears_count_name = to_string(intel::NativeEvents::MACHINE_CLEARS_COUNT);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string recovery_cycles_name = to_string(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(branch_misp_retired_name, intel::EventMapper::get(CoreEvents::BRANCH_MISP_RETIRED))
                    .add(machine_clears_count_name, intel::EventMapper::get(intel::NativeEvents::MACHINE_CLEARS_COUNT))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(recovery_cycles_name, intel::EventMapper::get(intel::NativeEvents::INT_MISC_RECOVERY_CYCLES))
                    .build("bad_speculation_pipeline_restarts__%",
                           [dispatch_slots_name, branch_misp_retired_name, machine_clears_count_name, uops_issued_name, uops_retired_slots_name, recovery_cycles_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t branch_misp_retired = get_event_count(counts, branch_misp_retired_name);
                               uint64_t machine_clears_count = get_event_count(counts, machine_clears_count_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t recovery_cycles = get_event_count(counts, recovery_cycles_name);

                               // Avoid div by zero
                               if (dispatch_slots == 0 || (branch_misp_retired + machine_clears_count) == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double bad_speculation = (static_cast<double>(uops_issued) - static_cast<double>(uops_retired_slots) + SUPERSCALAR_WIDE * static_cast<double>(uops_retired_slots)) / (static_cast<double>(dispatch_slots));

                               return 100 * bad_speculation - (bad_speculation * (static_cast<double>(branch_misp_retired / (branch_misp_retired + machine_clears_count))));
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &backend_bound_memory()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string stalls_l1d_miss_name = to_string(intel::NativeEvents::STALLS_L1D_MISS);
                std::string stalls_l2_miss_name = to_string(intel::NativeEvents::STALLS_L2_MISS);
                std::string stalls_l3_miss_name = to_string(intel::NativeEvents::STALLS_L3_MISS);
                std::string resource_stalls_sb_name = to_string(intel::NativeEvents::RESOURCE_STALLS_SB);

                return MetricBuilder<uint64_t>{}
                    .add(unhalted_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(stalls_l1d_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L1D_MISS))
                    .add(stalls_l2_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L2_MISS))
                    .add(stalls_l3_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L3_MISS))
                    .add(resource_stalls_sb_name, intel::EventMapper::get(intel::NativeEvents::RESOURCE_STALLS_SB))
                    .build("backend_bound_memory__%",
                           [unhalted_cycles_name, stalls_l1d_miss_name, stalls_l2_miss_name, stalls_l3_miss_name, resource_stalls_sb_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t unhalted_cycles = get_event_count(counts, unhalted_cycles_name);
                               uint64_t stalls_l1d_miss = get_event_count(counts, stalls_l1d_miss_name);
                               uint64_t stalls_l2_miss = get_event_count(counts, stalls_l2_miss_name);
                               uint64_t stalls_l3_miss = get_event_count(counts, stalls_l3_miss_name);
                               uint64_t resource_stalls_sb = get_event_count(counts, resource_stalls_sb_name);
                               // Avoid div by zero
                               if (unhalted_cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * ((static_cast<double>(stalls_l1d_miss) +
                                              static_cast<double>(stalls_l2_miss) +
                                              static_cast<double>(stalls_l3_miss) -
                                              static_cast<double>(resource_stalls_sb)) /
                                             unhalted_cycles);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &backend_bound_cpu()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string unhalted_cycles_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string stalls_l1d_miss_name = to_string(intel::NativeEvents::STALLS_L1D_MISS);
                std::string stalls_l2_miss_name = to_string(intel::NativeEvents::STALLS_L2_MISS);
                std::string stalls_l3_miss_name = to_string(intel::NativeEvents::STALLS_L3_MISS);
                std::string resource_stalls_sb_name = to_string(intel::NativeEvents::RESOURCE_STALLS_SB);

                return MetricBuilder<uint64_t>{}
                    .add(unhalted_cycles_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(stalls_l1d_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L1D_MISS))
                    .add(stalls_l2_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L2_MISS))
                    .add(stalls_l3_miss_name, intel::EventMapper::get(intel::NativeEvents::STALLS_L3_MISS))
                    .add(resource_stalls_sb_name, intel::EventMapper::get(intel::NativeEvents::RESOURCE_STALLS_SB))
                    .build("backend_bound_cpu__%",
                           [unhalted_cycles_name, stalls_l1d_miss_name, stalls_l2_miss_name, stalls_l3_miss_name, resource_stalls_sb_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t unhalted_cycles = get_event_count(counts, unhalted_cycles_name);
                               uint64_t stalls_l1d_miss = get_event_count(counts, stalls_l1d_miss_name);
                               uint64_t stalls_l2_miss = get_event_count(counts, stalls_l2_miss_name);
                               uint64_t stalls_l3_miss = get_event_count(counts, stalls_l3_miss_name);
                               uint64_t resource_stalls_sb = get_event_count(counts, resource_stalls_sb_name);
                               // Avoid div by zero
                               if (unhalted_cycles == 0)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double backend_memory_bound = ((static_cast<double>(stalls_l1d_miss) +
                                                               static_cast<double>(stalls_l2_miss) +
                                                               static_cast<double>(stalls_l3_miss) -
                                                               static_cast<double>(resource_stalls_sb)) /
                                                              unhalted_cycles);
                               return 100.0 * (1 - backend_memory_bound);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &retiring_fastpath()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string idq_ms_uops_name = to_string(intel::NativeEvents::IDQ_MS_UOPS);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(idq_ms_uops_name, intel::EventMapper::get(intel::NativeEvents::IDQ_MS_UOPS))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .build("retiring_fastpath__%",
                           [dispatch_slots_name, uops_retired_slots_name, idq_ms_uops_name, uops_issued_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t idq_ms_uops = get_event_count(counts, idq_ms_uops_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);

                               // Avoid div by zero
                               if (dispatch_slots)
                                   return std::numeric_limits<double>::quiet_NaN();
                               double retiring_microcode = ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) * (static_cast<double>(idq_ms_uops) / static_cast<double>(dispatch_slots)));
                               return 100 * ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) - retiring_microcode);
                           });
            }();
            return metric;
        }

        static const MetricBuilder<uint64_t> &retiring_microcode()
        {

            static const MetricBuilder<uint64_t> metric = []
            {
                std::string dispatch_slots_name = to_string(CoreEvents::UNHALTED_CORE_CYCLES);
                std::string uops_retired_slots_name = to_string(intel::NativeEvents::UOPS_RETIRED_SLOTS);
                std::string idq_ms_uops_name = to_string(intel::NativeEvents::IDQ_MS_UOPS);
                std::string uops_issued_name = to_string(intel::NativeEvents::UOPS_ISSUED);

                return MetricBuilder<uint64_t>{}
                    .add(dispatch_slots_name, intel::EventMapper::get(CoreEvents::UNHALTED_CORE_CYCLES))
                    .add(uops_retired_slots_name, intel::EventMapper::get(intel::NativeEvents::UOPS_RETIRED_SLOTS))
                    .add(idq_ms_uops_name, intel::EventMapper::get(intel::NativeEvents::IDQ_MS_UOPS))
                    .add(uops_issued_name, intel::EventMapper::get(intel::NativeEvents::UOPS_ISSUED))
                    .build("retiring_microcode__%",
                           [dispatch_slots_name, uops_retired_slots_name, idq_ms_uops_name, uops_issued_name](const std::unordered_map<std::string, uint64_t> &counts) -> double
                           {
                               uint64_t dispatch_slots = SUPERSCALAR_WIDE * get_event_count(counts, dispatch_slots_name);
                               uint64_t uops_retired_slots = get_event_count(counts, uops_retired_slots_name);
                               uint64_t idq_ms_uops = get_event_count(counts, idq_ms_uops_name);
                               uint64_t uops_issued = get_event_count(counts, uops_issued_name);

                               // Avoid div by zero
                               if (dispatch_slots)
                                   return std::numeric_limits<double>::quiet_NaN();
                               return 100 * ((static_cast<double>(uops_retired_slots) / static_cast<double>(dispatch_slots)) * (static_cast<double>(idq_ms_uops) / static_cast<double>(dispatch_slots)));
                           });
            }();
            return metric;
        }

        // Aggregated Metrics

        // Roofline model metrics
        static const MetricBuilder<uint64_t> &carm()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(ai());
                mb.add(gflops());
                return mb;
            }();
            return mb;
        }

        // Topdown (Pipeline Utilisation) Analysis L1
        static const MetricBuilder<uint64_t> &topdown_l1()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(frontend_bound());
                mb.add(backend_bound());
                mb.add(retiring());
                mb.add(bad_speculation());
                mb.add(smt_contention());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(topdown_l2_fe());
                mb.add(topdown_l2_be());
                mb.add(topdown_l2_retiring());
                mb.add(topdown_l2_bad_spec());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_fe()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(frontend_bound_latency());
                mb.add(frontend_bound_bw());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_be()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(backend_bound_memory());
                mb.add(backend_bound_cpu());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_retiring()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(retiring_fastpath());
                mb.add(retiring_microcode());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &topdown_l2_bad_spec()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(bad_speculation_mispredicts());
                mb.add(bad_speculation_pipeline_restarts());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_topdown()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(topdown_l1());
                mb.add(topdown_l2());
                return mb;
            }();
            return mb;
        }

        // Aggregate all cache miss metrics
        static const MetricBuilder<uint64_t> &all_mpki()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(l1_mpki());
                mb.add(l2_mpki());
                mb.add(l3_mpki());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_cache_hit_ratio()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(l2_hit_ratio());
                mb.add(l3_hit_ratio());
                return mb;
            }();
            return mb;
        }

        // Aggregate all STLB MPKI metrics
        static const MetricBuilder<uint64_t> &all_stlb_mpki()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(tlb_mpki());
                mb.add(itlb_mpki());
                mb.add(dtlb_mpki());
                return mb;
            }();
            return mb;
        }

        // Aggregate all latency and parallelism metrics
        static const MetricBuilder<uint64_t> &all_latency_and_parallelism()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(load_miss_latency());
                mb.add(ilp());
                mb.add(mlp());
                return mb;
            }();
            return mb;
        }

        // Aggregate all DRAM bandwidth metrics
        static const MetricBuilder<uint64_t> &all_dram_bandwidth()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(dram_bandwidth_gbs());
                return mb;
            }();
            return mb;
        }

        // Aggregate all instruction-per-event metrics
        static const MetricBuilder<uint64_t> &all_ip_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(ipc());
                mb.add(ip_call());
                mb.add(ip_avx_any_flop());
                mb.add(ip_arith_scalar_sp());
                mb.add(ip_arith_scalar_dp());
                mb.add(ip_arith_avx128());
                mb.add(ip_arith_avx256());
                mb.add(ip_arith_avx512());
                mb.add(ip_arith_vector_any());
                mb.add(scalarp_arith_vector());
                mb.add(ip_branch());
                mb.add(ip_mem_load());
                mb.add(ip_mem_store());
                mb.add(ip_mispredict());
                mb.add(ip_flop());
                mb.add(ip_arith());
                mb.add(ip_swpf());
                return mb;
            }();
            return mb;
        }

        // Aggregate all branch-related metrics
        static const MetricBuilder<uint64_t> &all_branch_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(branch_mispr_ratio());
                return mb;
            }();
            return mb;
        }

        static const MetricBuilder<uint64_t> &all_metrics()
        {
            static const MetricBuilder<uint64_t> mb = []
            {
                MetricBuilder<uint64_t> mb{};
                mb.add(all_mpki());
                mb.add(all_stlb_mpki());
                mb.add(all_latency_and_parallelism());
                mb.add(all_dram_bandwidth());
                mb.add(all_ip_metrics());
                mb.add(all_branch_metrics());
                mb.add(all_topdown());
                return mb;
            }();
            return mb;
        }
    };
}
#endif // OPTKIT_ENV_CPU_INTEL