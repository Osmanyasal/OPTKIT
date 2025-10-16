#include "solution_patch.hh"

#include <memory>
#include <string_view>

// Multiply two square matrices - Optimized version
void multiply_patch(Matrix &result, const Matrix &a, const Matrix &b)
{
    zero(result);

    // Loop interchange + blocking for better cache utilization
    const int BLOCK_SIZE = 64; // Tune this based on your cache size

    for (int ii = 0; ii < LOOP_INTERCHANGE1_N; ii += BLOCK_SIZE)
    {
        for (int kk = 0; kk < LOOP_INTERCHANGE1_N; kk += BLOCK_SIZE)
        {
            for (int jj = 0; jj < LOOP_INTERCHANGE1_N; jj += BLOCK_SIZE)
            {
                // Process block
                int i_max = std::min(ii + BLOCK_SIZE, LOOP_INTERCHANGE1_N);
                int k_max = std::min(kk + BLOCK_SIZE, LOOP_INTERCHANGE1_N);
                int j_max = std::min(jj + BLOCK_SIZE, LOOP_INTERCHANGE1_N);

                for (int i = ii; i < i_max; ++i)
                {
                    for (int k = kk; k < k_max; ++k)
                    {
                        const double a_ik = a[i][k]; // Hoist invariant
                        for (int j = jj; j < j_max; ++j)
                        {
                            result[i][j] += a_ik * b[k][j];
                        }
                    }
                }
            }
        }
    }
}

// Compute integer power of a given square matrix
Matrix power_patch(const Matrix &input, const uint32_t k)
{
    // Temporary products
    std::unique_ptr<Matrix> productCurrent(new Matrix());
    std::unique_ptr<Matrix> productNext(new Matrix());

    // Temporary elements = a^(2^integer)
    std::unique_ptr<Matrix> elementCurrent(new Matrix());
    std::unique_ptr<Matrix> elementNext(new Matrix());

    // Initial values
    identity(*productCurrent);
    *elementCurrent = input;

    // Use binary representation of k to be O(log(k))
    for (auto i = k; i > 0; i /= 2)
    {
        if (i % 2 != 0)
        {
            // Multiply the product by element
            multiply_patch(*productNext, *productCurrent, *elementCurrent);
            std::swap(productNext, productCurrent);

            // Exit early to skip next squaring
            if (i == 1)
                break;
        }

        // Square an element
        multiply_patch(*elementNext, *elementCurrent, *elementCurrent);
        std::swap(elementNext, elementCurrent);
    }

    return std::move(*productCurrent);
}