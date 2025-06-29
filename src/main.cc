#include <omp.h>
#include "optkit.hh"
#include "core/disk/disk_profiler.hh"
using namespace optkit::core::metrics;
int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();

    MetricBuilder mb{};
    mb.add(to_string(optkit::core::disk::CoreEvents::RCHAR), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::READ_BYTES), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::WRITE_BYTES), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::SYSCR), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::SYSCW), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::WCHAR), {0x0});
    mb.add(to_string(optkit::core::disk::CoreEvents::CANCELLED_WRITE_BYTES), {0x0});
    optkit::core::disk::IoDiskProfiler profiler("file_block", mb);

    std::ofstream out("example.bin");
    out << std::string(82596, 'A');
    out.close();

    std::ifstream in("/etc/hostname");
    char buf[5002];
    in.read(buf, sizeof(buf));
    return 0;
}
