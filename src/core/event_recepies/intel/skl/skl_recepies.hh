#pragma once

#include <vector>
#include <string>
#include "core/events/intel/skl.hh"

namespace optkit::core::recepies::intel::skl
{
    class Recepies final
    {
    public:
        static const std::vector<std::pair<uint64_t, std::string>> freq_governor_events();

    private:
        Recepies() = delete;
        ~Recepies() = delete;
    };
}
