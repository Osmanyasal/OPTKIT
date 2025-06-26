#pragma once

#include "common/utils.hh"

template <typename T>
inline void normalMatrixMultiplication(const std::vector<std::vector<T>> &A,
                                       const std::vector<std::vector<T>> &B,
                                       std::vector<std::vector<T>> &C)
{
    int n = static_cast<int>(A.size());
    int m = static_cast<int>(B[0].size());
    int p = static_cast<int>(B.size());

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            for (int k = 0; k < p; k++)
                C[i][j] += A[i][k] * B[k][j];
}

template <typename T>
inline void loopInterchangeMatrixMultiplication(const std::vector<std::vector<T>> &A,
                                                const std::vector<std::vector<T>> &B,
                                                std::vector<std::vector<T>> &C)
{
    int n = static_cast<int>(A.size());
    int m = static_cast<int>(B[0].size());
    int p = static_cast<int>(B.size());

    for (int i = 0; i < n; i++)
        for (int k = 0; k < p; k++)
            for (int j = 0; j < m; j++)
                C[i][j] += A[i][k] * B[k][j];
}