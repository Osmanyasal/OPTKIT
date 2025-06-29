#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "utils/utils.hh"

namespace fs = std::filesystem;
using namespace optkit::utils;

// Define a temporary folder to store test artifacts
const std::string test_folder = "bin/test_output";

// Ensure setup and cleanup for tests
class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!fs::exists(test_folder)) {
            fs::create_directory(test_folder);
        }
    }

    void TearDown() override {
        fs::remove_all(test_folder);
    }
};

TEST_F(UtilsTest, CreateAndCheckDirectory) {
    std::string folder = test_folder + "/subfolder";
    create_directory(folder);
    EXPECT_TRUE(is_path_exists(folder));
}

TEST_F(UtilsTest, WriteAndReadFile) {
    std::string file_path = test_folder + "/sample.txt";
    std::string content = "Hello World";

    write_file(file_path, content);
    std::string read_content = read_file(file_path);

    EXPECT_NE(read_content.find(content), std::string::npos);
}

TEST_F(UtilsTest, GetAllFilesInDirectory) {
    std::string file1 = test_folder + "/a.txt";
    std::string file2 = test_folder + "/b.txt";

    write_file(file1, "A");
    write_file(file2, "B");

    auto files = get_all_files(test_folder);
    EXPECT_GE(files.size(), 2);

    bool found_a = false, found_b = false;
    for (auto& f : files) {
        if (f.find("a.txt") != std::string::npos) found_a = true;
        if (f.find("b.txt") != std::string::npos) found_b = true;
    }
    EXPECT_TRUE(found_a);
    EXPECT_TRUE(found_b);
}

TEST_F(UtilsTest, GenerateGUID_Unique) {
    auto guid1 = generateGUID();
    auto guid2 = generateGUID();
    EXPECT_FALSE(guid1.empty());
    EXPECT_FALSE(guid2.empty());
    EXPECT_NE(guid1, guid2);
}

TEST_F(UtilsTest, GetDateAndTimeFormat) {
    auto date = get_date();
    auto time = get_time();

    EXPECT_FALSE(date.empty());
    EXPECT_FALSE(time.empty());
}

TEST_F(UtilsTest, StringSplitBasic) {
    std::string input = "a,b,c";
    auto result = str_split(input, ",");

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}
