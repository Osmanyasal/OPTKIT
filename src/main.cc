#include <omp.h>
#include "optkit.hh"

using namespace optkit::core::metrics;
int32_t main(int32_t argc, char **argv)
{
    OPTKIT_INIT();

    OPTKIT_DISK_EVENTS("main", disk::core_metrics::AllMetrics());
    std::ofstream out("example.bin");
    out << std::string(82596, 'A');
    out.close();

    std::ifstream in("example.bin");
    char buf[4096];
    in.read(buf, sizeof(buf));
    buf[4095] = '\0';
    std::cout << buf << "\n";
    in.close();
    return 0;
}
