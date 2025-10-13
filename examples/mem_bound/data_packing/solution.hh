#pragma once

#include <vector>
#include "constants.hh"

// FIXME: this data structure can be reduced in size
struct S
{
    int i;
    long long l;
    short s;
    double d;
    bool b;

    bool operator<(const S &s) const { return this->i < s.i; }
};
