#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <tensorflow/c/c_api.h>

// Function to read scaler values from scaler.txt
bool readScalerFile(const char *filename, std::vector<float> &mean_values, std::vector<float> &scale_values)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening scaler file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int count = 0;
    while (std::getline(file, line) && count < 6)
    {
        std::istringstream iss(line);
        float value;
        if (!(iss >> value))
        {
            std::cerr << "Error reading scaler file: " << filename << std::endl;
            return false;
        }
        if (count < 3)
        {
            mean_values.push_back(value);
        }
        else
        {
            scale_values.push_back(value);
        }
        count++;
    }

    if (mean_values.size() != 3 || scale_values.size() != 3)
    {
        std::cerr << "Error: scaler file does not contain expected number of values." << std::endl;
        return false;
    }

    file.close();
    return true;
}

// Function to scale input data
void scaleInput(float *data, const std::vector<float> &mean, const std::vector<float> &scale, int num_features)
{
    for (int i = 0; i < num_features; ++i)
    {
        data[i] = (data[i] - mean[i]) / scale[i];
    }
}

int main()
{
    // Load the SavedModel
    TF_SessionOptions *session_opts = TF_NewSessionOptions();
    TF_Buffer *run_opts = nullptr;
    const char *saved_model_dir = "/usr/local/include/optkit_governor_model/";
    const char *tags[] = {"serve"};
    TF_Graph *graph = TF_NewGraph();
    TF_Status *status = TF_NewStatus(); // Initialize TF_Status

    TF_Session *session = TF_LoadSessionFromSavedModel(session_opts, run_opts, saved_model_dir, tags, 1, graph, nullptr, status);

    if (TF_GetCode(status) != TF_OK)
    {
        std::cerr << "Error loading SavedModel: " << TF_Message(status) << std::endl;
        TF_DeleteGraph(graph);
        TF_DeleteSessionOptions(session_opts);
        TF_DeleteStatus(status); // Clean up status
        return 1;
    }

    // Example input data
    float new_data[3]{138944, 0.202812, 1.46747e-06};

    // Load scaler
    std::vector<float> scaler_mean, scaler_scale;
    readScalerFile("/usr/local/include/optkit_governor_model/scaler.txt", scaler_mean, scaler_scale);

    // Scale input data
    scaleInput(new_data, scaler_mean, scaler_scale, 3);

    // Create input tensor
    const int64_t input_dims[2]{1, 3};
    TF_Tensor *input_tensor = TF_NewTensor(TF_FLOAT, input_dims, 2, new_data, 3 * sizeof(float), [](void *data, size_t len, void *arg) {}, nullptr);
 

    // Prepare input and output tensors
    const char *input_names[] = {"serving_default_dense_input"};
    TF_Output inputs[] = {
        {TF_GraphOperationByName(graph, input_names[0]), 0}};

    const char *output_names[] = {"StatefulPartitionedCall"};
    TF_Output outputs[] = {
        {TF_GraphOperationByName(graph, output_names[0]), 0}};
 
    // Run session to make prediction
    TF_Tensor *output_tensor = nullptr;
    TF_SessionRun(session,
                  nullptr,                    // Run options.
                  inputs, &input_tensor, 1,   // Input tensors, input tensor count.
                  outputs, &output_tensor, 1, // Output tensors, output tensor count.
                  nullptr, 0,                 // Target operations, number of targets.
                  nullptr,                    // Run metadata.
                  status);                    // Status object
 
    // Print prediction
    if (output_tensor)
    {
        auto data = static_cast<float *>(TF_TensorData(output_tensor));
        std::cout << "Predicted values:" << std::endl;
        for (int i = 0; i < 2; ++i)
        {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;

        TF_DeleteTensor(output_tensor);
    }

    // Clean up
    TF_DeleteTensor(input_tensor);
    TF_DeleteGraph(graph);
    TF_DeleteSession(session, status); // Use status for TF_DeleteSession
    TF_DeleteSessionOptions(session_opts);
    TF_DeleteStatus(status); // Clean up status

    return 0;
}
