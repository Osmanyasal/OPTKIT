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


// CUSTOM HEADERS
#include "utils/logging/logger.hh"
#include "utils/type.hh"

// OPTIMIZATION HEADERS
#include "utils/optimizations/cpu_opt.hh"

// PROFILING HEADERS
#include "utils/profiling/block_timer.hh"
#include "utils/profiling/gantt_instrumentor.hh"
 
#include "core/query.hh"

// MACRO DEFINITIONS
#define BIT(x) (1 << x)

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

// GLOBAL VARIABLES

extern std::string EXECUTION_FOLDER_NAME;

// FUNCTION DECLERATIONS
std::string generateGUID();
std::string get_date(const std::string &format = "%d_%m_%Y");
std::string get_time(const std::string &format = "%H_%M_%S");
std::vector<std::string> get_all_files(const std::string &directory_name);
std::vector<std::string> str_split(std::string s, std::string delimiter);

OPT_FORCE_INLINE std::string read_file(const std::string &location, bool is_verbose = true)
{
    std::stringstream buffer;
    std::ifstream file(location);
    if (OPT_UNLIKELY(is_verbose && !file.is_open()))
    {
        OPTKIT_CORE_ERROR("file not found at the location {}", location);
        throw std::runtime_error("Failed to open the file: " + location);
    }
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
OPT_FORCE_INLINE void write_file(const std::string &location, const std::string &text, bool is_verbose = false)
{
    std::ofstream file(location, std::ios_base::out | std::ios_base::app); // create & append mode
    if (OPT_UNLIKELY(is_verbose && !file.is_open()))
    {
        OPTKIT_CORE_ERROR("Failed to open the file for writing: {}", location);
        throw std::runtime_error("Failed to open the file for writing: " + location);
    }

    file << text << "\n";
    file.close();

    if (is_verbose)
    {
        OPTKIT_CORE_INFO("Data successfully written to file: {}", location);
    }
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
