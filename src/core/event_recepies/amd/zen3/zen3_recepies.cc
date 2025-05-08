
#include "core/event_recepies/amd/zen3/zen3_recepies.hh"

namespace optkit::core::recepies::amd64::zen3
{
    const std::vector<std::pair<uint64_t, std::string>> Recepies::computational_intensity()
    {
        return {{optkit::amd64::fam19h_zen3::RETIRED_SSE_AVX_FLOPS |
                     optkit::amd64::fam19h_zen3::RETIRED_SSE_AVX_FLOPS__MASK__AMD64_FAM19H_ZEN3_RETIRED_SSE_AVX_FLOPS__ANY,
                 "fp_arith"},
                {optkit::amd64::fam19h_zen3::RETIRED_INSTRUCTIONS, "instructions_retired"},
                {optkit::amd64::fam19h_zen3::L2_PREFETCH_MISS_L3 | optkit::amd64::fam19h_zen3::L2_PREFETCH_MISS_L3__MASK__AMD64_FAM19H_ZEN3_L2_PREFETCH_HIT_L2__L2_HW_PREFETCHER, "l3_cache_miss"}};
    }
}