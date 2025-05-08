
#include "core/freq/base_governor.hh"

namespace optkit::core::freq
{

    BaseGovernor *BaseGovernor::current_governor = nullptr;
    std::vector<float> BaseGovernor::mean;
    std::vector<float> BaseGovernor::scale;

    TF_Tensor *BaseGovernor::input_tensor;
    TF_SessionOptions *BaseGovernor::session_opts;
    TF_Buffer *BaseGovernor::run_opts;
    const char *BaseGovernor::saved_model_dir;
    const char *BaseGovernor::tags[1];
    TF_Graph *BaseGovernor::graph;
    TF_Status *BaseGovernor::status;
    TF_Session *BaseGovernor::session;

    float BaseGovernor::current_core_freq = 0;
    float BaseGovernor::current_uncore_freq = 0;

    BaseGovernor::BaseGovernor(bool data_collector_mode, int64_t sample_period) : data_collector_mode{data_collector_mode}, config{false, true, false, 0, -1}, sample_period{sample_period}
    {
        this->config.perf_event_config.sample_period = this->sample_period;
        this->config.cpu = Query::OPTKIT_SOCKET1__ENABLED ? Query::detect_packages().at(1)[0] : Query::detect_packages().at(0)[0];
        memset(&sa, 0, sizeof(struct sigaction));
        if (data_collector_mode)
            sa.sa_sigaction = BaseGovernor::collector_call_back;
        else
            sa.sa_sigaction = BaseGovernor::call_back;

        sa.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR2, &sa, NULL) < 0)
        {
            OPTKIT_CORE_ERROR("Error setting up signal handler");
            exit(1);
        }

        if (!data_collector_mode)
        {
            read_scaler_file(BaseGovernor::mean, BaseGovernor::scale);

            session_opts = TF_NewSessionOptions();
            run_opts = nullptr;
            tags[0] = "serve";
            graph = TF_NewGraph();
            status = TF_NewStatus();
            saved_model_dir = "/usr/local/include/optkit_governor_model/";
            session = TF_LoadSessionFromSavedModel(session_opts, run_opts, saved_model_dir, tags, 1, graph, nullptr, status);
        }
    }

    BaseGovernor::~BaseGovernor()
    {
        if (!data_collector_mode)
        {
            // Clean up
            TF_DeleteGraph(graph);
            TF_DeleteSession(session, status); // Use status for TF_DeleteSession
            TF_DeleteSessionOptions(session_opts);
            TF_DeleteStatus(status); // Clean up status
        }
    }

    void BaseGovernor::collector_call_back(int32_t signum, siginfo_t *oh, void *blah)
    {
        if (OPT_UNLIKELY(BaseGovernor::current_governor == nullptr))
            return;
        current_governor->disalbe_callback_trigger();

        current_governor->snapshot_pmus();
        double compute_intensity = current_governor->compute_intensity();
        double dram_intensity = current_governor->dram_intensity();
        double cache_intensity = current_governor->cache_intensity();

        std::cout << compute_intensity << "," << cache_intensity << "," << dram_intensity << ",best_core_freq,best_uncore_freq\n";

        // ENABLE CALL_BACK TRIGGER to prevent multiple entry
        current_governor->enable_callback_trigger();
    }

    void BaseGovernor::call_back(int32_t signum, siginfo_t *oh, void *blah)
    {
        if (OPT_UNLIKELY(BaseGovernor::current_governor == nullptr))
            return;
        current_governor->disalbe_callback_trigger();
        static int32_t ctor = 1;
        std::cout << "called:" << ctor++
                  << std::endl;
        current_governor->snapshot_pmus();
        static double compute_intensity = 0;
        static double cache_intensity = 0;
        static double dram_intensity = 0;

        double current_compute_intensity = current_governor->compute_intensity();
        double current_cache_intensity = current_governor->cache_intensity();
        double current_dram_intensity = current_governor->dram_intensity();

        // detect phase change and return if necessary
        if ((std::abs(current_compute_intensity - compute_intensity) < compute_intensity / 10) &&
            (std::abs(current_cache_intensity - cache_intensity) < cache_intensity / 10) &&
            (std::abs(current_dram_intensity - dram_intensity) < dram_intensity / 10))
        {
            // std::cout << "Early return\n";
            current_governor->enable_callback_trigger();
            return;
        }

        compute_intensity = current_compute_intensity;
        cache_intensity = current_cache_intensity;
        dram_intensity = current_dram_intensity;

        // scale input!
        static float new_data[3]{0, 0, 0};
        new_data[0] = (compute_intensity - BaseGovernor::mean[0]) / BaseGovernor::scale[0];
        new_data[1] = (cache_intensity - BaseGovernor::mean[1]) / BaseGovernor::scale[1];
        new_data[2] = (dram_intensity - BaseGovernor::mean[2]) / BaseGovernor::scale[2];

        static const int64_t input_dims[2]{1, 3};
        input_tensor = TF_NewTensor(TF_FLOAT, input_dims, 2, new_data, 3 * sizeof(float), [](void *data, size_t len, void *arg) {}, nullptr);

        static const char *input_names[] = {"serving_default_dense_input"};
        static TF_Output inputs[] = {
            {TF_GraphOperationByName(graph, input_names[0]), 0}};

        static const char *output_names[] = {"StatefulPartitionedCall"};
        static TF_Output outputs[] = {
            {TF_GraphOperationByName(graph, output_names[0]), 0}};

        // Run session to make prediction
        static TF_Tensor *output_tensor = nullptr;
        TF_SessionRun(session,
                      nullptr,                    // Run options.
                      inputs, &input_tensor, 1,   // Input tensors, input tensor count.
                      outputs, &output_tensor, 1, // Output tensors, output tensor count.
                      nullptr, 0,                 // Target operations, number of targets.
                      nullptr,                    // Run metadata.
                      status);                    // Status object

        // return if freq bigger than the freq rank!
        static int64_t max_core_freq = QueryFreq::get_cpuinfo_max_freq();
        static int64_t min_core_freq = QueryFreq::get_cpuinfo_min_freq();
        static std::pair<int64_t, int64_t> uncore_min_max = CPUFrequency::get_uncore_min_max(0); // get socket 1s core-uncore freq

        float *data = static_cast<float *>(TF_TensorData(output_tensor));

        int64_t data_0 = (std::ceil(data[0] * 10 + 0.5) / 10) * GHZ;
        int64_t data_1 = (std::ceil(data[1] * 10 + 0.5) / 10) * GHZ;
        data_0 = std::max(min_core_freq, std::min(data_0, max_core_freq));
        data_1 = std::max(uncore_min_max.first, std::min(data_1, uncore_min_max.second));

        static int64_t _threshold = 0.2 * GHZ;
        if (std::abs(current_core_freq - data_0) < _threshold && std::abs(current_uncore_freq - data_1) < _threshold)
        {
            current_governor->enable_callback_trigger();
            return;
        }

        std::cout << "pmu snapshot: " << compute_intensity << ", " << cache_intensity << ", " << dram_intensity << " --- current: " << current_core_freq << " - " << current_uncore_freq << " --- estm: " << data_0 << " - " << data_1 << "\n";

        // std::cout << "current uncore -----> " << uncore_min_max << std::endl;
        if (Query::OPTKIT_SOCKET0__ENABLED)
        {
            CPUFrequency::set_core_frequency(current_core_freq = data_0, 0);
            CPUFrequency::set_uncore_frequency(current_uncore_freq = data_1, 0);
        }
        if (Query::OPTKIT_SOCKET1__ENABLED)
        {
            CPUFrequency::set_core_frequency(current_core_freq = data_0, 1);
            CPUFrequency::set_uncore_frequency(current_uncore_freq = data_1, 1);
        }

        TF_DeleteTensor(output_tensor);
        TF_DeleteTensor(input_tensor);

        // ENABLE CALL_BACK TRIGGER to prevent multiple entry
        current_governor->enable_callback_trigger();
    }

    void BaseGovernor::read_scaler_file(std::vector<float> &mean_values, std::vector<float> &scale_values)
    {
        mean_values.clear();
        scale_values.clear();
        static std::string file_txt = ::read_file("/usr/local/include/optkit_governor_model/scaler.txt");
        auto lines = ::str_split(file_txt, "\n");
        mean_values.push_back(std::atof(lines[0].c_str()));
        mean_values.push_back(std::atof(lines[1].c_str()));
        mean_values.push_back(std::atof(lines[2].c_str()));
        scale_values.push_back(std::atof(lines[3].c_str()));
        scale_values.push_back(std::atof(lines[4].c_str()));
        scale_values.push_back(std::atof(lines[5].c_str()));
    }
}