#include "gtest/gtest.h"
#include "core/pmu/cpu/query_pmu.hh"

using namespace optkit::core::pmu::cpu;

TEST(QueryPMUTest, AvailablePMUIdsNotEmpty) {
    auto ids = QueryPMU::avail_pmu_ids();
    EXPECT_FALSE(ids.empty());
}

TEST(QueryPMUTest, DefaultPMUisPresent) {
    pfm_pmu_info_t info = QueryPMU::default_pmu_info();
    EXPECT_EQ(info.is_present, 1);
    EXPECT_GT(strlen(info.name), 0);
}

TEST(QueryPMUTest, EventDetailReturnsValidInfoForEachPMU) {
    auto ids = QueryPMU::avail_pmu_ids();
    ASSERT_FALSE(ids.empty()) << "No PMU IDs available on the system.";

    for (int32_t pmu_id : ids) {
        pfm_event_info_t event_info;
        ASSERT_NO_FATAL_FAILURE(event_info = QueryPMU::event_detail(pmu_id, 0))
            << "Failed to get event detail for PMU ID: " << pmu_id;

        EXPECT_GT(strlen(event_info.name), 0)
            << "Event name is empty for PMU ID: " << pmu_id;
    }
}

TEST(QueryPMUTest, PmuInfoMatchesDefault) {
    pfm_pmu_info_t default_info = QueryPMU::default_pmu_info();
    pfm_pmu_info_t queried_info = QueryPMU::pmu_info(default_info.pmu);

    EXPECT_STREQ(default_info.name, queried_info.name);
    EXPECT_EQ(default_info.pmu, queried_info.pmu);
}


TEST(QueryPMUTest, EachPMUHasEvents) {
    auto ids = QueryPMU::avail_pmu_ids();
    ASSERT_FALSE(ids.empty()) << "No PMU IDs available on this system.";

    for (int32_t pmu_id : ids) {
        pfm_pmu_info_t info = QueryPMU::pmu_info(pmu_id);
        ASSERT_GT(info.nevents, 0) << "PMU ID " << pmu_id << " has no events.";
    }
}