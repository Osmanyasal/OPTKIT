#include <algorithm>
#include <array>
#include <random>
#include "solution.hh"

void solution(std::vector<S> &arr)
{
    // 1. shuffle
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);

    // 2. counting sort
    constexpr int cntSize = DATA_PACKING_MAX_RANDOM - DATA_PACKING_MIN_RANDOM + 1;
    std::array<int, cntSize> cnt{};
    for (const auto &v : arr)
    {
        ++cnt[v.i - DATA_PACKING_MIN_RANDOM + 1];
    }
    for (int i = 1; i < cntSize; ++i)
    {
        cnt[i] += cnt[i - 1];
    }
    std::vector<S> sorted(DATA_PACKING_N);
    for (const auto &v : arr)
    {
        sorted[cnt[v.i - DATA_PACKING_MIN_RANDOM]++] = v;
    }
    arr = sorted;
}