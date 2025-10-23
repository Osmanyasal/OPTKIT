#include "constants.hh"
#include <vector>
#include <random>

void init(std::vector<char> &data)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(UNBUFFERED_IO_MIN_VALUE, UNBUFFERED_IO_MAX_VALUE);

    // Fill data with random values
    for (auto &byte : data)
    {
        byte = static_cast<char>(dis(gen));
    }
}