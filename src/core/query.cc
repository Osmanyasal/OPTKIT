
#include "core/query.hh"
#include "utils/utils.hh"
namespace optkit::core
{

    int64_t Query::OPTKIT_SOCKET0__ENABLED = 0;
    int64_t Query::OPTKIT_SOCKET1__ENABLED = 0;

    int64_t Query::OPTKIT_SOCKET0__CORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET1__CORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET0__UNCORE_FREQ = -1;
    int64_t Query::OPTKIT_SOCKET1__UNCORE_FREQ = -1;

    bool Query::create_folder;
    const int16_t Query::num_sockets = OPTKIT_ENV_CPU_NUM_SOCKETS;
    const int16_t Query::num_logical_cores = OPTKIT_ENV_CPU_TOTAL_LOGICAL_CPUS;
    const bool Query::is_root_priv_enabled = (geteuid() == 0);

    int32_t Query::paranoid()
    {
        std::string value = optkit::utils::read_file("/proc/sys/kernel/perf_event_paranoid");
        return std::stoi(value);
    }

    // Socket id - cpus belonging to that socket 
    // e.g. {0: [0, 1, 2, 3], 1: [8, 9, 10, 11]} means socket 0 has cores 0-3 and socket 1 has cores 4-7
    const std::map<int32_t, std::vector<int32_t>> &Query::detect_cpu_packages()
    {
        static std::map<int32_t, std::vector<int32_t>> result;
        
        if (result.size() == 0)
        {
            int32_t core_id = 0;
            while (true)
            {
                try
                {
                    int32_t package_id = std::stoi(optkit::utils::read_file("/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/topology/physical_package_id"));
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

    std::string to_string(const std::map<int32_t, std::vector<int32_t>> &packages)
    {
        std::ostringstream oss;
        oss << packages;
        return oss.str();
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