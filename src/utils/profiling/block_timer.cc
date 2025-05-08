#include "utils/profiling/block_timer.hh"

namespace optkit::utils
{

    BlockTimer::BlockTimer(const std::string& block_name) : block_name{block_name}
    {
        OPTKIT_CORE_INFO("BLOCK :{} is being measured..", this->block_name);
        start = std::chrono::high_resolution_clock::now();
    }

    BlockTimer::~BlockTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
        OPTKIT_CORE_INFO("block :{} execution time : {}ms", this->block_name, duration);
    }

} // namespace optkit::utils
