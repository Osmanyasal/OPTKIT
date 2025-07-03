#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

using namespace optkit::core::metrics;

TEST(PipelineUtilization, Instructions_10M)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("Instructions_10M", cpu::core_metrics::TopdownL1());

    for (int i = 0; i < 10; i++)
    {
        instructions_million();
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, NaiveMatMul)
{
    sleep(1);
    OPTKIT_CPU_GROUP_EVENTS("NaiveMatMul", cpu::core_metrics::TopdownL1());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);
    normalMatrixMultiplication(A, B, C);

    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
TEST(PipelineUtilization, LoopInterchangeMatMul)
{
    sleep(1);
    OPTKIT_CPU_GROUP_EVENTS("LoopInterchangeMatMul", cpu::core_metrics::TopdownL1());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);
    loopInterchangeMatrixMultiplication(A, B, C);

    SUCCEED();
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
TEST(PipelineUtilization, L1SerialAccessVectorSum)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("L1SerialAccessVectorSum", cpu::core_metrics::TopdownL1());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << serial_access<int64_t>(generate_vector<int64_t>()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << std::fixed << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, L1RandomAccessVectorSum)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("L1RandomAccessVectorSum", cpu::core_metrics::TopdownL1());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, L1RandomAccessParallelVectorSum)
{
    sleep(1);

    OPTKIT_CPU_EVENTS("L1RandomAccessParallelVectorSum", cpu::core_metrics::TopdownL1());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access_parallel<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, TopdownAllSerialAccessVectorSum)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("TopdownAllSerialAccessVectorSum", cpu::core_metrics::AllTopdown());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << serial_access<int64_t>(generate_vector<int64_t>()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << std::fixed << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, TopdownAllRandomAccessVectorSum)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("TopdownAllRandomAccessVectorSum", cpu::core_metrics::AllTopdown());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(PipelineUtilization, TopdownAllRandomAccessParallelVectorSum)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("TopdownAllRandomAccessParallelVectorSum", cpu::core_metrics::AllTopdown());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access_parallel<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
