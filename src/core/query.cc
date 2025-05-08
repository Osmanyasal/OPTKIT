
#include "core/query.hh"

namespace optkit::core
{

    int64_t Query::OPTKIT_SOCKET0__ENABLED = 0;
    int64_t Query::OPTKIT_SOCKET1__ENABLED = 0;

    int64_t Query::OPTKIT_SOCKET0__CORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET1__CORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET0__UNCORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET1__UNCORE_FREQ = -1;

    const int16_t Query::num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    const int16_t Query::num_sockets = Query::detect_packages().size();
    const bool Query::is_root_priv_enabled = (geteuid() == 0);

    // Socket - cpu list
    const std::map<int32_t, std::vector<int32_t>> &Query::detect_packages()
    {
        static std::map<int32_t, std::vector<int32_t>> result;

        if (result.size() == 0)
        {
            int32_t core_id = 0;
            while (true)
            {
                try
                {
                    int32_t package_id = std::stoi(read_file("/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/topology/physical_package_id", false));
                    if (result.find(package_id) == result.end())
                    {
                        result[package_id] = {};
                    }
                    result[package_id].push_back(core_id);
                    core_id++;
                }
                catch (const std::exception &e)
                {
                    break; // when there's no more file/cores.
                }
            }
        }
        return result;
    }
 

std::ostream &operator<<(std::ostream &out, const std::map<int32_t, std::vector<int32_t>> &packages)
{
    std::ostringstream oss;
    int32_t total_cores = 0;
    int32_t count = 0;
    for (const auto &entry : packages)
    {
        int32_t package_id = entry.first;
        const std::vector<int32_t> &core_ids = entry.second;
        total_cores += core_ids.size();

        for (int32_t core_id : core_ids)
        {
            oss << core_id << " (" << package_id << ") ";
            if (++count % 8 == 0)
                oss << std::endl;
        }
    }
    out << "\n\tDetected " << total_cores << " cores in " << packages.size() << " packages\n---------------------------------------\n"
        << oss.str();
    return out;
}

} // namespace optkit::core::pmu