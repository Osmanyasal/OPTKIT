#ifndef SOLUTION_HH
#define SOLUTION_HH

#include <vector>

// Naive GPU matrix multiplication - demonstrates poor memory access patterns
void solution(const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C, int size);

#endif // SOLUTION_HH