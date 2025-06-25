#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

#include "common/instructions.hh"
#include "common/matmul.hh"

using namespace optkit::core::metrics;

#define _10M 10000
#define MAT_SIZE 512

TEST(PipelineUtilization, Instructions_10M)
{
    OPTKIT_CPU_EVENTS("Instructions_10M", cpu::core_metrics::AllTopdown());

    for (int i = 0; i < _10M; i++)
    {
        instructions_million();
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

std::vector<std::vector<int>> createMatrix(int rows, int cols, int val = 1)
{
    return std::vector<std::vector<int>>(rows, std::vector<int>(cols, val));
}

TEST(PipelineUtilization, NaiveMatMul)
{
    OPTKIT_CPU_EVENTS("NaiveMatMul", cpu::core_metrics::AllTopdown());
    auto A = createMatrix(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix(MAT_SIZE, MAT_SIZE, 0);

    normalMatrixMultiplication(A, B, C);

    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
TEST(PipelineUtilization, LoopInterchangeMatMul)
{
    OPTKIT_CPU_EVENTS("LoopInterchangeMatMul", cpu::core_metrics::AllTopdown());
    auto A = createMatrix(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix(MAT_SIZE, MAT_SIZE, 0);

    loopInterchangeMatrixMultiplication(A, B, C);

    SUCCEED();
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}