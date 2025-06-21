#include "gtest/gtest.h"
#include <algorithm>
#include <unistd.h> // for close()
#include <vector>

#include "core/pmu/cpu/pmu_event_manager.hh"

using namespace optkit::core::pmu::cpu;

class PMUEventManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        PMUEventManager::reset();
    }

    void TearDown() override
    {
        PMUEventManager::reset();
    }
};

// Helper: Create fake FDs (not actually valid OS FDs)
int get_fake_fd()
{
    static int next_fd = 1000;
    return next_fd++;
}

TEST_F(PMUEventManagerTest, RegisterAndUnregisterEvent)
{
    int fd = get_fake_fd();
    int num_events = 4;

    ASSERT_TRUE(PMUEventManager::register_event(fd, num_events));
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), num_events);

    int removed = PMUEventManager::unregister_event(fd);
    EXPECT_EQ(removed, num_events);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, RegisterDuplicateFdFails)
{
    int fd = get_fake_fd();

    EXPECT_TRUE(PMUEventManager::register_event(fd, 2));
    EXPECT_FALSE(PMUEventManager::register_event(fd, 5)); // Should fail, FD already registered

    EXPECT_EQ(PMUEventManager::unregister_event(fd), 2);
}

TEST_F(PMUEventManagerTest, UnregisterNonexistentFdReturnsZero)
{
    int nonexistent_fd = 99999;
    EXPECT_EQ(PMUEventManager::unregister_event(nonexistent_fd), 0); // Should be no-op
}

TEST_F(PMUEventManagerTest, AllFdsCorrectlyListed)
{
    int fd1 = get_fake_fd();
    int fd2 = get_fake_fd();

    PMUEventManager::register_event(fd1, 1);
    PMUEventManager::register_event(fd2, 2);

    std::vector<int32_t> fds = PMUEventManager::all_fds();
    EXPECT_EQ(fds.size(), 2);
    EXPECT_NE(std::find(fds.begin(), fds.end(), fd1), fds.end());
    EXPECT_NE(std::find(fds.begin(), fds.end(), fd2), fds.end());

    PMUEventManager::unregister_event(fd1);
    PMUEventManager::unregister_event(fd2);

    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, EventCountDoesNotUnderflow)
{
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);

    // Unregister an untracked FD - no change expected
    EXPECT_EQ(PMUEventManager::unregister_event(get_fake_fd()), 0);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, MultiplexingWarningLogicTriggered)
{
    const int32_t max_counters = PMUEventManager::pmu_num_cntrs();
    const int fd = get_fake_fd();
    const int excessive_events = max_counters + 5;

    EXPECT_TRUE(PMUEventManager::register_event(fd, excessive_events));
    EXPECT_GE(PMUEventManager::number_of_events_being_monitored(), excessive_events);

    PMUEventManager::unregister_event(fd);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, MultipleFdsTrackedAndUnregisteredCorrectly)
{
    int fd1 = get_fake_fd(), fd2 = get_fake_fd(), fd3 = get_fake_fd();

    PMUEventManager::register_event(fd1, 1);
    PMUEventManager::register_event(fd2, 2);
    PMUEventManager::register_event(fd3, 3);

    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 6);

    PMUEventManager::unregister_event(fd2);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 4);

    PMUEventManager::unregister_event(fd1);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 3);

    PMUEventManager::unregister_event(fd3);
    EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
}

TEST_F(PMUEventManagerTest, RepeatedRegisterUnregisterDoesNotLeak)
{
    int fd = get_fake_fd();
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_TRUE(PMUEventManager::register_event(fd, 2));
        EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 2);
        EXPECT_EQ(PMUEventManager::unregister_event(fd), 2);
        EXPECT_EQ(PMUEventManager::number_of_events_being_monitored(), 0);
    }
}
