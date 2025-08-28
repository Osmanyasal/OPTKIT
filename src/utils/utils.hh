#pragma once
/*
    All header files that's defined in utils.
    to access utils include this file
*/
#include <algorithm>
#include <cstdint>
#include <random>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

#include <chrono>
#include <string>

// ENV CONFIG
#include "utils/deployment/deployment_config.hh"

// CUSTOM HEADERS
#include "utils/logging/logger.hh"

// OPTIMIZATION HEADERS
#include "utils/optimizations/cpu_opt.hh"

// PROFILING HEADERS
#include "core/query.hh"

// JSON
#include "utils/json.hh"

// MACRO DEFINITIONS
#define BIT(x) (1 << x)
#define STRINGIFY(...) #__VA_ARGS__

#define CONCAT(a, b) a##b
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)

#define BLOCK_TIMER(block_name) \
    optkit::utils::BlockTimer block_timer { block_name }

#define EXEC_IF_ROOT                                                                                                 \
    if (!optkit::core::Query::is_root_priv_enabled)                                                                  \
    {                                                                                                                \
        OPTKIT_CORE_WARN("Root priv is required for the execution of method '{}' in file '{}'", __func__, __FILE__); \
        return;                                                                                                      \
    }
#define EXEC_IF_ROOT_RETURN(ret_value)                                                                               \
    if (!optkit::core::Query::is_root_priv_enabled)                                                                  \
    {                                                                                                                \
        OPTKIT_CORE_WARN("Root priv is required for the execution of method '{}' in file '{}'", __func__, __FILE__); \
        return ret_value;                                                                                            \
    }

/*  USAGE EXPLANATION
    Following works like this class structure..

    class Base
    {
        public:
            virtual ~Base() {}
    };

    class Derived : public Base
    {
    };

    # takes references

        Derived dd;
        Base& bref = dd;

        INSTANCEOF(Base, dd) -> true
        INSTANCEOF(Derived, dd) -> true

        INSTANCEOF(Base, bref) -> true
        INSTANCEOF(Derived, bref) -> true

    # takes pointers

        Derived* dd = new Derived;
        INSTANCEOF_PTR(Base, dd) -> true
        INSTANCEOF_PTR(Derived, dd) -> true
*/
#define INSTANCEOF(CLASS, objREF) (dynamic_cast<CLASS *>(&objREF) != nullptr)
#define INSTANCEOF_PTR(CLASS, objPTR) (dynamic_cast<CLASS *>(objPTR) != nullptr)

namespace optkit::utils
{
    class BlockTimer
    {
    public:
        BlockTimer(const std::string &block_name) : block_name(block_name)
        {
            OPTKIT_CORE_INFO("BLOCK :{} is being measured..", this->block_name);
            start = std::chrono::high_resolution_clock::now();
        }
        ~BlockTimer()
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
            OPTKIT_CORE_INFO("block :{} execution time : {}ms", this->block_name, duration);
        }

    private:
        const std::string block_name;
        std::chrono::high_resolution_clock::time_point start;
    };

    extern std::string EXECUTION_FOLDER_NAME;

    // FUNCTION DECLERATIONS
    std::string generateGUID();
    std::string get_date(const std::string &format = "%d_%m_%Y");
    std::string get_time(const std::string &format = "%H_%M_%S");
    std::vector<std::string> get_all_files(const std::string &directory_name);
    std::vector<std::string> str_split(const std::string &s, const std::string &delimiter);

    template <typename resultT>
    nlohmann::json to_json(double duration, const char *metric_name,
                           const std::vector<std::pair<std::string, resultT>> &results,
                           const std::vector<std::pair<std::string, double>> &metric_results,
                           int32_t socket_number = -1)
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
            resultT value = results[i].second;

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

    OPT_FORCE_INLINE int count_trailing_zeros(unsigned int x)
    {
        if (x == 0)
        {
            return 0; // Or handle the zero case as needed
        }

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
        return __builtin_ctz(x);
#elif defined(_MSC_VER)
        unsigned long index;
        _BitScanForward(&index, x);
        return index;
#else
        // Portable implementation (less efficient)
        int count = 0;
        while ((x & 1) == 0)
        {
            x >>= 1;
            count++;
        }
        return count;
#endif
    }

    OPT_FORCE_INLINE bool is_path_exists(const std::string &location)
    {
        struct stat buffer;
        return (stat(location.c_str(), &buffer) == 0);
    }

    OPT_FORCE_INLINE void create_directory(const std::string &folderName)
    {
        // For Linux/Unix
        if (mkdir(folderName.c_str(), 0777) != 0)
        {
            OPTKIT_CORE_ERROR("Directory already exists {}", folderName);
        }
    }

    OPT_FORCE_INLINE std::string read_file(const std::string &location)
    {
        std::stringstream buffer;
        std::ifstream file(location);

        if (OPT_UNLIKELY(!file.is_open()))
            throw std::runtime_error("Failed to open the file: " + location);

        buffer << file.rdbuf();
        file.close();

        return buffer.str();
    }
    OPT_FORCE_INLINE void write_file(const std::string &location, const std::string &text, bool is_verbose = false)
    {
        std::ofstream file(location, std::ios_base::out | std::ios_base::app); // create & append mode
        if (OPT_UNLIKELY(!file.is_open()))
            throw std::runtime_error("Failed to open the file for writing: " + location);

        file << text;
        file.close();

        if (is_verbose)
        {
            OPTKIT_CORE_INFO("Data successfully written to file: {}", location);
        }
    }

    OPT_FORCE_INLINE void read_msr(int32_t cpuid, off_t MSR_REGISTER_address, uint64_t *MSR_REGISTER_bits)
    {
        char msr_file_name[64];
        sprintf(msr_file_name, "/dev/cpu/%d/msr_safe", cpuid);
        int32_t fd = open(msr_file_name, O_RDONLY);
        if (fd < 0)
        {
            OPTKIT_CORE_WARN("read msr error [{}]", cpuid);
            return;
        }

        if (pread(fd, MSR_REGISTER_bits, sizeof MSR_REGISTER_bits, MSR_REGISTER_address) != sizeof MSR_REGISTER_bits)
        {

            OPTKIT_CORE_WARN("read msr error -- cannot read register {}", MSR_REGISTER_address);
        }
        close(fd);
    }

    OPT_FORCE_INLINE void write_msr(int32_t cpuid, off_t MSR_REGISTER_address, uint64_t MSR_REGISTER_bits)
    {
        char msr_file_name[64];
        sprintf(msr_file_name, "/dev/cpu/%d/msr_safe", cpuid);
        int32_t fd = open(msr_file_name, O_WRONLY);
        if (fd < 0)
        {
            OPTKIT_CORE_WARN("write msr error [{}]", cpuid);
            return;
        }

        if (pwrite(fd, &MSR_REGISTER_bits, sizeof MSR_REGISTER_bits, MSR_REGISTER_address) != sizeof MSR_REGISTER_bits)
        {
            OPTKIT_CORE_WARN("read msr error -- cannot read register {}", MSR_REGISTER_address);
        }
    }

}
