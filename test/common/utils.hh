#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <omp.h>
#include <chrono>

#include "utils/environment_config.hh"

#define CODE_UNIMPLEMENTED -1
#define ERROR_RESULT -2
#define ERROR_RATE 0.05 // 5% error rate
#define REPEAT 100      // repeat measurements 100 times
#define MAT_SIZE 1024
#define VECTOR_SIZE 1'000'000

// Generate a vector of doubles where vec[i] = i
template <class T>
inline std::vector<T> generate_vector(size_t n = VECTOR_SIZE)
{
    std::vector<T> vec(n);
    for (size_t i = 0; i < n; ++i)
        vec[i] = static_cast<T>(i);
    return vec;
}

// Generate a shuffled index vector: [0, 1, ..., n-1] randomly shuffled
inline std::vector<size_t> generate_shuffled_indices(size_t n = VECTOR_SIZE)
{
    std::vector<size_t> indices(n);
    for (size_t i = 0; i < n; ++i)
        indices[i] = i;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);
    return indices;
}


template <class T>
inline std::vector<std::vector<T>> createMatrix(size_t rows, size_t cols, size_t val = 1)
{
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, val));
}
