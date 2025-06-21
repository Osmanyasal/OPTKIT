#include "core/metrics/metric_builder.hh"

namespace optkit::core::metrics
{
    // Global to_string function
    std::string to_string(const MetricBuilder &mb)
    {
        std::ostringstream oss;

        // Header summary
        oss << "MetricBuilder Summary:\n";
        oss << "  Total Events: " << mb.metric_events.size() << "\n";
        oss << "  Defined Metrics: " << mb.metric_names().size() << "\n\n";

        // List metric names
        oss << "Metrics:\n";
        for (const auto &name : mb.metric_names())
        {
            oss << "  - " << name << "\n";
        }

        // List event codes
        oss << "\nEvents:\n";
        for (const auto &pair : mb.metric_events)
        {
            oss << "  " << pair.first << " = 0x" << std::hex << pair.second << std::dec << "\n";
        }

        return oss.str();
    }

    // Global operator<< overload
    std::ostream &operator<<(std::ostream &os, const MetricBuilder &mb)
    {
        return os << to_string(mb);
    }
}