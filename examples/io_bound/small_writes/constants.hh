#ifndef CONSTANTS_HH
#define CONSTANTS_HH

// Constants for small writes benchmark
constexpr int SMALL_WRITES_COUNT = 100000; // Number of small write operations
constexpr int SMALL_WRITE_SIZE = 64;       // 64 bytes per small write
constexpr int LARGE_WRITE_SIZE = 65536;    // 64KB batched write size
constexpr int BENCHMARK_ITERATIONS = 3;    // Number of benchmark runs
constexpr char FILL_CHAR = 'A';            // Character to fill data with

#endif // CONSTANTS_HH