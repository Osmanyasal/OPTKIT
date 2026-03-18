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
};

class OnnxApi
{
public:
    virtual ~OnnxApi() {}

    virtual void load_model(const std::string &model_path) = 0;
    virtual InferenceSummary infer(const std::vector<float> &input_data = std::vector<float>()) = 0;
};

std::unique_ptr<OnnxApi> create_onnx_api();
