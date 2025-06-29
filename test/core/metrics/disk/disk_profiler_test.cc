#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "optkit.hh"

using namespace optkit::core::disk;
using namespace optkit::core::metrics;


TEST(IoDiskProfilerTest, MeasuresFileReadWriteEventsConsistently)
{
    MetricBuilder mb{};
    mb.add(to_string(CoreEvents::RCHAR), {0x0});
    mb.add(to_string(CoreEvents::READ_BYTES), {0x0});
    mb.add(to_string(CoreEvents::WRITE_BYTES), {0x0});
    mb.add(to_string(CoreEvents::SYSCR), {0x0});
    mb.add(to_string(CoreEvents::SYSCW), {0x0});
    mb.add(to_string(CoreEvents::WCHAR), {0x0});
    mb.add(to_string(CoreEvents::CANCELLED_WRITE_BYTES), {0x0});

    // Use the macro to create a profiler named 'profiler_var'
    OPTKIT_DISK_EVENTS("file_block_test", mb);

    double total_write_bytes = 0;
    double total_read_bytes = 0;

    // Repeat some file IO and measure deltas
    for (int i = 0; i < 5; i++)
    {
        // Write some data to a temporary file
        std::ofstream ofs("bin/test_io_disk_profiler.tmp");
        ofs << std::string(50 * 1024, 'x'); // write 50 KB
        ofs.close();

        // Read from a common system file (e.g. /etc/hostname or /proc/version)
        std::ifstream ifs("/etc/hostname");
        char buf[4096];
        ifs.read(buf, sizeof(buf));
        ifs.close();

        // Accumulate deltas for write_bytes and read_bytes
        auto deltas = var22.read_and_store().second;
        auto event_names = mb.event_names();

        for (size_t j = 0; j < event_names.size(); j++)
        {
            if (event_names[j] == "write_bytes")
                total_write_bytes += deltas[j];
            else if (event_names[j] == "read_bytes")
                total_read_bytes += deltas[j];
        }
    }

    // Aggregate the buffered results
    auto agg = var22.aggregate();

    // Compare accumulated per-iteration totals with aggregate results
    double agg_write = static_cast<double>(agg["write_bytes"]);
    double agg_read = static_cast<double>(agg["read_bytes"]);

    EXPECT_NEAR(total_write_bytes, agg_write, agg_write * 0.05) << "Write bytes mismatch";
    EXPECT_NEAR(total_read_bytes, agg_read, agg_read * 0.05) << "Read bytes mismatch";

    // Cleanup temporary file
    std::remove("bin/test_io_disk_profiler.tmp");
}