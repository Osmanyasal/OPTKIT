#include "solution.hh"
#include "constants.hh"
#include <fstream>
#include <stdexcept>
#include <iostream>

// Small frequent writes solution - demonstrates I/O bottleneck from many syscalls
void solution(const std::vector<std::string> &data_chunks, const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        throw std::runtime_error("Failed to open file for small writes");
    }

    // Write each chunk individually with immediate flush - this is the bottleneck!
    for (const auto &chunk : data_chunks)
    {
        file.write(chunk.c_str(), chunk.size());
        file.flush(); // Force immediate write - causes many syscalls
    }

    if (!file.good())
    {
        throw std::runtime_error("Failed to write data chunks to file");
    }
}