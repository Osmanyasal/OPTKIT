#pragma once

#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <tensorflow/c/c_api.h>

#include "core/profiling/profiler_config.hh"
#include "core/profiling/pmu/block_group_profiler.hh"
#include "core/profiling/pmu/block_profiler.hh"
#include "core/freq/cpu_frequency.hh"
#include "core/freq/query_frequency.hh"
 


#define OPTKIT_BASE_GOVERNOR_MMAP_PAGES 8
#define OPTKIT_BASE_GOVERNOR_GOVERNOR_CALLBACK_PERIOD_MS 10000 // 10 sec 1 call
#define GHZ 1000000000
namespace optkit::core::freq
{
    /**
     * @brief Base governor is called each 5ms by defaut.
     *
     */
    class BaseGovernor
    {
    public:
        // this will be called when sample period exceeds
        static void call_back(int32_t signum, siginfo_t *oh, void *blah);
        static void collector_call_back(int32_t signum, siginfo_t *oh, void *blah);
        static BaseGovernor *current_governor;
        
    private:
        static std::vector<float> mean;
        static std::vector<float> scale;
        static TF_Tensor *input_tensor;
        static TF_SessionOptions *session_opts;
        static TF_Buffer *run_opts;
        static const char *saved_model_dir;
        static const char *tags[1];
        static TF_Graph *graph;
        static TF_Status *status;
        static TF_Session *session;
        static float current_core_freq;
        static float current_uncore_freq;

    public:
        BaseGovernor(bool data_collector_mode = false ,int64_t sample_period = (((QueryFreq::get_cpuinfo_max_freq() + QueryFreq::get_cpuinfo_min_freq()) / 2) / 1000) * OPTKIT_BASE_GOVERNOR_GOVERNOR_CALLBACK_PERIOD_MS);
        virtual ~BaseGovernor();

        virtual void snapshot_pmus() = 0;
        virtual double compute_intensity() = 0;
        virtual double dram_intensity() = 0;
        virtual double cache_intensity() = 0;
        virtual void disalbe_callback_trigger() = 0;
        virtual void enable_callback_trigger() = 0;

    private:
        // Function to read scaler values from scaler.txt
        void read_scaler_file(std::vector<float> &mean_values, std::vector<float> &scale_values);
 

    protected:
        bool data_collector_mode;
        std::vector<uint64_t> pmu_record;
        optkit::core::ProfilerConfig config;
        const int64_t sample_period;
        struct sigaction sa;
        void *our_mmap;
    };
}
