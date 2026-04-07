#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <ostream>
#include <thread>

#include "utils/base_profiler.hh"
#include "utils/metric_builder.hh"
#include "utils/utils.hh"
#include "core/energy/pdu/query.hh"

#if OPTKIT_ENV_LIB_NET_SNMP
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#endif

namespace optkit::energy::pdu
{
    class Profiler : public BaseProfiler<std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>>
    {
    public:
        Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb);
        virtual ~Profiler();

        virtual void disable() override;
        virtual void enable() override;
        virtual void reset() override {}
        virtual std::string to_json() override;
        virtual std::unordered_map<int32_t, std::unordered_map<PduDomain, double>> read() override;
        virtual std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>> aggregate() override;

    private:
        PduTargetInfo target;
        optkit::metrics::MetricBuilder<double> metric_builder;
        std::unordered_map<uint32_t, std::vector<std::pair<std::string, double>>> metric_results;
        std::thread sampling_thread;
        std::atomic<bool> is_sampling;
    };

    const optkit::metrics::MetricBuilder<double> &default_metrics();

    std::string to_string(const std::unordered_map<optkit::energy::pdu::PduDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::pdu::PduDomain, double> &map);
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::pdu::PduDomain, double>> &map);
}

using optkit::energy::pdu::operator<<;