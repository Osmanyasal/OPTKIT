#include "common/matmul.hh"

void normalMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int j = 0; j < m; j++)
        { // Iterate over columns of B
            for (int k = 0; k < p; k++)
            { // Sum over the common dimension
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void loopInterchangeMatrixMultiplication(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B, std::vector<std::vector<int>> &C)
{
    int n = A.size();
    int m = B[0].size();
    int p = B.size();

    for (int i = 0; i < n; i++)
    { // Iterate over rows of A
        for (int k = 0; k < p; k++)
        { // Iterate over columns of A/rows of B
            for (int j = 0; j < m; j++)
            { // Iterate over columns of B
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
