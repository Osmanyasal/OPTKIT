#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

/**
 * @brief In case of in-consistancy, errors on your system, open a ticket at: https://github.com/Osmanyasal/OPTKIT/issues
 */
using namespace optkit::core::metrics;
int main(int argc, char **argv)
{
    OPTKIT_INIT(false); // init optkit
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}