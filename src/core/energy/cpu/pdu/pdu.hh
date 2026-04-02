#pragma once

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/logging/logger.hh"

namespace optkit::energy::pdu
{
    enum class PduDomain
    {
        BEGIN = 0,
        NODE_ENERGY = (1 << 0),
        END = (1 << 1),
        ALL = 0x1,
    };

    enum class PduReadMethods
    {
        SNMP = (1 << 0),
    };

    struct PduEndpointInfo
    {
        std::string ip;
        std::vector<int> ports;
    };

    struct PduTargetInfo
    {
        std::string label;
        std::string community;
        std::string power_oid;
        std::vector<PduEndpointInfo> endpoints;
    };

    extern const std::unordered_map<int32_t, std::string> pdu_domain_name_mapping;
    extern const std::unordered_map<int32_t, std::string> pdu_read_method_name_mapping;

    PduDomain metric_name_to_pdu_domain(const std::string &metric_name);

    std::string to_string(const optkit::energy::pdu::PduDomain &domain);
    std::string to_string(const optkit::energy::pdu::PduReadMethods &read_method);
    std::string to_string(const optkit::energy::pdu::PduEndpointInfo &endpoint);
    std::string to_string(const optkit::energy::pdu::PduTargetInfo &target);

    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduDomain &domain);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduReadMethods &read_method);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduEndpointInfo &endpoint);
    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduTargetInfo &target);
}

using optkit::energy::pdu::operator<<;