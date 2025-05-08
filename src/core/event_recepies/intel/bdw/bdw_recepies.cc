
#include "core/event_recepies/intel/bdw/bdw_recepies.hh"

namespace optkit::core::recepies::intel::bdw
{
    const std::vector<std::pair<uint64_t, std::string>> Recepies::computational_intensity()
    {
        return {{optkit::intel::bdw::FP_ARITH_INST_RETIRED |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__SCALAR |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__SCALAR_SINGLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__SCALAR_DOUBLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__PACKED |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__128B_PACKED_DOUBLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__128B_PACKED_SINGLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__256B_PACKED_DOUBLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__256B_PACKED_SINGLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__SINGLE |
                     optkit::intel::bdw::FP_ARITH_INST_RETIRED__MASK__BDW_FP_ARITH__DOUBLE,
                 "fp_arith"},
                {optkit::intel::bdw::INSTRUCTIONS_RETIRED, "instructions_retired"},
                {optkit::intel::bdw::MEM_LOAD_UOPS_RETIRED | optkit::intel::bdw::MEM_LOAD_UOPS_RETIRED__MASK__BDW_MEM_LOAD_UOPS_RETIRED__L3_MISS, "l3_cache_miss"}};
    }
}