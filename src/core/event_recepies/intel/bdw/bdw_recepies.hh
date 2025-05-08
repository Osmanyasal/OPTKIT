#pragma once

#include <vector>
#include <string>
#include "core/events/intel/bdw.hh"

namespace optkit::core::recepies::intel::bdw
{
    class Recepies final
    {
    public:
        static const std::vector<std::pair<uint64_t, std::string>> computational_intensity();
        // static const std::vector<std::pair<uint64_t, std::string>> computational_intensity();

    private:
        Recepies() = delete;
        ~Recepies() = delete;
    };
}
