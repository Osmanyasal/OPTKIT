#include <iostream>
#include "core/event_recepies/tma_metrics.hh"

namespace optkit::core::recepies
{
    // Function to convert L1Metric to string
    std::string to_string(L1Metric metric)
    {
        switch (metric)
        {
        case L1Metric::Default:
            return "L1 Analysis";
        case L1Metric::IPC:
            return "IPC";
        case L1Metric::CPU_UTIL:
            return "CPU UTIL";
        case L1Metric::BackendBound:
            return "L1 Backend Bound";
        case L1Metric::BadSpeculation:
            return "L1 Bad Speculation";
        case L1Metric::Retiring:
            return "L1 Retiring";
        case L1Metric::FrontendBound:
            return "L1 Frontend Bound";
        default:
            return "Unknown L1 Metric";
        }
    }

    // Function to convert L2Metric to string
    std::string to_string(L2Metric metric)
    {
        switch (metric)
        {
        case L2Metric::Default:
            return "L2 Analysis";
        case L2Metric::FetchLatency:
            return "L2 Fetch Latency";
        case L2Metric::FetchBandwidth:
            return "L2 Fetch Bandwidth";
        case L2Metric::BranchMisprediction:
            return "L2 Branch Misprediction";
        case L2Metric::MachineClear:
            return "L2 Machine Clear";
        case L2Metric::Base:
            return "L2 Base";
        case L2Metric::MicroSequencer:
            return "L2 Micro Sequencer";
        case L2Metric::CoreBound:
            return "L2 Core Bound";
        case L2Metric::MemoryBound:
            return "L2 Memory Bound";
        default:
            return "Unknown L2 Metric";
        }
    }

    // Function to convert L2Metric to string
    std::string to_string(L3Metric metric)
    {
        switch (metric)
        {
        case L3Metric::Default:
            return "L3 Analysis";
        case L3Metric::ITLBMiss:
            return "L3 ITLB Miss";
        case L3Metric::ICacheMiss:
            return "L3 ICache Miss";
        case L3Metric::BranchRegisters:
            return "L3 Branch Registers";
        case L3Metric::FetchSrc1:
            return "L3 FetchSrc1";
        case L3Metric::FetchSrc2:
            return "L3 FetchSrc2";
        case L3Metric::FPArith:
            return "L3 FPArith";
        case L3Metric::Divider:
            return "L3 Divider";
        case L3Metric::ExecutionPortUtilisation:
            return "L3 Execution Port Utilisation";
        case L3Metric::StoresBound:
            return "L3 Stores Bound";
        case L3Metric::L1Bound:
            return "L3 L1Bound";
        case L3Metric::L2Bound:
            return "L3 L2Bound";
        case L3Metric::L3Bound:
            return "L3 L3Bound";
        case L3Metric::ExtMemoryBound:
            return "L3 External Memory Bound";
        default:
            return "Unknown L3 Metric";
        }
    }

    // Overloading the << operator for L1Metric and L2Metric unordered maps
    std::ostream &operator<<(std::ostream &out, const std::map<L1Metric, double> &map)
    {
        out << "{";
        bool first = true;
        for (const auto &pair : map)
        {
            if (!first)
            {
                out << ", ";
            }
            first = false;
            out << to_string(pair.first) << " = " << pair.second;
        }
        out << "}";
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const std::map<L2Metric, double> &map)
    {
        out << "{";
        bool first = true;
        for (const auto &pair : map)
        {
            if (!first)
            {
                out << ", ";
            }
            first = false;
            out << to_string(pair.first) << " = " << pair.second;
        }
        out << "}";
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const std::map<L3Metric, double> &map)
    {
        out << "{";
        bool first = true;
        for (const auto &pair : map)
        {
            if (!first)
            {
                out << ", ";
            }
            first = false;
            out << to_string(pair.first) << " = " << pair.second;
        }
        out << "}";
        return out;
    }

    nlohmann::json to_json(const char *event_name, const std::map<L1Metric, double> &map)
    {
        nlohmann::json result;

        return result;
    }
    nlohmann::json to_json(const char *event_name, const std::map<L2Metric, double> &map)
    {
        nlohmann::json result;

        return result;
    }
    nlohmann::json to_json(const char *event_name, const std::map<L3Metric, double> &map)
    {
        nlohmann::json result;

        return result;
    }
}