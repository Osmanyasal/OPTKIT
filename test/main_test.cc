#include <iostream>
#include "gtest/gtest.h"
#include "common/module.hh"
#include "optkit.hh"

int main(int argc, char **argv)
{

    using namespace optkit::core::metrics;
    OPTKIT_INIT(false); // init optkit

    optkit::core::pmu::cpu::PMUEventManager::reset();

    {
        MetricBuilder mb{};
        mb.add(to_string(cpu::core_events::INST_RETIRED), cpu::event_mapper::get(cpu::core_events::INST_RETIRED))
            .add(to_string(cpu::core_events::BRANCH_MISP_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_MISP_RETIRED))
            .add(to_string(cpu::core_events::BRANCH_INST_RETIRED), cpu::event_mapper::get(cpu::core_events::BRANCH_INST_RETIRED))
            .add("DUMMY_EVENT", {0x0})
            .build("INST_RETIRED_AVG", [](const auto &map) -> double
                   { return map.at(to_string(cpu::core_events::INST_RETIRED)) / 20.0; })
            .build("BRANCH_MISP_RETIRED_AVG", [](const auto &map) -> double
                   { return map.at(to_string(cpu::core_events::BRANCH_MISP_RETIRED)) / 20.0; })
            .build("BRANCH_INST_RETIRED_AVG", [](const auto &map) -> double
                   { return map.at(to_string(cpu::core_events::BRANCH_INST_RETIRED)) / 20; });

        OPTKIT_CPU_EVENTS_REPEAT("instructions_million", mb, 20)
        {
            instructions_million();
        }
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}