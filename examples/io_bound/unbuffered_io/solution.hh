#ifndef SOLUTION_HH
#define SOLUTION_HH

#include <vector>
#include <string>

// Unbuffered I/O approach - uses direct syscalls with O_SYNC
void solution(const std::vector<char> &data, const std::string &filename);

#endif // SOLUTION_HH