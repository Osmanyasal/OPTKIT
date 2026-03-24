#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct TensorSummary
{
    std::string name;
    std::vector<int64_t> shape;
    size_t elements = 0;
};

struct InferenceSummary
{
    TensorSummary input;
    TensorSummary output;
    std::vector<float> output_values;
};

class OnnxApi
{
public:
    virtual ~OnnxApi() {}

    virtual void load_model(const std::string &model_path) = 0;
    virtual TensorSummary input_summary() const = 0;
    virtual TensorSummary output_summary() const = 0;
    virtual InferenceSummary infer(const std::vector<float> &input_data = std::vector<float>(),
                                  const std::vector<int64_t> &input_shape = std::vector<int64_t>()) = 0;
};

std::unique_ptr<OnnxApi> create_onnx_api();
