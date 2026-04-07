#include "core/energy/pdu/pdu.hh"

namespace optkit::energy::pdu
{
    const std::unordered_map<int32_t, std::string> pdu_domain_name_mapping = {
        {static_cast<int32_t>(PduDomain::BEGIN), "begin"},
        {static_cast<int32_t>(PduDomain::NODE_ENERGY), "energy-pdu"},
        {static_cast<int32_t>(PduDomain::END), "end"},
        {static_cast<int32_t>(PduDomain::ALL), "All domains"}};

    const std::unordered_map<int32_t, std::string> pdu_read_method_name_mapping = {
        {static_cast<int32_t>(PduReadMethods::SNMP), "snmp"}};

    PduDomain metric_name_to_pdu_domain(const std::string &metric_name)
    {
        for (std::unordered_map<int32_t, std::string>::const_iterator it = pdu_domain_name_mapping.begin(); it != pdu_domain_name_mapping.end(); ++it)
        {
            if (it->second == metric_name)
                return static_cast<PduDomain>(it->first);
        }

        OPTKIT_CORE_WARN("Unknown PDU metric_name: {}", metric_name);
        return PduDomain::BEGIN;
    }

    std::string to_string(const optkit::energy::pdu::PduDomain &domain)
    {
        std::ostringstream oss;
        oss << domain;
        return oss.str();
    }

    std::string to_string(const optkit::energy::pdu::PduReadMethods &read_method)
    {
        std::ostringstream oss;
        oss << read_method;
        return oss.str();
    }

    std::string to_string(const optkit::energy::pdu::PduEndpointInfo &endpoint)
    {
        std::ostringstream oss;
        oss << endpoint;
        return oss.str();
    }

    std::string to_string(const optkit::energy::pdu::PduTargetInfo &target)
    {
        std::ostringstream oss;
        oss << target;
        return oss.str();
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduDomain &domain)
    {
        switch (domain)
        {
        case optkit::energy::pdu::PduDomain::NODE_ENERGY:
            os << "energy-pdu";
            break;
        default:
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduReadMethods &read_method)
    {
        switch (read_method)
        {
        case optkit::energy::pdu::PduReadMethods::SNMP:
            os << "snmp";
            break;
        default:
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduEndpointInfo &endpoint)
    {
        os << endpoint.ip << ":";
        for (size_t index = 0; index < endpoint.ports.size(); ++index)
        {
            if (index != 0)
                os << ",";
            os << endpoint.ports[index];
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const optkit::energy::pdu::PduTargetInfo &target)
    {
        os << "Label=" << target.label << ", "
           << "Community=" << target.community << ", "
           << "PowerOID=" << target.power_oid << ", Endpoints=[";

        for (size_t index = 0; index < target.endpoints.size(); ++index)
        {
            if (index != 0)
                os << "; ";
            os << target.endpoints[index];
        }
        os << "]";
        return os;
    }
}