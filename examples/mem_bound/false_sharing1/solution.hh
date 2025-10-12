#pragma once

#include <cstring>
#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <atomic>
#include <omp.h>

std::size_t solution(const std::vector<uint32_t> &data, int thread_count);