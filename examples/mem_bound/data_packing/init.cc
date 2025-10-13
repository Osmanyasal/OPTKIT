#include "solution.hh"
#include "solution_patch.hh"
#include "constants.hh"
#include <random>

template <typename T>
T create_entry(int first_value, int second_value)
{
    T entry;

    entry.i = first_value;
    entry.s = static_cast<short>(second_value);
    entry.l = static_cast<long long>(first_value * second_value);
    entry.d = static_cast<double>(first_value) / DATA_PACKING_MAX_RANDOM;
    entry.b = first_value < second_value;

    return entry;
}

template <typename T>
void init(std::vector<T> &arr)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(DATA_PACKING_MIN_RANDOM, DATA_PACKING_MAX_RANDOM - 1);

    for (int i = 0; i < DATA_PACKING_N; i++)
    {
        int random_int1 = distribution(generator);
        int random_int2 = distribution(generator);

        arr[i] = create_entry<T>(random_int1, random_int2);
    }
}

// Explicit template instantiations
template S create_entry<S>(int first_value, int second_value);
template void init<S>(std::vector<S> &arr);

template S_patch create_entry<S_patch>(int first_value, int second_value);
template void init<S_patch>(std::vector<S_patch> &arr);