#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

using namespace optkit::core::metrics;
using namespace optkit::core::metrics::disk;

constexpr int64_t WRITE_SIZE = 41298;
constexpr int64_t READ_SIZE = WRITE_SIZE;

class DiskProfiler : public ::testing::Test
{
protected:
    const std::string write_path = "bin/disk_test_write.dump";
    const std::string read_path = "bin/disk_test_read.dump";

    void SetUp() override
    {
        // Clean files before each test
        std::remove(write_path.c_str());
        std::remove(read_path.c_str());
    }

    void TearDown() override
    {
        // Clean files after each test
        std::remove(write_path.c_str());
        std::remove(read_path.c_str());
    }
};

TEST_F(DiskProfiler, Write82KCharsAtOnce)
{
    MetricBuilder mb{};
    mb.add(core_metrics::AllMetrics());

    OPTKIT_DISK_EVENTS("Write82KCharsAtOnce", mb);

    optkit::utils::write_file(write_path, std::string(WRITE_SIZE * 2, 'M'));

    var37.read_and_store();
    const auto &result = var37.aggregate();
    EXPECT_NEAR(result.at(to_string(disk::core_events::WCHAR)), WRITE_SIZE * 2, WRITE_SIZE * 2 * ERROR_RATE);
}

TEST_F(DiskProfiler, Write82KCharsDivided)
{
    MetricBuilder mb{};
    mb.add(core_metrics::AllMetrics());

    OPTKIT_DISK_EVENTS("Write82KCharsDivided", mb);

    optkit::utils::write_file(write_path, std::string(WRITE_SIZE, 'A'));
    optkit::utils::write_file(write_path, std::string(WRITE_SIZE, 'B'));

    var51.read_and_store();
    const auto &result = var51.aggregate();
    EXPECT_NEAR(result.at(to_string(disk::core_events::WCHAR)), WRITE_SIZE * 2, WRITE_SIZE * 2 * ERROR_RATE);
}

TEST_F(DiskProfiler, Read41KChars)
{
    optkit::utils::write_file(read_path, std::string(WRITE_SIZE, 'X'));

    MetricBuilder mb{};
    mb.add(core_metrics::AllMetrics());

    OPTKIT_DISK_EVENTS("Read41KChars", mb);
    {
        std::string data = optkit::utils::read_file(read_path);
        ASSERT_FALSE(data.empty());
    }
    var68.read_and_store();
    const auto result = var68.aggregate();

    EXPECT_NEAR(result.at(to_string(disk::core_events::RCHAR)), READ_SIZE, READ_SIZE * ERROR_RATE);
}