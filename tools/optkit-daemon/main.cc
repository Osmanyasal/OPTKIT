#include <iostream>
#include <string>
#include <cstdlib>

#include "onnx_api.hh"

std::string resolve_model_path(int argc, char **argv)
{
    if (argc > 1)
        return argv[1];

    const char *env_model = std::getenv("OPTKIT_ONNX_MODEL");
    if (env_model != nullptr && env_model[0] != '\0')
        return std::string(env_model);

    return {};
}

int main(int argc, char **argv)
{
    std::string model_path = resolve_model_path(argc, argv);
    if (model_path.empty())
    {
        std::cerr << "Usage: " << argv[0] << " <model.onnx>\n";
        std::cerr << "Or set OPTKIT_ONNX_MODEL=<model.onnx>\n";
        return EXIT_FAILURE;
    }

    try
    {
        std::unique_ptr<OnnxApi> onnx_api = create_onnx_api();
        onnx_api->load_model(model_path);
        InferenceSummary summary = onnx_api->infer();

        std::cout << "Model loaded: " << model_path << "\n";
        std::cout << "Input: " << summary.input.name << " elements=" << summary.input.elements << "\n";
        std::cout << "Output: " << summary.output.name << " elements=" << summary.output.elements << " shape=[";
        for (size_t i = 0; i < summary.output.shape.size(); ++i)
        {
            if (i)
                std::cout << ",";
            std::cout << summary.output.shape[i];
        }
        std::cout << "]\n";
        std::cout << "Inference completed successfully\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}