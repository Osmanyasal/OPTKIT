#ifndef SOLUTION_PATCH_HH
#define SOLUTION_PATCH_HH

#include <vector>
#include <string>

// Optimized batched writes approach - accumulate and write in larger chunks
void solution_patch(const std::vector<std::string> &data_chunks, const std::string &filename);

#endif // SOLUTION_PATCH_HH