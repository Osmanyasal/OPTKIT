#pragma once

#include <array>
#include <cstdint>
#include "constants.hh"

// Square matrix 400 x 400
using Matrix = std::array<std::array<float, LOOP_INTERCHANGE1_N>, LOOP_INTERCHANGE1_N>;

void zero(Matrix &result);
void identity(Matrix &result);
void multiply_patch(Matrix &result, const Matrix &a, const Matrix &b);
Matrix power_patch(const Matrix &input, const uint32_t k);

void init(Matrix &matrix);