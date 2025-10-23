#ifndef SOLUTION_PATCH_HH
#define SOLUTION_PATCH_HH

#include <vector>
#include <string>

// Optimized approach - creates fewer large files with same total data
void solution_patch(const std::vector<std::string> &data_chunks, const std::string &base_dir);

#endif // SOLUTION_PATCH_HH