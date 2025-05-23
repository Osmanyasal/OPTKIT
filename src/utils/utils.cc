#include "utils/utils.hh"

// This global variable is needed by save methods of profilers.
std::string optkit::utils::EXECUTION_FOLDER_NAME{optkit::utils::get_date() + "__" + optkit::utils::get_time() + "__" + optkit::utils::generateGUID().substr(0, CONF_LOG_PRINT_GUID_LENGTH)};

std::string optkit::utils::generateGUID()
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

std::vector<std::string> optkit::utils::get_all_files(const std::string &directory_name)
{
    std::vector<std::string> files;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory_name.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            if(OPT_LIKELY(ent->d_name[0] != '.'))
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
std::vector<std::string> optkit::utils::str_split(std::string s, std::string delimiter)
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
    // res.push_back(s.substr(pos_start));
    return res;
}

std::string optkit::utils::get_date(const std::string &format)
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
std::string optkit::utils::get_time(const std::string &format)
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