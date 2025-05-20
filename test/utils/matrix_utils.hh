#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <vector>
#include <cstdlib>   // for std::rand, std::srand
#include <algorithm> // for std::min
#include "./utils/cpu_opt.hh"

// Performs normal matrix multiplication: C = A * B
template <typename T>
inline void normalMatrixMultiplication(
    const std::vector<std::vector<T>> &A,
    const std::vector<std::vector<T>> &B,
    std::vector<std::vector<T>> &C)
{
    int32_t n = A.size();
    int32_t m = B[0].size();
    int32_t p = B.size();

    for (int32_t i = 0; i < n; i++)
        for (int32_t j = 0; j < m; j++)
            for (int32_t k = 0; k < p; k++)
                C[i][j] += A[i][k] * B[k][j];
}

// Performs matrix multiplication using loop interchange optimization: C = A * B
template <typename T>
inline void loopInterchangeMatrixMultiplication(
    const std::vector<std::vector<T>> &A,
    const std::vector<std::vector<T>> &B,
    std::vector<std::vector<T>> &C)
{
    int32_t n = A.size();
    int32_t m = B[0].size();
    int32_t p = B.size();

    for (int32_t i = 0; i < n; i++)
        for (int32_t k = 0; k < p; k++)
            for (int32_t j = 0; j < m; j++)
                C[i][j] += A[i][k] * B[k][j];
}

// Performs tiled matrix multiplication using loop interchange
template <typename T>
inline void tiledLoopInterchangeMatrixMultiplication(
    const std::vector<std::vector<T>> &A,
    const std::vector<std::vector<T>> &B,
    std::vector<std::vector<T>> &C,
    int32_t tileSize = OPT_NUM_ELEMENTS_PER_CACHELINE(T) * 4)
{
    int32_t n = A.size();    // rows of A and C
    int32_t m = B[0].size(); // cols of B and C
    int32_t p = B.size();    // cols of A and rows of B

    for (int32_t ii = 0; ii < n; ii += tileSize)
        for (int32_t kk = 0; kk < p; kk += tileSize)
            for (int32_t jj = 0; jj < m; jj += tileSize)
                for (int32_t i = ii; i < std::min(ii + tileSize, n); i++)
                    for (int32_t k = kk; k < std::min(kk + tileSize, p); k++)
                        for (int32_t j = jj; j < std::min(jj + tileSize, m); j++)
                            C[i][j] += A[i][k] * B[k][j];
}

// Checks if two matrices are equal
template <typename T>
inline bool matricesAreEqual(const std::vector<std::vector<T>> &A,
                             const std::vector<std::vector<T>> &B)
{
    if (A.size() != B.size() || A[0].size() != B[0].size())
        return false;

    for (size_t i = 0; i < A.size(); ++i)
        for (size_t j = 0; j < A[0].size(); ++j)
            if (A[i][j] != B[i][j])
                return false;

    return true;
}

// Creates a zero-initialized matrix with the given dimensions
template <typename T>
inline std::vector<std::vector<T>> createMatrix(int32_t rows, int32_t cols, T val = 0)
{
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, val));
}

// Creates a matrix filled with random values between 0 and 99 (int) or 0.0 to 99.9 (float/double)
template <typename T>
inline std::vector<std::vector<T>> createRandomMatrix(int32_t rows, int32_t cols)
{
    std::srand(42); // Fixed seed for reproducibility
    std::vector<std::vector<T>> A(rows, std::vector<T>(cols));

    for (int32_t i = 0; i < rows; i++)
        for (int32_t j = 0; j < cols; j++)
            A[i][j] = static_cast<T>(rand() % 100); // You can modify this for floating-point ranges
    return A;
}

#endif // MATRIX_UTILS_H
