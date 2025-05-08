#pragma once

#include <iostream>
#include <map>

#include "utils/json.hh"
// Enum for L1, L2 and L3 analysis metrics

namespace optkit::core::recepies
{
    enum class L1Metric : char
    {
        Default,
        CPU_UTIL,
        IPC,
        FrontendBound,
        BadSpeculation,
        Retiring,
        BackendBound,
    };

    enum class L2Metric : char
    {
        Default,
        // FrontendBound
        FetchLatency,
        FetchBandwidth,

        // BadSpec.
        BranchMisprediction,
        MachineClear,

        // Retiring
        Base,
        MicroSequencer,

        // BackendBound
        CoreBound,
        MemoryBound,
    };

    enum class L3Metric : char
    {
        Default,
        // FrontendBound.FetchLatency
        ITLBMiss,
        ICacheMiss,
        BranchRegisters,

        // FrontendBound.FetchBandwidth
        FetchSrc1,
        FetchSrc2,

        // Retiring.Base
        FPArith,

        // Backend.CoreBound
        Divider,
        ExecutionPortUtilisation,

        // Backend.MemoryBound
        StoresBound,
        L1Bound,
        L2Bound,
        L3Bound,
        ExtMemoryBound,
    };

    std::string to_string(L1Metric metric);
    std::string to_string(L2Metric metric);
    std::string to_string(L3Metric metric);

    // Overloading the << operator for L1Metric and L2Metric unordered maps
    std::ostream &operator<<(std::ostream &out, const std::map<L1Metric, double> &map);
    std::ostream &operator<<(std::ostream &out, const std::map<L2Metric, double> &map);
    std::ostream &operator<<(std::ostream &out, const std::map<L3Metric, double> &map);

    nlohmann::json to_json(const char *event_name, const std::map<L1Metric, double> &map);
    nlohmann::json to_json(const char *event_name, const std::map<L2Metric, double> &map);
    nlohmann::json to_json(const char *event_name, const std::map<L3Metric, double> &map);
}