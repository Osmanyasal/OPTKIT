#include "constants.hh"
#include <vector>
#include <string>
#include <random>

void init(std::vector<std::string> &data_chunks)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> char_dis('A', 'Z');

    // Create many small data chunks
    data_chunks.reserve(SMALL_WRITES_COUNT);

    for (int i = 0; i < SMALL_WRITES_COUNT; ++i)
    {
        std::string chunk;
        chunk.reserve(SMALL_WRITE_SIZE);

        // Fill chunk with random characters
        for (int j = 0; j < SMALL_WRITE_SIZE; ++j)
        {
            chunk += static_cast<char>(char_dis(gen));
        }

        data_chunks.push_back(std::move(chunk));
    }
}