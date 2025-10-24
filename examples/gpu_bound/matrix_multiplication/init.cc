#include "constants.hh"
#include <vector>
#include <random>

void init(std::vector<float> &A, std::vector<float> &B, std::vector<float> &C)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    int size = MATRIX_SIZE;

    // Initialize matrices A and B with random values
    A.resize(size * size);
    B.resize(size * size);
    C.resize(size * size);

    for (int i = 0; i < size * size; ++i)
    {
        A[i] = dis(gen);
        B[i] = dis(gen);
        C[i] = 0.0f; // Initialize result matrix to zero
    }
}