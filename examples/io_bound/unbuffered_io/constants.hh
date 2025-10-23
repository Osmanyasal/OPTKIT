#ifndef CONSTANTS_HH
#define CONSTANTS_HH

// Constants for unbuffered I/O benchmark
constexpr int UNBUFFERED_IO_N = 10000;         // Number of write operations
constexpr int UNBUFFERED_IO_CHUNK_SIZE = 4096; // 4KB chunks (page size)
constexpr int BENCHMARK_ITERATIONS = 5;        // Number of benchmark runs
constexpr int UNBUFFERED_IO_MIN_VALUE = 1;     // Min data value
constexpr int UNBUFFERED_IO_MAX_VALUE = 255;   // Max data value

#endif // CONSTANTS_HH