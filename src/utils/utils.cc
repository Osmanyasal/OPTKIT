#include "utils/utils.hh"

namespace optkit::utils
{

    // This global variable is needed by save methods of profilers.
    std::string EXECUTION_FOLDER_NAME{get_date() + "__" + get_time() + "__" + generateGUID().substr(0, CONF_LOG_PRINT_GUID_LENGTH)};

    std::string generateGUID()
    {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, 0xFFFFFFFFFFFFFFFF);

        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        // Generate first part (8 digits)
        ss << std::setw(8) << dis(gen);

        // Generate second part (4 digits)
        ss << '-';
        ss << std::setw(4) << dis(gen);

        // Generate third part (4 digits)
        ss << '-';
        ss << std::setw(4) << dis(gen);

        // Generate fourth part (4 digits)
        ss << '-';
        ss << std::setw(4) << dis(gen);

        // Generate fifth part (12 digits)
        ss << '-';
        ss << std::setw(12) << dis(gen);
        return ss.str();
    }

    std::vector<std::string> get_all_files(const std::string &directory_name)
    {
        std::vector<std::string> files;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(directory_name.c_str())) != NULL)
        {
            /* print all the files and directories within directory */
            while ((ent = readdir(dir)) != NULL)
            {
                if (OPT_LIKELY(ent->d_name[0] != '.'))
                    files.push_back(ent->d_name);
            }
            closedir(dir);
        }
        else
        {
            OPTKIT_CORE_ERROR("Couldn't open the directory ! {}", directory_name);
        }
        return files;
    }
    std::vector<std::string> str_split(const std::string &s, const std::string &delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }
        res.push_back(s.substr(pos_start));
        return res;
    }

    nlohmann::json to_json(double duration, const char *metric_name,
                           const std::vector<std::pair<std::string, uint64_t>> &results,
                           const std::vector<std::pair<std::string, double>> &metric_results,
                           int32_t socket_number)
    {
        nlohmann::json result;
        nlohmann::json packageJson;
        packageJson["duration"] = duration;
        packageJson["duration_unit"] = "ms";
        packageJson["socket_number"] = socket_number;
        packageJson["measurement_type"] = metric_name;

        // Helper for splitting name and unit
        auto split_name_and_unit = [](const std::string &full_name) -> std::pair<std::string, std::string>
        {
            size_t pos = full_name.rfind("__");
            if (pos != std::string::npos && pos + 2 < full_name.size())
            {
                return std::make_pair(full_name.substr(0, pos), full_name.substr(pos + 2));
            }
            return std::make_pair(full_name, "None");
        };

        for (size_t i = 0; i < results.size(); ++i)
        {
            const std::string &raw_name = results[i].first;
            uint64_t value = results[i].second;

            std::pair<std::string, std::string> parsed = split_name_and_unit(raw_name);
            const std::string &name = parsed.first;
            const std::string &unit = parsed.second;

            std::ostringstream ss;
            ss << std::fixed << value;

            nlohmann::json entry;
            entry["type"] = "event";
            entry["name"] = name;
            entry["value"] = ss.str();
            entry["value_unit"] = unit;
            entry["dtype"] = "uint64_t";

            packageJson["measurements"].push_back(entry);
        }

        for (size_t i = 0; i < metric_results.size(); ++i)
        {
            const std::string &raw_name = metric_results[i].first;
            double value = metric_results[i].second;

            std::pair<std::string, std::string> parsed = split_name_and_unit(raw_name);
            const std::string &name = parsed.first;
            const std::string &unit = parsed.second;

            std::ostringstream ss;
            ss << std::fixed << value;

            nlohmann::json entry;
            entry["type"] = "metric";
            entry["name"] = name;
            entry["value"] = ss.str();
            entry["value_unit"] = unit;
            entry["dtype"] = "double";

            packageJson["measurements"].push_back(entry);
        }

        result["readings"].push_back(packageJson);
        return result;
    }

    std::string get_date(const std::string &format)
    {
        // Get the current time point
        auto now = std::chrono::system_clock::now();

        // Convert the time point to a time_t object
        auto currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert the time_t object to a tm struct
        std::tm *localTime = std::localtime(&currentTime);

        // Format the date using the provided format
        std::ostringstream oss;
        oss << std::put_time(localTime, format.c_str());

        return oss.str();
    }
    std::string get_time(const std::string &format)
    {
        // Get the current time point
        auto now = std::chrono::system_clock::now();

        // Convert the time point to a time_t object
        auto currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert the time_t object to a tm struct
        std::tm *localTime = std::localtime(&currentTime);

        // Format the time using the provided format
        std::ostringstream oss;
        oss << std::put_time(localTime, format.c_str());

        return oss.str();
    }

} // namespace optkit::utils::