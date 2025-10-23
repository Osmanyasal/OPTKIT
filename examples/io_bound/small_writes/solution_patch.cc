#include "solution_patch.hh"
#include "constants.hh"
#include <fstream>
#include <stdexcept>
#include <sstream>

// Optimized batched writes solution - accumulate data and write in larger chunks
void solution_patch(const std::vector<std::string> &data_chunks, const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        throw std::runtime_error("Failed to open file for batched writes");
    }

    // Set a large buffer for better performance
    const size_t buffer_size = 64 * 1024; // 64KB buffer
    std::vector<char> buffer(buffer_size);
    file.rdbuf()->pubsetbuf(buffer.data(), buffer_size);

    // Accumulate data in memory before writing
    std::ostringstream batch_buffer;
    size_t current_batch_size = 0;

    for (const auto &chunk : data_chunks)
    {
        batch_buffer << chunk;
        current_batch_size += chunk.size();

        // Write when we reach a reasonable batch size
        if (current_batch_size >= LARGE_WRITE_SIZE)
        {
            std::string batch_data = batch_buffer.str();
            file.write(batch_data.c_str(), batch_data.size());

            // Reset for next batch
            batch_buffer.str("");
            batch_buffer.clear();
            current_batch_size = 0;
        }
    }

    // Write any remaining data
    if (current_batch_size > 0)
    {
        std::string batch_data = batch_buffer.str();
        file.write(batch_data.c_str(), batch_data.size());
    }

    if (!file.good())
    {
        throw std::runtime_error("Failed to write batched data to file");
    }

    // File destructor will handle the final flush
}