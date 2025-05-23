#pragma once

#include <vector>
#include <string>
#include "core/pmu/cpu/perf/events/amd64/fam19h_zen3.hh"
#include "core/pmu/cpu/perf/events/amd64/fam19h_zen3_l3.hh"

namespace optkit::core::metrics::amd64::zen3
{
    class Recepies final
    {
    public:
        static const std::vector<std::pair<uint64_t, std::string>> computational_intensity();

    private:
        Recepies() = delete;
        ~Recepies() = delete;
    };
}
