#pragma once

#include <cstring>
#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <atomic>
#include <omp.h>

std::size_t solution_patch(const std::vector<uint32_t> &data, int thread_count);