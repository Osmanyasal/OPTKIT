#include <gtest/gtest.h>
#include "core/pmu/cpu/pmu_event_manager.hh"

using namespace optkit::core::pmu::cpu;

class PMUEventManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean slate: Unregister all fake FDs used in test
        for (int fd : PMUEventManager::all_fds()) {
            PMUEventManager::unregister_event(fd);
        }
    }

    void TearDown() override {
        for (int fd : PMUEventManager::all_fds()) {
            PMUEventManager::unregister_event(fd);
        }
    }
};

TEST_F(PMUEventManagerTest, RegisterEventIncreasesFDCountAndEventCount) {
    int fake_fd = 1001;
    int num_events = 3;

    ASSERT_TRUE(PMUEventManager::register_event(fake_fd, num_events));
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), num_events);

    auto fds = PMUEventManager::all_fds();
    ASSERT_EQ(fds.size(), 1);
    EXPECT_EQ(fds[0], fake_fd);
}

TEST_F(PMUEventManagerTest, UnregisterEventReducesFDAndEventCount) {
    int fake_fd = 1002;
    int num_events = 2;

    ASSERT_TRUE(PMUEventManager::register_event(fake_fd, num_events));
    int unregistered = PMUEventManager::unregister_event(fake_fd);

    EXPECT_EQ(unregistered, num_events);
    EXPECT_TRUE(PMUEventManager::all_fds().empty());
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, DoubleUnregisterReturnsZero) {
    int fake_fd = 1003;
    ASSERT_TRUE(PMUEventManager::register_event(fake_fd, 1));

    PMUEventManager::unregister_event(fake_fd);
    int second_try = PMUEventManager::unregister_event(fake_fd);

    EXPECT_EQ(second_try, 0);
}

TEST_F(PMUEventManagerTest, MultipleFDsCorrectlyTracked) {
    ASSERT_TRUE(PMUEventManager::register_event(2001, 1));
    ASSERT_TRUE(PMUEventManager::register_event(2002, 2));
    ASSERT_TRUE(PMUEventManager::register_event(2003, 3));

    EXPECT_EQ(PMUEventManager::all_fds().size(), 3);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 6);

    PMUEventManager::unregister_event(2002);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 4);

    PMUEventManager::unregister_event(2001);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 3);

    PMUEventManager::unregister_event(2003);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}