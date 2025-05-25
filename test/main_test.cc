#include <iostream>
#include "gtest/gtest.h"
#include "optkit.hh"


TEST(OPTKIT, INIT_OPTKIT)
{
    OPTKIT_INIT();
    OPTKIT_RAPL(rapl,"main");

    sleep(3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}