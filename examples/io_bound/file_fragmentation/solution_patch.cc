#include "solution_patch.hh"
#include "constants.hh"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <sstream>

// Optimized solution - creates fewer large files with same total data
void solution_patch(const std::vector<std::string> &data_chunks, const std::string &base_dir)
{
    // Create directory if it doesn't exist
    std::filesystem::create_directories(base_dir);

    // Calculate how many chunks go into each large file
    size_t chunks_per_file = data_chunks.size() / LARGE_FILE_COUNT;

    // Create fewer large files
    for (int file_idx = 0; file_idx < LARGE_FILE_COUNT; ++file_idx)
    {
        std::string filename = base_dir + "/consolidated_" + std::to_string(file_idx) + ".dat";

        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            throw std::runtime_error("Failed to create consolidated file: " + filename);
        }

        // Set larger buffer for better performance
        const size_t buffer_size = 64 * 1024; // 64KB buffer
        std::vector<char> buffer(buffer_size);
        file.rdbuf()->pubsetbuf(buffer.data(), buffer_size);

        // Write multiple chunks to single file
        size_t start_idx = file_idx * chunks_per_file;
        size_t end_idx = (file_idx == LARGE_FILE_COUNT - 1) ? data_chunks.size() : (file_idx + 1) * chunks_per_file;

        // Consolidate data before writing
        std::ostringstream consolidated_data;
        for (size_t i = start_idx; i < end_idx; ++i)
        {
            consolidated_data << data_chunks[i];
        }

        // Single large write instead of many small files
        std::string data_str = consolidated_data.str();
        file.write(data_str.c_str(), data_str.size());

        if (!file.good())
        {
            throw std::runtime_error("Failed to write to consolidated file: " + filename);
        }

        // Only one file close per large file instead of many
    }
}