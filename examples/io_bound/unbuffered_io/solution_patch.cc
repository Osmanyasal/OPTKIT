#include "solution_patch.hh"
#include "constants.hh"
#include <fstream>
#include <stdexcept>

// Optimized buffered I/O solution - uses standard library buffering
void solution_patch(const std::vector<char> &data, const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        throw std::runtime_error("Failed to open file for buffered writing");
    }

    // Set a larger buffer size for better performance
    const size_t buffer_size = 64 * 1024; // 64KB buffer
    std::vector<char> buffer(buffer_size);
    file.rdbuf()->pubsetbuf(buffer.data(), buffer_size);

    // Write all data at once - let the library handle buffering
    file.write(data.data(), data.size());

    if (!file.good())
    {
        throw std::runtime_error("Failed to write data to file");
    }

    // File destructor will handle the final flush
}