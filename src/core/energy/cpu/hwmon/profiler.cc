#include "core/energy/cpu/hwmon/profiler.hh"

namespace optkit::energy::hwmon
{
    // Sampling function that runs in a separate thread
    OPT_FORCE_INLINE void sampling_function(Profiler &profiler)
    {
        profiler.read_and_store();
    }

    Profiler::Profiler(const ProfilerConfig &profiler_config, const optkit::metrics::MetricBuilder<double> &mb)
        : BaseProfiler{profiler_config}, metric_builder{mb}
    {
        auto &avail_domains = Query::hwmon_domain_info();

        if (avail_domains.empty())
        {
            OPTKIT_CORE_ERROR("HWMON: No power sensors found!");
            return;
        }

        // Initialize sensors from detected domains
        for (const auto &domain_info : avail_domains)
        {
            SensorInfo sensor;
            sensor.path = domain_info.path;
            sensor.domain = domain_info.domain;
            sensor.socket_id = domain_info.socket_id;
            sensor.scale = domain_info.scale;
            
            // Open file stream for reading
            sensor.file_stream.open(sensor.path);
            if (!sensor.file_stream.is_open())
            {
                OPTKIT_CORE_WARN("Failed to open sensor: {}", sensor.path);
                continue;
            }

            sensors.push_back(std::move(sensor));
        }

        if (sensors.empty())
        {
            OPTKIT_CORE_ERROR("HWMON: Failed to open any power sensors!");
            return;
        }

        last_read_time = std::chrono::high_resolution_clock::now();

        // Start sampling thread if requested
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
        // Stop sampling thread if running
        if (this->config.is_sampling && this->sampling_thread.joinable())
        {
            this->is_sampling = false;
            this->sampling_thread.join();
        }

        // Final read and store
        read_and_store();

        // Aggregate results
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>> aggregated = aggregate();

        for (auto &&aggr_item : aggregated)
        {
            std::vector<std::pair<std::string, double>> event_values;
            
            for (const auto &inner_pair : aggr_item.second)
            {
                for (const auto &domain_pair : inner_pair.second)
                {
                    HwmonDomain domain = domain_pair.first;
                    double reading = domain_pair.second;
                    event_values.emplace_back(to_string(domain), reading);
                }
            }
            
            event_values.emplace_back("duration_microsec", this->total_duration_ms * 1000.0); // convert to microseconds

            this->metric_results[std::strtol(aggr_item.first.c_str(), nullptr, 10)] = 
                this->metric_builder.calculate(
                    std::unordered_map<std::string, double>(event_values.begin(), event_values.end()));
        }

        if (OPT_LIKELY(this->config.dump_results_to_file))
            this->save();

        if (OPT_LIKELY(this->config.verbose))
        {
            std::cout << std::fixed << "\033[1;33m" // Yellow
                      << "Block: " << this->config.block_name << "\033[0m"
                      << " [" << this->total_duration_ms << "ms] Measured\n";

            if (OPT_UNLIKELY(this->metric_builder.print_events))
            {
                for (auto &&event : this->event_results)
                    std::cout << event.second << std::endl;
            }

            for (auto &&metric : this->metric_results)
            {
                std::cout << "\tSocket " << metric.first << " Metrics: \n";
                for (const auto &pair : metric.second)
                    std::cout << std::fixed << "\t\t" << pair.first << ": " << pair.second << "\n";
            }
        }

        // Close all file streams
        for (auto &sensor : sensors)
        {
            if (sensor.file_stream.is_open())
                sensor.file_stream.close();
        }
    }

    void Profiler::disable()
    {
        OPTKIT_CORE_WARN("HWMON cannot be disabled (sysfs-based)");
    }

    void Profiler::enable()
    {
        OPTKIT_CORE_WARN("HWMON is always enabled (sysfs-based)");
    }

    std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> Profiler::read()
    {
        std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> result;
        
        for (auto &sensor : sensors)
        {
            // Rewind to beginning of file
            sensor.file_stream.clear();
            sensor.file_stream.seekg(0, std::ios::beg);

            std::string value_str;
            if (std::getline(sensor.file_stream, value_str))
            {
                try
                {
                    // Read value in microwatts and convert to watts
                    uint64_t value_uw = std::stoull(value_str);
                    double value_w = static_cast<double>(value_uw) * sensor.scale;
                    
                    result[sensor.socket_id][sensor.domain] = value_w;
                }
                catch (const std::exception &e)
                {
                    OPTKIT_CORE_WARN("Failed to parse value from {}: {}", sensor.path, e.what());
                }
            }
        }

        return result;
    }

    std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>> Profiler::aggregate()
    {
        double total_duration = 0.0;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>> aggregated_events;

        for (size_t i = 0; i < read_buffer.size(); ++i)
        {
            const auto &entry = read_buffer[i];
            double duration_ms = entry.first;
            total_duration += duration_ms;
            
            const std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>> &power_values = entry.second;

            // Convert power (W) to energy (J) by multiplying by time (s)
            double duration_s = duration_ms / 1000.0;

            for (const auto &socket_pair : power_values)
            {
                int32_t socket_id = socket_pair.first;

                for (const auto &domain_pair : socket_pair.second)
                {
                    HwmonDomain domain = domain_pair.first;
                    double power_w = domain_pair.second;
                    double energy_j = power_w * duration_s;

                    // Aggregate the energy
                    aggregated_events[std::to_string(socket_id)][socket_id][domain] += energy_j;
                }
            }
        }

        std::vector<std::pair<std::string, std::unordered_map<int32_t, std::unordered_map<HwmonDomain, double>>>> event_value(
            aggregated_events.begin(), aggregated_events.end());

        this->event_results = std::move(event_value);
        this->total_duration_ms = total_duration;
        
        return aggregated_events;
    }

    std::string Profiler::to_json()
    {
        std::stringstream ss;
        ss << "[\n";
        bool first = true;

        // Handle sockets with events (regardless of whether they have metrics)
        for (const auto &event_pair : this->event_results)
        {
            int32_t socket_id = std::stoi(event_pair.first);

            // Convert HWMON domain results to individual event entries
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
        
        ss << "\n]\n";
        return ss.str();
    }

    std::string to_string(const std::unordered_map<optkit::energy::hwmon::HwmonDomain, double> &map)
    {
        std::ostringstream oss;
        oss << map;
        return oss.str();
    }

    // Overloading << for map with HwmonDomain as keys
    std::ostream &operator<<(std::ostream &os, const std::unordered_map<optkit::energy::hwmon::HwmonDomain, double> &map)
    {
        for (const auto &item : map)
            os << item.first << ": " << item.second << " W\n";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const std::unordered_map<int32_t, std::unordered_map<optkit::energy::hwmon::HwmonDomain, double>> &map)
    {
        for (const auto &pair : map)
        {
            os << "\tSocket " << pair.first << " Domains:\n";
            for (const auto &innerpair : pair.second)
            {
                os << "\t\t" << innerpair.first << ": " << innerpair.second << " Watts\n";
            }
        }
        return os;
    }

} // namespace optkit::energy::hwmon
