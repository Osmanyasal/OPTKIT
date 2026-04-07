#include "core/energy/pdu/query.hh"

#include <cstdlib>
#include <unistd.h>

#include "utils/deployment/deployment_config.hh"
#include "utils/utils.hh"

namespace
{
    static std::string env_or_default(const char *name, const std::string &fallback)
    {
        const char *value = std::getenv(name);
        if (!value || !*value)
            return fallback;
        return optkit::utils::str_trim(value);
    }

    static std::vector<int> parse_ports(const std::string &ports_str)
    {
        std::string normalized = ports_str;
        for (size_t index = 0; index < normalized.size(); ++index)
        {
            if (normalized[index] == '|')
                normalized[index] = ',';
        }

        std::vector<int> ports;
        std::vector<std::string> tokens = optkit::utils::str_split(normalized, ",");
        for (size_t index = 0; index < tokens.size(); ++index)
        {
            std::string token = optkit::utils::str_trim(tokens[index]);
            if (token.empty())
                continue;
            ports.push_back(std::atoi(token.c_str()));
        }
        return ports;
    }

    static std::vector<optkit::energy::pdu::PduEndpointInfo> parse_endpoints(const std::string &endpoints_env)
    {
        std::vector<optkit::energy::pdu::PduEndpointInfo> endpoints;
        if (endpoints_env.empty())
            return endpoints;

        std::vector<std::string> endpoint_specs = optkit::utils::str_split(endpoints_env, ";");
        for (size_t index = 0; index < endpoint_specs.size(); ++index)
        {
            const std::string entry = optkit::utils::str_trim(endpoint_specs[index]);
            if (entry.empty())
                continue;

            const size_t delimiter = entry.find(':');
            if (delimiter == std::string::npos)
                continue;

            optkit::energy::pdu::PduEndpointInfo endpoint;
            endpoint.ip = optkit::utils::str_trim(entry.substr(0, delimiter));
            endpoint.ports = parse_ports(entry.substr(delimiter + 1));
            if (!endpoint.ip.empty() && !endpoint.ports.empty())
                endpoints.push_back(endpoint);
        }

        return endpoints;
    }

    static std::string default_hostname()
    {
        char hostname[256];
        if (::gethostname(hostname, sizeof(hostname)) != 0)
            return "localhost";

        hostname[sizeof(hostname) - 1] = '\0';
        return std::string(hostname);
    }
}

namespace optkit::energy::pdu
{
    int32_t Query::avail_pdu_read_methods()
    {
        int32_t result = 0;
        if (is_pdu_snmp_avail())
            result = result | static_cast<int32_t>(PduReadMethods::SNMP);
        return result;
    }

    bool Query::is_pdu_snmp_avail()
    {
#if OPTKIT_ENV_LIB_NET_SNMP
        return is_configured();
#else
        return false;
#endif
    }

    bool Query::is_configured()
    {
        return !target_info().endpoints.empty();
    }

    const PduTargetInfo &Query::target_info()
    {
        static const PduTargetInfo info = []() {
            PduTargetInfo target;

            const std::string configured_label = env_or_default("OPTKIT_PDU_LABEL", OPTKIT_CONF_PDU_LABEL);
            target.label = configured_label.empty() ? default_hostname() : configured_label;
            target.community = env_or_default("OPTKIT_PDU_COMMUNITY", OPTKIT_CONF_PDU_COMMUNITY);
            target.power_oid = env_or_default("OPTKIT_PDU_POWER_OID", OPTKIT_CONF_PDU_POWER_OID);
            target.endpoints = parse_endpoints(env_or_default("OPTKIT_PDU_ENDPOINTS", OPTKIT_CONF_PDU_ENDPOINTS));
            return target;
        }();

        return info;
    }
}