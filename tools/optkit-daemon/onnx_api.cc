#include "onnx_api.hh"

#include <numeric>
#include <stdexcept>
#include <utility>

#include <onnxruntime_cxx_api.h>

namespace
{
class OnnxRuntimeApi : public OnnxApi
{
public:
    OnnxRuntimeApi() : env_(ORT_LOGGING_LEVEL_WARNING, "optkit-daemon") {}

    void load_model(const std::string &model_path) override
    {
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        session_ = Ort::Session(env_, model_path.c_str(), session_options);

        if (session_.GetInputCount() == 0 || session_.GetOutputCount() == 0)
            throw std::runtime_error("Invalid model: expected at least one input and one output");

        Ort::AllocatorWithDefaultOptions allocator;
        auto input_name_holder = session_.GetInputNameAllocated(0, allocator);
        auto output_name_holder = session_.GetOutputNameAllocated(0, allocator);
        input_name_ = input_name_holder.get();
        output_name_ = output_name_holder.get();

        auto input_info = session_.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();

        // Some models (especially with dynamic shapes) can report an empty shape via
        // GetShape(), yet still have a known rank. Prefer the explicit dimension APIs.
        const size_t dim_count = input_info.GetDimensionsCount();
        input_shape_.assign(dim_count, 1);
        if (dim_count > 0)
            input_info.GetDimensions(input_shape_.data(), dim_count);

        for (size_t i = 0; i < input_shape_.size(); ++i)
        {
            if (input_shape_[i] <= 0)
                input_shape_[i] = 1;
        }

        input_elements_ = 1;
        if (!input_shape_.empty())
        {
            input_elements_ = static_cast<size_t>(std::accumulate(
                input_shape_.begin(),
                input_shape_.end(),
                int64_t{1},
                [](int64_t acc, int64_t d)
                {
                    return acc * d;
                }));
        }
        model_loaded_ = true;
    }

    TensorSummary input_summary() const override
    {
        if (!model_loaded_)
            throw std::runtime_error("Model is not loaded. Call load_model first.");
        TensorSummary s;
        s.name = input_name_;
        s.shape = input_shape_;
        s.elements = input_elements_;
        return s;
    }

    TensorSummary output_summary() const override
    {
        if (!model_loaded_)
            throw std::runtime_error("Model is not loaded. Call load_model first.");
        TensorSummary s;
        s.name = output_name_;
        s.shape = {};
        s.elements = 0;
        return s;
    }

    InferenceSummary infer(const std::vector<float> &input_data,
                           const std::vector<int64_t> &input_shape_override) override
    {
        if (!model_loaded_)
            throw std::runtime_error("Model is not loaded. Call load_model first.");

        std::vector<float> effective_input = input_data;
        if (effective_input.empty())
            effective_input.assign(input_elements_, 0.0f);

        std::vector<int64_t> runtime_shape;
        size_t runtime_elements = 0;

        if (!input_shape_override.empty())
        {
            runtime_shape = input_shape_override;
        }
        else
        {
            // If the model input shape is unknown/empty (common with dynamic exports),
            // infer a reasonable shape from the provided flat input.
            runtime_shape = input_shape_;
            if (runtime_shape.empty())
                runtime_shape = {1, 1, static_cast<int64_t>(effective_input.size())};
        }

        // Compute runtime element count.
        runtime_elements = 1;
        for (size_t i = 0; i < runtime_shape.size(); ++i)
        {
            const int64_t d = runtime_shape[i];
            if (d <= 0)
                throw std::runtime_error("Invalid (non-positive) input shape dimension");
            runtime_elements *= static_cast<size_t>(d);
        }

        if (effective_input.size() != runtime_elements)
            throw std::runtime_error("Input data size does not match model input shape.");

        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info,
            effective_input.data(),
            effective_input.size(),
            runtime_shape.data(),
            runtime_shape.size());

        const char *input_names[] = {input_name_.c_str()};
        const char *output_names[] = {output_name_.c_str()};

        std::vector<Ort::Value> output_tensors = session_.Run(
            Ort::RunOptions{nullptr},
            input_names,
            &input_tensor,
            1,
            output_names,
            1);

        auto output_info = output_tensors[0].GetTensorTypeAndShapeInfo();
        const size_t output_elements = static_cast<size_t>(output_info.GetElementCount());

        std::vector<float> out_values;
        out_values.resize(output_elements);
        if (output_elements > 0)
        {
            const float *out_ptr = output_tensors[0].GetTensorData<float>();
            std::copy(out_ptr, out_ptr + output_elements, out_values.begin());
        }

        InferenceSummary summary;
        summary.input.name = input_name_;
        summary.input.shape = runtime_shape;
        summary.input.elements = runtime_elements;
        summary.output.name = output_name_;
        summary.output.shape = output_info.GetShape();
        summary.output.elements = output_elements;
        summary.output_values = std::move(out_values);
        return summary;
    }

private:
    Ort::Env env_;
    Ort::Session session_{nullptr};
    bool model_loaded_ = false;
    std::string input_name_;
    std::string output_name_;
    std::vector<int64_t> input_shape_;
    size_t input_elements_ = 0;
};
} // namespace

std::unique_ptr<OnnxApi> create_onnx_api()
{
    return std::unique_ptr<OnnxApi>(new OnnxRuntimeApi());
}
