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
#include <type_traits>
#include <unordered_map>

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

#define BLOCK_TIMER(block_name, out_duration_ms) \
    optkit::utils::BlockTimer block_timer { block_name, out_duration_ms }

#define EXEC_IF_ROOT                                                                                                 \
    if (!optkit::Query::is_root_priv_enabled)                                                                        \
    {                                                                                                                \
        OPTKIT_CORE_WARN("Root priv is required for the execution of method '{}' in file '{}'", __func__, __FILE__); \
        return;                                                                                                      \
    }
#define EXEC_IF_ROOT_RETURN(ret_value)                                                                               \
    if (!optkit::Query::is_root_priv_enabled)                                                                        \
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
        BlockTimer(const std::string &block_name, double &duration_ms, bool verbose = false) : block_name(block_name), duration_ms(duration_ms), verbose(verbose)
        {
            if (OPT_LIKELY(verbose))
            {
                OPTKIT_INFO("BLOCK :{} is being measured..", this->block_name);
            }
            start = std::chrono::high_resolution_clock::now();
        }
        ~BlockTimer()
        {
            auto end = std::chrono::high_resolution_clock::now();
            duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
            if (OPT_LIKELY(verbose))
            {
                OPTKIT_INFO("block :{} execution time : {}ms", this->block_name, duration_ms);
            }
        }

    private:
        const std::string block_name;
        std::chrono::high_resolution_clock::time_point start;
        double &duration_ms;
        bool verbose;
    };

    extern std::string EXECUTION_FOLDER_NAME;

    // FUNCTION DECLERATIONS
    std::string generateGUID();
    std::string get_date(const std::string &format = "%d_%m_%Y");
    std::string get_time(const std::string &format = "%H_%M_%S");
    std::vector<std::string> get_all_files(const std::string &directory_name);
    std::vector<std::string> str_split(const std::string &s, const std::string &delimiter);
    std::string str_trim(const std::string &s);

    // Helper functions for C++11 compatible type handling
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    format_result_value(std::ostringstream &ss, const T &value)
    {
        ss << std::fixed << value;
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::unordered_map<uint32_t, double>>::value, void>::type
    format_result_value(std::ostringstream &ss, const T &value)
    {
        bool first = true;
        for (typename T::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            if (!first)
                ss << ",";
            ss << std::fixed << it->second;
            first = false;
        }
    }

    // Specialization for std::pair<double, double> (e.g., temperature pairs)
    template <typename T>
    typename std::enable_if<std::is_same<T, std::pair<double, double>>::value, void>::type
    format_result_value(std::ostringstream &ss, const T &value)
    {
        ss << "{gpu:" << std::fixed << value.first << ",mem:" << std::fixed << value.second << "}";
    }

    // Fallback for unsupported types
    template <typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value &&
                                !std::is_same<T, std::unordered_map<uint32_t, double>>::value &&
                                !std::is_same<T, std::pair<double, double>>::value,
                            void>::type
    format_result_value(std::ostringstream &ss, const T &value)
    {
        ss << "\"unsupported_type\"";
    }

    // Helper to get dtype string
    template <typename T>
    typename std::enable_if<std::is_same<T, uint64_t>::value, std::string>::type
    get_dtype_string() { return "uint64_t"; }

    template <typename T>
    typename std::enable_if<std::is_same<T, double>::value, std::string>::type
    get_dtype_string() { return "double"; }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::pair<double, double>>::value, std::string>::type
    get_dtype_string() { return "pair<double,double>"; }

    template <typename T>
    typename std::enable_if<!std::is_same<T, uint64_t>::value &&
                                !std::is_same<T, double>::value &&
                                !std::is_same<T, std::pair<double, double>>::value,
                            std::string>::type
    get_dtype_string() { return "double"; }

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
            format_result_value(ss, value);

            nlohmann::json entry;
            entry["type"] = "event";
            entry["name"] = name;
            entry["value"] = ss.str();
            entry["value_unit"] = unit;
            entry["dtype"] = get_dtype_string<resultT>();

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

    OPT_FORCE_INLINE void remove_directory(const std::string &folderName)
    {
        // For Linux/Unix
        if (rmdir(folderName.c_str()) != 0)
        {
            OPTKIT_CORE_ERROR("Failed to remove directory {}", folderName);
        }
        else
        {
            OPTKIT_CORE_INFO("Directory removed: {}", folderName);
        }
    }

    OPT_FORCE_INLINE std::string read_file(const std::string &location, bool verbose) noexcept
    {
        std::stringstream buffer;
        std::ifstream file(location);

        if (OPT_UNLIKELY(!file.is_open()))
        {
            if (verbose)
            {
                OPTKIT_CORE_ERROR("Failed to open the file: {}", location);
            }
            return "";
        }
        buffer << file.rdbuf();
        file.close();

        return buffer.str();
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

    OPT_FORCE_INLINE void write_file(const std::string &location, const std::string &text, bool is_verbose) noexcept
    {
        std::ofstream file(location, std::ios_base::out | std::ios_base::app); // create & append mode
        if (OPT_UNLIKELY(!file.is_open()))
        {
            if (is_verbose)
            {
                OPTKIT_CORE_ERROR("Failed to open the file: {}", location);
            }
        }

        file << text;
        file.close();
        if (is_verbose)
        {
            OPTKIT_CORE_INFO("Data successfully written to file: {}", location);
        }
    }

    OPT_FORCE_INLINE void write_file(const std::string &location, const std::string &text)
    {
        std::ofstream file(location, std::ios_base::out | std::ios_base::app); // create & append mode
        if (OPT_UNLIKELY(!file.is_open()))
            throw std::runtime_error("Failed to open the file for writing: " + location);

        file << text;
        file.close();
    }

    OPT_FORCE_INLINE bool remove_file(const std::string &location, bool is_verbose)
    {
        if (std::remove(location.c_str()) != 0)
        {
            if (is_verbose)
            {
                OPTKIT_CORE_ERROR("Error deleting file: {}", location);
            }
            return false;
        }
        else
        {
            if (is_verbose)
            {
                OPTKIT_CORE_INFO("File deleted successfully: {}", location);
            }
        }
        return true;
    }

    OPT_FORCE_INLINE bool read_msr(int32_t cpuid, off_t MSR_REGISTER_address, uint64_t *MSR_REGISTER_bits, bool is_verbose = false)
    {
        char msr_file_name[64];
        sprintf(msr_file_name, "/dev/cpu/%d/msr_safe", cpuid);
        int32_t fd = open(msr_file_name, O_RDONLY);
        if (fd < 0)
        {
            if (is_verbose)
            {
                OPTKIT_CORE_WARN("read msr error [{}]", cpuid);
            }
            return false;
        }

        if (pread(fd, MSR_REGISTER_bits, sizeof MSR_REGISTER_bits, MSR_REGISTER_address) != sizeof MSR_REGISTER_bits)
        {
            if (is_verbose)
            {
                OPTKIT_CORE_WARN("read msr error -- cannot read register {}", MSR_REGISTER_address);
            }
            return false;
        }
        close(fd);
        return true;
    }

    OPT_FORCE_INLINE bool write_msr(int32_t cpuid, off_t MSR_REGISTER_address, uint64_t MSR_REGISTER_bits, bool is_verbose = false)
    {
        char msr_file_name[64];
        sprintf(msr_file_name, "/dev/cpu/%d/msr_safe", cpuid);
        int32_t fd = open(msr_file_name, O_WRONLY);
        if (fd < 0)
        {
            if (is_verbose)
            {
                OPTKIT_CORE_WARN("write msr error [{}]", cpuid);
            }
            return false;
        }

        if (pwrite(fd, &MSR_REGISTER_bits, sizeof MSR_REGISTER_bits, MSR_REGISTER_address) != sizeof MSR_REGISTER_bits)
        {
            if (is_verbose)
            {
                OPTKIT_CORE_WARN("write msr error -- cannot write register {}", MSR_REGISTER_address);
            }
            return false;
        }
        return true;
    }
}
