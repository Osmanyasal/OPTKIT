#include "core/metrics/cpu/core_metrics.hh"

namespace optkit::core::metrics::cpu
{
    // Global to_string function
    std::string to_string(const MetricBuilder &mb)
    {
        std::ostringstream oss;
        oss << "MetricBuilder: " << mb.metric_name << "\n";
        oss << "Events:\n";
        for (std::vector<std::pair<std::string, uint64_t>>::const_iterator it = mb.metric_events.begin(); it != mb.metric_events.end(); ++it)
        {
            oss << "  " << it->first << " = 0x" << std::hex << it->second << std::dec << "\n";
        }
        return oss.str();
    }

    // Global operator<< overload
    std::ostream &operator<<(std::ostream &os, const MetricBuilder &mb)
    {
        return os << to_string(mb);
    }
}