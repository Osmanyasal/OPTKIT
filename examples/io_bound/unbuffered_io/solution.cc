#include "solution.hh"
#include "constants.hh"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>
#include <cstring>

// Unbuffered I/O solution - forces immediate disk writes
void solution(const std::vector<char> &data, const std::string &filename)
{
    // Open file with O_SYNC to force unbuffered writes
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0644);
    if (fd == -1)
    {
        throw std::runtime_error("Failed to open file for unbuffered writing: " + std::string(strerror(errno)));
    }

    // Write data in chunks, each write goes directly to disk
    size_t total_written = 0;
    const char *buffer = data.data();
    size_t remaining = data.size();

    while (remaining > 0)
    {
        size_t chunk_size = std::min(remaining, static_cast<size_t>(UNBUFFERED_IO_CHUNK_SIZE));

        ssize_t written = write(fd, buffer + total_written, chunk_size);
        if (written == -1)
        {
            close(fd);
            throw std::runtime_error("Failed to write to file: " + std::string(strerror(errno)));
        }

        total_written += written;
        remaining -= written;
    }

    close(fd);
}