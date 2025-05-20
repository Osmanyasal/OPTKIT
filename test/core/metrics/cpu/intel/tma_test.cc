#include <iostream>
#include <vector>
#include "gtest/gtest.h"

#include "utils/matrix_utils.hh"

TEST(TMA_Intel, MATMUT_LoopInterchangeIsCorrect)
{
    int rows = 512, mid = 512, cols = 512;
    auto A = createRandomMatrix<float>(rows, mid);
    auto B = createRandomMatrix<float>(mid, cols);

    auto C_normal = createMatrix<float>(rows, cols);
    auto C_loop = createMatrix<float>(rows, cols);

    normalMatrixMultiplication(A, B, C_normal);
    loopInterchangeMatrixMultiplication(A, B, C_loop);

    ASSERT_TRUE(matricesAreEqual(C_normal, C_loop));
}

TEST(TMA_Intel, MATMUT_TiledIsCorrect)
{
    int rows = 512, mid = 512, cols = 512;
    auto A = createRandomMatrix<float>(rows, mid);
    auto B = createRandomMatrix<float>(mid, cols);

    auto C_normal = createMatrix<float>(rows, cols);
    auto C_tiled = createMatrix<float>(rows, cols);

    normalMatrixMultiplication(A, B, C_normal);
    tiledLoopInterchangeMatrixMultiplication(A, B, C_tiled);

    ASSERT_TRUE(matricesAreEqual(C_normal, C_tiled));
}

TEST(TMA_Intel, MATMUT_LoopInterchangeIsFasterThanNormal)
{
    int rows = 4096, mid = 4096, cols = 4096;
    auto A = createRandomMatrix<float>(rows, mid);
    auto B = createRandomMatrix<float>(mid, cols);

    auto C_normal = createMatrix<float>(rows, cols);
    auto C_loop = createMatrix<float>(rows, cols);

    auto start1 = std::chrono::high_resolution_clock::now();
    normalMatrixMultiplication(A, B, C_normal);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto start2 = std::chrono::high_resolution_clock::now();
    loopInterchangeMatrixMultiplication(A, B, C_loop);
    auto end2 = std::chrono::high_resolution_clock::now();

    ASSERT_TRUE(matricesAreEqual(C_normal, C_loop));

    auto normal_time = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    auto loop_time = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    std::cout << "[ PERF ] Normal: " << normal_time << " ms, LoopInterchange: " << loop_time << " ms\n";

    ASSERT_LT(loop_time, normal_time * 1.1); // Allow small margin
}

TEST(TMA_Intel, MATMUT_TiledIsFasterThanLoopInterchange)
{
    int rows = 4096, mid = 4096, cols = 4096;
    auto A = createRandomMatrix<float>(rows, mid);
    auto B = createRandomMatrix<float>(mid, cols);

    auto C_loop = createMatrix<float>(rows, cols);
    auto C_tiled = createMatrix<float>(rows, cols);

    auto start1 = std::chrono::high_resolution_clock::now();
    loopInterchangeMatrixMultiplication(A, B, C_loop);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto start2 = std::chrono::high_resolution_clock::now();
    tiledLoopInterchangeMatrixMultiplication(A, B, C_tiled);
    auto end2 = std::chrono::high_resolution_clock::now();

    ASSERT_TRUE(matricesAreEqual(C_loop, C_tiled));

    auto loop_time = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
    auto tiled_time = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    std::cout << "[ PERF ] LoopInterchange: " << loop_time << " ms, Tiled: " << tiled_time << " ms\n";

    ASSERT_LT(tiled_time, loop_time * 1.1); // Allow small margin
}