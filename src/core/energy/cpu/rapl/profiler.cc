#include "core/energy/cpu/rapl/profiler.hh"

namespace optkit::energy::rapl
{
    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb) : BaseProfiler{profiler_config}, metric_builder{mb}
    {
        auto &packages = optkit::Query::detect_cpu_packages();
        auto &avail_domains = Query::rapl_domain_info(); // Monitor for all available domains
        auto s_type = optkit::utils::read_file("/sys/bus/event_source/devices/power/type");
        auto type = std::atoi(s_type.c_str());

        struct perf_event_attr attr;

        fd_package_domain.resize(packages.size());

        for (size_t package = 0; package < packages.size(); package++)
        {
            fd_package_domain[package].resize(avail_domains.size());

            for (auto domain = 0u; domain < avail_domains.size(); domain++)
            {
                auto selected_domain = avail_domains[domain];
                fd_package_domain[package][domain] = -1;

                ::memset(&attr, 0x0, sizeof(attr));
                attr.type = type;
                attr.config = selected_domain.config;

                if (attr.config == 0)
                    continue;

                fd_package_domain[package][domain] = ::syscall(__NR_perf_event_open, &attr, -1, packages.at(package).at(0), -1, 0);

                if (fd_package_domain[package][domain] < 0)
                {
                    OPTKIT_CORE_ERROR("RAPL PERF: SOMETHING WENT WRONG!! {}", fd_package_domain[package][domain]);
                }
            }
        }
    }

    Profiler::~Profiler()
    {
        read_and_store();

        // socket_id_str - <socket_id - <rapl_domain - read value>>
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<optkit::energy::rapl::RaplDomain, double>>> aggregated = aggregate();
        for (auto &&aggr_item : aggregated)
        {
            std::vector<std::pair<std::string, double>> event_values;
            for (const auto &inner_pair : aggr_item.second)
            {
                // Remove unused variable socket_id
                for (const auto &domain_pair : inner_pair.second)
                {
                    RaplDomain domain = domain_pair.first;
                    double reading = domain_pair.second;
                    event_values.emplace_back(to_string(domain), reading);
                }
            }
            event_values.emplace_back("duration_microsec", this->total_duration_ms * 1000.0); // convert to microseconds

            this->metric_results[std::strtol(aggr_item.first.c_str(), nullptr, 10)] = this->metric_builder.calculate(
                std::unordered_map<std::string, double>(event_values.begin(), event_values.end()));
        }

        // call it for socket 0 and 1 and so on...
        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            if (OPT_UNLIKELY(this->metric_builder.print_events))
                for (auto &&event : this->event_results)
                    std::cout << event.second << std::endl;

            for (auto &&metric : this->metric_results)
            {
                std::cout << "\tPackage " << metric.first << " Metrics: \n";
                for (const auto &pair : metric.second)
                    std::cout << std::fixed << "\t\t" << pair.first << ":" << pair.second << "\n";
            }
        }
        // Close all file descriptions!
        for (auto package = 0u; package < OPTKIT_ENV_CPU_NUM_SOCKETS; package++)
            for (auto domain = 0u; domain < Query::rapl_domain_info().size(); domain++)
                ::close(fd_package_domain[package][domain]);
    }

    // returns socket_id_str - <socket_id - <rapl_domain - read value>>
    std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>> aggregated_events;
        for (const auto &entry : read_buffer)
        {
            total_duration += entry.first;
            const std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> &values = entry.second; // socket_id - rapl_domain - reading

            for (const auto &pair : values)
            {
                int32_t socket_id = pair.first;

                for (const auto &innerpair : pair.second)
                {
                    RaplDomain domain = innerpair.first;
                    double reading = innerpair.second;

                    // Aggregate the readings
                    aggregated_events[std::to_string(socket_id)][socket_id][domain] += reading;
                    std::cout << "Aggregating Event: " << to_string(domain) << " Socket: " << socket_id << " Domain: " << domain << " Reading: " << reading << " total:" << aggregated_events[std::to_string(socket_id)][socket_id][domain] << "\n";
                }
            }
        }
        std::vector<std::pair<std::string, std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>>>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        return aggregated_events;
    }

    void Profiler::disable()
    {
        OPTKIT_CORE_WARN("Rapl cannot be disabled");
    }
    void Profiler::enable()
    {
        OPTKIT_CORE_WARN("Rapl is always enabled");
    }

    std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> Profiler::read()
    {
        std::unordered_map<int32_t, std::unordered_map<RaplDomain, double>> result;
        int64_t value = 0;
        const auto &avail_domains = Query::rapl_domain_info();

        for (size_t package = 0; package < OPTKIT_ENV_CPU_NUM_SOCKETS; ++package)
        {
            for (size_t domain = 0; domain < avail_domains.size(); ++domain)
            {
                const auto &selected_domain = avail_domains[domain];
                int fd = fd_package_domain[package][domain];

                if (fd == -1)
                    continue;

                if (::read(fd, &value, sizeof(value)) == sizeof(value))
                {
                    if (OPT_LIKELY(this->config.is_reset_after_read))
                        ::ioctl(fd, PERF_EVENT_IOC_RESET, 0);

                    result[static_cast<int32_t>(package)][selected_domain.domain] = static_cast<double>(value) * selected_domain.scale;
                    std::cout << "Read Package " << package << " Domain " << selected_domain.event << ": " << result[static_cast<int32_t>(package)][selected_domain.domain] << " " << selected_domain.units << "\n";
                }
            }
        }
        return result;
    }

    std::string Profiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";

        bool first = true;
        std::set<int32_t> processed_sockets;

        // First, handle sockets with events (regardless of whether they have metrics)
        for (const auto &event_pair : this->event_results)
        {
            int32_t socket_id = std::stoi(event_pair.first);
            processed_sockets.insert(socket_id);

            // Convert RAPL domain results to individual event entries
            std::vector<std::pair<std::string, double>> event_values;
            for (const auto &socket_pair : event_pair.second)
            {
                for (const auto &domain_pair : socket_pair.second)
                {
                    std::string domain_name = to_string(domain_pair.first) + "__Joules";
                    double domain_value = domain_pair.second;
                    event_values.emplace_back(domain_name, domain_value);
                }
            }

            // Check if this socket also has metrics
            std::vector<std::pair<std::string, double>> metric_values;
            auto metric_it = this->metric_results.find(socket_id);
            if (metric_it != this->metric_results.end())
            {
                for (const auto &metric : metric_it->second)
                {
                    metric_values.emplace_back(metric.first, metric.second);
                }
            }

            if (!first)
                ss << ",\n";
            first = false;

            // Generate JSON with events and metrics (if any) for this socket
            nlohmann::json socket_json = utils::to_json<double>(
                this->total_duration_ms,
                this->config.measurement_type,
                event_values,
                metric_values,
                socket_id);
            ss << socket_json.dump(2);
        }

        // Then, handle sockets that have metrics but no events
        for (const auto &metric_pair : this->metric_results)
        {
            int32_t socket_id = metric_pair.first;

            // Skip if we already processed this socket in the events loop
            if (processed_sockets.find(socket_id) != processed_sockets.end())
                continue;

            // Empty events vector since this socket has no events
            std::vector<std::pair<std::string, double>> empty_events;

            // Convert metric results to vector format
            std::vector<std::pair<std::string, double>> metric_values;
            for (const auto &metric : metric_pair.second)
            {
                metric_values.emplace_back(metric.first, metric.second);
            }

            if (!first)
                ss << ",\n";
            first = false;

            // Generate JSON with only metrics for this socket
            nlohmann::json socket_json = utils::to_json<double>(
                this->total_duration_ms,
                this->config.measurement_type,
                empty_events,
                metric_values,
                socket_id);
            ss << socket_json.dump(2);
        }

        ss << "\n]\n";
        return ss.str();
    }

    std::string to_string(const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        std::ostringstream oss;
        oss << map;
        return oss.str();
    }
    // Overloading << for map with RaplDomain as keys
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::rapl::RaplDomain, double> &map)
    {
        for (const auto &item : map)
            os << item.first << ": " << item.second << "\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::rapl::RaplDomain, double>> &map)
    {
        for (const auto &pair : map)
        {
            os << "\tPackage " << pair.first << " Domains:\n";
            for (const auto &innerpair : pair.second)
            {
                os << "\t\t" << innerpair.first << ": " << innerpair.second << " Joules Consumed.\n";
            }
        }
        return os;
    }
}