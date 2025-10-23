#include "solution.hh"
#include "constants.hh"
#include <fstream>
#include <stdexcept>
#include <filesystem>

// File fragmentation solution - creates many small files (demonstrates file system overhead)
void solution(const std::vector<std::string> &data_chunks, const std::string &base_dir)
{
    // Create directory if it doesn't exist
    std::filesystem::create_directories(base_dir);

    // Create many small files - this is the bottleneck!
    for (size_t i = 0; i < data_chunks.size() && i < FRAGMENTED_FILE_COUNT; ++i)
    {
        std::string filename = base_dir + "/fragment_" + std::to_string(i) + ".dat";

        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            throw std::runtime_error("Failed to create fragmented file: " + filename);
        }

        file.write(data_chunks[i].c_str(), data_chunks[i].size());

        if (!file.good())
        {
            throw std::runtime_error("Failed to write to fragmented file: " + filename);
        }

        // File is closed automatically, forcing metadata updates
        // Each file creation/close involves significant file system overhead
    }
}