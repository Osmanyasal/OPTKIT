#include <gtest/gtest.h>
#include "core/metrics/cpu/core_events.hh"

using namespace optkit::core::metrics::cpu;

TEST(CoreEventsTest, ToStringReturnsValidNameForAllKnownEvents)
{
    for (int i = static_cast<int>(CoreEvents::BEGIN) + 1; i < static_cast<int>(CoreEvents::END); ++i)
    {
        CoreEvents event = static_cast<CoreEvents>(i);
        std::string name = to_string(event);

        // Should never be the default unknown string for known events
        EXPECT_NE(name, "UNKNOWN_CORE_EVENT") << "Event index: " << i;

        // The returned string should be non-empty
        EXPECT_FALSE(name.empty()) << "Event index: " << i;

        // Optional: You could test output stream operator too
        std::stringstream ss;
        ss << event;
        EXPECT_EQ(ss.str(), name);
    }
}

TEST(CoreEventsTest, ToStringReturnsUnknownForOutOfRange)
{
    // Test some invalid enum values
    std::string unknown = to_string(static_cast<CoreEvents>(-1));
    EXPECT_EQ(unknown, "UNKNOWN_CORE_EVENT");

    unknown = to_string(static_cast<CoreEvents>(static_cast<int>(CoreEvents::END) + 10));
    EXPECT_EQ(unknown, "UNKNOWN_CORE_EVENT");
}