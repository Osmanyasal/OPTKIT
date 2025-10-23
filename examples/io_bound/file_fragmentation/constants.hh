#ifndef CONSTANTS_HH
#define CONSTANTS_HH

// Constants for file fragmentation benchmark
constexpr int FRAGMENTED_FILE_COUNT = 1000;  // Number of small files
constexpr int SMALL_FILE_SIZE = 1024;        // 1KB per small file
constexpr int LARGE_FILE_COUNT = 1;          // Number of large files
constexpr int LARGE_FILE_SIZE = 1024 * 1024; // 1MB per large file
constexpr int BENCHMARK_ITERATIONS = 3;      // Number of benchmark runs
constexpr char FILL_CHAR = 'F';              // Character to fill files with

#endif // CONSTANTS_HH