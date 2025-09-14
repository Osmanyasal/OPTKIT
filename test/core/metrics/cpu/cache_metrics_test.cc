#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

using namespace optkit::metrics;

TEST(CacheMetrics, Instructions_10M_AllMPKI)
{
    OPTKIT_CPU_EVENTS("Instructions_10M_AllMPKI", cpu::core_metrics::AllMPKI());

    for (int i = 0; i < 10; i++)
    {
        instructions_million();
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, Instructions_10M_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("Instructions_10M_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());

    for (int i = 0; i < 10; i++)
    {
        instructions_million();
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, NaiveMatMul_AllMPKI)
{
    OPTKIT_CPU_EVENTS("NaiveMatMul_AllMPKI", cpu::core_metrics::AllMPKI());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);

    normalMatrixMultiplication(A, B, C);

    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, NaiveMatMul_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("NaiveMatMul_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);

    normalMatrixMultiplication(A, B, C);

    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, LoopInterchangeMatMul_AllMPKI)
{
    OPTKIT_CPU_EVENTS("LoopInterchangeMatMul_AllMPKI", cpu::core_metrics::AllMPKI());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);

    loopInterchangeMatrixMultiplication(A, B, C);

    SUCCEED();
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, LoopInterchangeMatMul_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("LoopInterchangeMatMul_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());
    auto A = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto B = createMatrix<double>(MAT_SIZE, MAT_SIZE);
    auto C = createMatrix<double>(MAT_SIZE, MAT_SIZE, 0);

    loopInterchangeMatrixMultiplication(A, B, C);

    SUCCEED();
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, SerialAccessVectorSum_AllMPKI)
{
    OPTKIT_CPU_EVENTS("SerialAccessVectorSum_AllMPKI", cpu::core_metrics::AllMPKI());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << serial_access<int64_t>(generate_vector<int64_t>()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << std::fixed << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, SerialAccessVectorSum_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("SerialAccessVectorSum_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << serial_access<int64_t>(generate_vector<int64_t>()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << std::fixed << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, RandomAccessVectorSum_AllMPKI)
{
    OPTKIT_CPU_EVENTS("RandomAccessVectorSum_AllMPKI", cpu::core_metrics::AllMPKI());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, RandomAccessVectorSum_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("RandomAccessVectorSum_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CacheMetrics, RandomAccessParallelVectorSum_AllMPKI)
{
    OPTKIT_CPU_EVENTS("RandomAccessParallelVectorSum_AllMPKI", cpu::core_metrics::AllMPKI());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access_parallel<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
TEST(CacheMetrics, RandomAccessParallelVectorSum_AllCacheHitRatio)
{
    OPTKIT_CPU_EVENTS("RandomAccessParallelVectorSum_AllCacheHitRatio", cpu::core_metrics::AllCacheHitRatio());
    GTEST_LOG_(INFO) << std::fixed << "Result->" << random_access_parallel<int64_t>(generate_vector<int64_t>(), generate_shuffled_indices()) << " Expected->" << 499999500000.0;
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}