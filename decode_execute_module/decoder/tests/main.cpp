#include <vector>
#include <gtest/gtest.h>
#include <filesystem>
#include <sstream> 

#include "decoder_test.hpp"

//-----------------------------------------------------------------------------------------

int main (int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    int ret_val = RUN_ALL_TESTS();
    return ret_val;
}
