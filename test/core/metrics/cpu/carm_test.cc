#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

using namespace optkit::core::metrics;

TEST(CARMTest, AI)
{
    sleep(1);
    OPTKIT_CPU_EVENTS("AI", cpu::core_metrics::AI());

    std::vector<double> v1 = generate_vector<double>(); // 1 million elements
    std::vector<double> v2 = generate_vector<double>(); // 1 million elements
    std::vector<double> v3 = generate_vector<double>(); // 1 million elements

    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        v3[i] = v1[i] * 2.0 + v2[i];
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}

TEST(CARMTest, FLOPs)
{
    sleep(1);
    std::vector<double> v1 = generate_vector<double>(); // 1 million elements
    std::vector<double> v2 = generate_vector<double>(); // 1 million elements
    std::vector<double> v3 = generate_vector<double>(); // 1 million elements

    OPTKIT_CPU_EVENTS("FLOPs", cpu::core_metrics::FLOPs());
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        v3[i] = v1[i] * 2.0 + v2[i];
    }
    GTEST_LOG_(INFO) << "Results need to be evaluated by the user. In case of inconsistency, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues\n";
}
