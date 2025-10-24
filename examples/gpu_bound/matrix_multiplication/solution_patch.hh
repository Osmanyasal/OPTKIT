#ifndef SOLUTION_PATCH_HH
#define SOLUTION_PATCH_HH

#include <vector>

// Optimized GPU matrix multiplication using shared memory tiling
void solution_patch(const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C, int size);

#endif // SOLUTION_PATCH_HH