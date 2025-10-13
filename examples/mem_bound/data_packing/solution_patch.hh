#pragma once

#include <vector>
#include "constants.hh"

struct S_patch
{
    unsigned i : 7;
    unsigned l : 14;
    unsigned s : 7;
    unsigned b : 1;
    float d;

    bool operator<(const S_patch &s) const { return this->i < s.i; }
};