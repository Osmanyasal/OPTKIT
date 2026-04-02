#include "core/energy/cpu/pdu/profiler.hh"

#include <cstdlib>
#include <cstring>

namespace
{
#if OPTKIT_ENV_LIB_NET_SNMP
    static double snmp_get_value(const std::string &ip, const std::string &community, const std::string &oid_str)
    {
        static bool snmp_initialized = false;
        if (!snmp_initialized)
        {
            init_snmp("optkit-pdu");
            snmp_initialized = true;
        }

        struct snmp_session session;
        struct snmp_session *ss = nullptr;
        struct snmp_pdu *pdu = nullptr;
        struct snmp_pdu *response = nullptr;
        oid anOID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        double result = 0.0;

        snmp_sess_init(&session);
        session.peername = strdup(ip.c_str());
        session.version = SNMP_VERSION_1;
        session.community = reinterpret_cast<u_char *>(const_cast<char *>(community.c_str()));
        session.community_len = community.size();

        SOCK_STARTUP;
        ss = snmp_open(&session);
        if (!ss)
        {
            snmp_perror("snmp_open");
            SOCK_CLEANUP;
            free(session.peername);
            return 0.0;
        }

        if (!read_objid(oid_str.c_str(), anOID, &anOID_len))
        {
            snmp_perror("read_objid");
            snmp_close(ss);
            SOCK_CLEANUP;
            free(session.peername);
            return 0.0;
        }

        pdu = snmp_pdu_create(SNMP_MSG_GET);
        snmp_add_null_var(pdu, anOID, anOID_len);

        const int status = snmp_synch_response(ss, pdu, &response);
        if (status == STAT_SUCCESS && response && response->errstat == SNMP_ERR_NOERROR)
        {
            for (struct variable_list *vars = response->variables; vars; vars = vars->next_variable)
            {
                if (vars->type == ASN_INTEGER || vars->type == ASN_GAUGE || vars->type == ASN_COUNTER || vars->type == ASN_TIMETICKS || vars->type == ASN_UINTEGER)
                    result = static_cast<double>(*vars->val.integer);
                else if (vars->type == ASN_COUNTER64)
                    result = static_cast<double>(vars->val.counter64->high) * 4294967296.0 + static_cast<double>(vars->val.counter64->low);
            }
        }

        if (response)
            snmp_free_pdu(response);
        snmp_close(ss);
        SOCK_CLEANUP;
        free(session.peername);

        return result;
    }
#endif

    static void sampling_function(optkit::energy::pdu::Profiler &profiler)
    {
        profiler.read_and_store();
    }
}

namespace optkit::energy::pdu
{
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb)
        : BaseProfiler{profiler_config}, target{Query::target_info()}, metric_builder{mb}
    {
        if (!Query::is_pdu_snmp_avail())
            throw std::runtime_error("PDU backend is not available or not configured. Set OPTKIT_PDU_ENDPOINTS.");

        if (OPT_UNLIKELY(this->config.is_sampling))
        {
            this->sampling_thread = std::thread([this]() {
                this->is_sampling = true;
                while (this->is_sampling)
                {
                    sampling_function(*this);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            });
        }
    }

    Profiler::~Profiler()
    {
        if (this->config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

        read_and_store();

        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>> aggregated = aggregate();
        for (std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>>::const_iterator aggr_item = aggregated.begin(); aggr_item != aggregated.end(); ++aggr_item)
        {
            std::vector<std::pair<std::string, double>> event_values;
            for (std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>::const_iterator socket_it = aggr_item->second.begin(); socket_it != aggr_item->second.end(); ++socket_it)
            {
                for (std::unordered_map<PduDomain, double>::const_iterator domain_it = socket_it->second.begin(); domain_it != socket_it->second.end(); ++domain_it)
                    event_values.push_back(std::make_pair(to_string(domain_it->first), domain_it->second));
            }

            event_values.push_back(std::make_pair("duration_microsec", this->total_duration_ms * 1000.0));
            this->metric_results[std::strtol(aggr_item->first.c_str(), nullptr, 10)] =
                this->metric_builder.calculate(std::unordered_map<std::string, double>(event_values.begin(), event_values.end()));
        }

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();
    }

    void Profiler::disable()
    {
        this->is_enabled = false;
    }

    void Profiler::enable()
    {
        this->is_enabled = true;
    }

    std::unordered_map<int32_t, std::unordered_map<PduDomain, double>> Profiler::read()
    {
        std::unordered_map<int32_t, std::unordered_map<PduDomain, double>> result;
        if (!this->is_enabled)
            return result;

        double total_power = 0.0;

#if OPTKIT_ENV_LIB_NET_SNMP
        for (size_t endpoint_index = 0; endpoint_index < target.endpoints.size(); ++endpoint_index)
        {
            const PduEndpointInfo &endpoint = target.endpoints[endpoint_index];
            for (size_t port_index = 0; port_index < endpoint.ports.size(); ++port_index)
            {
                const std::string oid = target.power_oid + "." + std::to_string(endpoint.ports[port_index]);
                total_power += snmp_get_value(endpoint.ip, target.community, oid);
            }
        }
#endif

        result[0][PduDomain::NODE_ENERGY] = total_power;
        return result;
    }

    std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>> aggregated_events;

        for (size_t index = 0; index < read_buffer.size(); ++index)
        {
            const double duration_ms = read_buffer[index].first;
            total_duration += duration_ms;
            const double duration_s = duration_ms / 1000.0;
            const std::unordered_map<int32_t, std::unordered_map<PduDomain, double>> &power_values = read_buffer[index].second;

            for (std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>::const_iterator socket_it = power_values.begin(); socket_it != power_values.end(); ++socket_it)
            {
                for (std::unordered_map<PduDomain, double>::const_iterator domain_it = socket_it->second.begin(); domain_it != socket_it->second.end(); ++domain_it)
                    aggregated_events[std::to_string(socket_it->first)][socket_it->first][domain_it->first] += domain_it->second * duration_s;
            }
        }

        std::vector<std::pair<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>>> event_value(aggregated_events.begin(), aggregated_events.end());
        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        return aggregated_events;
    }

    std::string Profiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        bool first = true;

        for (std::vector<std::pair<std::string, std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>>>::const_iterator event_pair = this->event_results.begin(); event_pair != this->event_results.end(); ++event_pair)
        {
            const int32_t socket_id = std::stoi(event_pair->first);
            std::vector<std::pair<std::string, double>> event_values;
            for (std::unordered_map<int32_t, std::unordered_map<PduDomain, double>>::const_iterator socket_pair = event_pair->second.begin(); socket_pair != event_pair->second.end(); ++socket_pair)
            {
                for (std::unordered_map<PduDomain, double>::const_iterator domain_pair = socket_pair->second.begin(); domain_pair != socket_pair->second.end(); ++domain_pair)
                    event_values.push_back(std::make_pair(to_string(domain_pair->first) + "__Joules", domain_pair->second));
            }

            std::vector<std::pair<std::string, double>> metric_values;
            std::unordered_map<uint32_t, std::vector<std::pair<std::string, double>>>::const_iterator metric_it = this->metric_results.find(socket_id);
            if (metric_it != this->metric_results.end())
                metric_values = metric_it->second;

            if (!first)
                ss << ",\n";
            first = false;

            nlohmann::json socket_json = utils::to_json<double>(this->total_duration_ms, this->config.measurement_type, event_values, metric_values, socket_id);
            socket_json["readings"][0]["pdu_label"] = target.label;
            ss << socket_json.dump(2);
        }

        ss << "\n]\n";
        return ss.str();
    }

    const optkit::metrics::MetricBuilder<double> &default_metrics()
    {
        static const optkit::metrics::MetricBuilder<double> mb = []() {
            return optkit::metrics::MetricBuilder<double>{}
                .add(to_string(PduDomain::NODE_ENERGY), std::vector<uint64_t>(1, 0x0))
                .build("watt_hour", [](const std::unordered_map<std::string, double> &results) {
                    return optkit::metrics::get_event_count(results, to_string(PduDomain::NODE_ENERGY)) / 3600.0;
                });
        }();

        return mb;
    }

    std::string to_string(const std::unordered_map<optkit::energy::pdu::PduDomain, double> &map)
    {
        std::ostringstream oss;
        oss << map;
        return oss.str();
    }

    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::pdu::PduDomain, double> &map)
    {
        for (std::unordered_map<optkit::energy::pdu::PduDomain, double>::const_iterator it = map.begin(); it != map.end(); ++it)
            os << to_string(it->first) << ": " << it->second << " ";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::pdu::PduDomain, double>> &map)
    {
        for (std::unordered_map<int32_t, std::unordered_map<optkit::energy::pdu::PduDomain, double>>::const_iterator it = map.begin(); it != map.end(); ++it)
            os << "socket " << it->first << " => " << it->second;
        return os;
    }
}