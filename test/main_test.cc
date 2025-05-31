#include <iostream>
#include "gtest/gtest.h"
#include "optkit.hh"

int main(int argc, char **argv) {
    
    OPTKIT_INIT({false});   // init optkit

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}