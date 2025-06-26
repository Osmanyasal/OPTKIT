#include "common/utils.hh"

// Serial access: natural order
template <class T>
inline double serial_access(const std::vector<T> &vec)
{
    double sum = 0.0;
    for (const auto &v : vec)
        sum += v;
    return sum;
}

// Serial access: using shuffled indices
template <class T>
inline double random_access(const std::vector<T> &vec, const std::vector<size_t> &indices)
{
    double sum = 0.0;
    for (const auto &index : indices)
        sum += vec[index];
    return sum;
}

// Parallel access: using shuffled indices
template <class T>
inline double random_access_parallel(const std::vector<T> &vec, const std::vector<size_t> &indices)
{
    double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < static_cast<int>(indices.size()); ++i)
        sum += vec[indices[i]];
    return sum;
}
