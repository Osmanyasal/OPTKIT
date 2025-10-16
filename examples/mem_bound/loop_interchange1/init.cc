
#include <cmath>
#include <limits>
#include <random>
#include "solution.hh"

void init(Matrix &matrix)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-0.95f, 0.95f);

    for (int i = 0; i < LOOP_INTERCHANGE1_N; i++)
    {
        float sum = 0;
        for (int j = 0; j < LOOP_INTERCHANGE1_N; j++)
        {
            float value = distribution(generator);
            sum += value * value;
            matrix[i][j] = value;
        }

        // Normalize rows
        if (sum >= std::numeric_limits<float>::min())
        {
            float scale = 1.0f / std::sqrt(sum);
            for (int j = 0; j < LOOP_INTERCHANGE1_N; j++)
            {
                matrix[i][j] *= scale;
            }
        }
    }
}