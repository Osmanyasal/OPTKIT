#include "topology.hh"

void execute_topology_command(const CommandArgs &args)
{
    switch (args.target)
    {
    case Target::CPU:
        call_cpu_info_tools();
        break;
    case Target::GPU:
        call_gpu_info_tools();
        break;
    case Target::ALL:
    default:
        call_cpu_info_tools();
        call_gpu_info_tools();
        break;
    }
}

Topology get_system_topology()
{
    Topology topology;
    std::regex cpu_regex(R"(cpu\d+)");

    const std::string base = "/sys/devices/system/cpu";
    std::vector<std::string> entries = optkit::utils::get_all_files(base);
    for (size_t i = 0; i < entries.size(); ++i)
    {
        const std::string &cpu_dir = entries[i];
        if (!std::regex_match(cpu_dir, cpu_regex))
            continue;

        int cpu_num = std::stoi(cpu_dir.substr(3));

        try
        {
            std::string socket_str = optkit::utils::read_file(base + "/" + cpu_dir + "/topology/physical_package_id");
            std::string core_str = optkit::utils::read_file(base + "/" + cpu_dir + "/topology/core_id");

            int socket = std::stoi(socket_str);
            int core = std::stoi(core_str);
            topology[std::make_pair(socket, core)].push_back(cpu_num);
        }
        catch (...)
        {
            continue;
        }
    }

    return topology;
}

CacheHierarchy get_cache_hierarchy()
{
    CacheHierarchy cache_hierarchy;
    std::set<std::tuple<int, int, std::string, std::string>> seen;
    std::regex cpu_regex(R"(cpu\d+)");
    std::regex index_regex(R"(index\d+)");

    const std::string cpu_base = "/sys/devices/system/cpu";
    std::vector<std::string> cpu_entries = optkit::utils::get_all_files(cpu_base);
    for (size_t e = 0; e < cpu_entries.size(); ++e)
    {
        const std::string &cpu_dir = cpu_entries[e];
        if (!std::regex_match(cpu_dir, cpu_regex))
            continue;

        int socket;
        try
        {
            std::string socket_str = optkit::utils::read_file(cpu_base + "/" + cpu_dir + "/topology/physical_package_id");
            socket = std::stoi(socket_str);
        }
        catch (...)
        {
            continue;
        }

        std::string cache_path = cpu_base + "/" + cpu_dir + "/cache";
        if (!optkit::utils::is_path_exists(cache_path))
            continue;

        std::vector<std::string> cache_entries = optkit::utils::get_all_files(cache_path);
        for (size_t c = 0; c < cache_entries.size(); ++c)
        {
            const std::string &index_dir = cache_entries[c];
            if (!std::regex_match(index_dir, index_regex))
                continue;

            std::string base_path = cache_path + "/" + index_dir;

            try
            {
                std::string level_str = optkit::utils::read_file(base_path + "/level");
                std::string cache_type = optkit::utils::read_file(base_path + "/type");
                std::string size_str = optkit::utils::read_file(base_path + "/size");
                std::string shared_cpus = optkit::utils::read_file(base_path + "/shared_cpu_list");

                int level = std::stoi(level_str);

                // normalize strings (remove trailing newlines/spaces)
                while (!cache_type.empty() && (cache_type.back() == '\n' || cache_type.back() == '\r' || cache_type.back() == ' ' || cache_type.back() == '\t'))
                    cache_type.pop_back();
                while (!size_str.empty() && (size_str.back() == '\n' || size_str.back() == '\r' || size_str.back() == ' ' || size_str.back() == '\t'))
                    size_str.pop_back();
                while (!shared_cpus.empty() && (shared_cpus.back() == '\n' || shared_cpus.back() == '\r' || shared_cpus.back() == ' ' || shared_cpus.back() == '\t'))
                    shared_cpus.pop_back();

                int size_kb;
                if (size_str.find('K') != std::string::npos)
                {
                    size_kb = std::stoi(size_str.substr(0, size_str.find('K')));
                }
                else if (size_str.find('M') != std::string::npos)
                {
                    size_kb = static_cast<int>(std::stof(size_str.substr(0, size_str.find('M'))) * 1024);
                }
                else
                {
                    int size_bytes = std::stoi(size_str);
                    size_kb = size_bytes > 1024 ? size_bytes / 1024 : 1;
                }

                auto key = std::make_tuple(socket, level, cache_type, shared_cpus);
                if (seen.find(key) != seen.end())
                    continue;
                seen.insert(key);

                cache_hierarchy[socket][level][cache_type].push_back({size_kb,
                                                                      shared_cpus,
                                                                      cpu_dir + "/" + index_dir});
            }
            catch (...)
            {
                continue;
            }
        }
    }

    return cache_hierarchy;
}

std::string read_cache_attribute(const std::string &base_path, const std::string &attribute)
{
    try
    {
        std::ifstream file(base_path + "/" + attribute);
        std::string value;
        if (std::getline(file, value))
        {
            return value;
        }
    }
    catch (...)
    {
    }
    return "N/A";
}

std::vector<std::string> make_box_lines(const std::string &content, int width)
{
    std::string top = "+" + std::string(width - 2, '-') + "+";
    std::string mid = "|";
    int content_len = content.length();
    int available = width - 2;
    int padding_left = (available - content_len) / 2;
    int padding_right = available - content_len - padding_left;

    if (padding_left < 0)
        padding_left = 0;
    if (padding_right < 0)
        padding_right = 0;

    mid += std::string(padding_left, ' ') + content + std::string(padding_right, ' ') + "|";
    return {top, mid, top};
}

std::vector<std::string> join_vertical(const std::vector<std::vector<std::string>> &boxes)
{
    std::vector<std::string> joined;
    for (const auto &box : boxes)
    {
        joined.insert(joined.end(), box.begin(), box.end());
    }
    return joined;
}

std::vector<std::string> pad_lines(std::vector<std::string> lines, int height)
{
    int current = lines.size();
    if (current < height)
    {
        int width = lines.empty() ? 0 : lines[0].length();
        for (int i = 0; i < height - current; ++i)
        {
            lines.push_back(std::string(width, ' '));
        }
    }
    return lines;
}

std::vector<std::string> make_cache_box(const std::string &size, int box_w, int height)
{
    std::string top = "+" + std::string(box_w - 2, '-') + "+";
    std::string mid = "|";
    int size_len = size.length();
    int available = box_w - 2;
    int padding_left = (available - size_len) / 2;
    int padding_right = available - size_len - padding_left;

    if (padding_left < 0)
        padding_left = 0;
    if (padding_right < 0)
        padding_right = 0;

    mid += std::string(padding_left, ' ') + size + std::string(padding_right, ' ') + "|";
    std::string empty_line = "|" + std::string(box_w - 2, ' ') + "|";

    std::vector<std::string> box = {top};
    int mid_index = height / 2;
    for (int i = 1; i < height - 1; ++i)
    {
        box.push_back(i == mid_index ? mid : empty_line);
    }
    box.push_back(top);

    return box;
}

void draw_dynamic_socket_layout(int socket_id, const std::vector<std::vector<int>> &core_groups,
                                const std::map<int, std::map<std::string, std::vector<CacheInstance>>> &cache_hierarchy)
{
    int num_cores = core_groups.size();
    std::vector<int> cache_levels;
    for (const auto &level_entry : cache_hierarchy)
    {
        cache_levels.push_back(level_entry.first);
    }
    std::sort(cache_levels.begin(), cache_levels.end());

    int core_label_width = 0;
    for (const auto &core : core_groups)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < core.size(); ++i)
        {
            if (i > 0)
                oss << " ";
            oss << core[i];
        }
        core_label_width = std::max(core_label_width, static_cast<int>(oss.str().length()) + 2);
    }
    int core_box_w = std::max(core_label_width, 9);

    std::vector<std::vector<std::string>> core_boxes;
    for (const auto &core : core_groups)
    {
        std::ostringstream oss;
        std::vector<int> sorted_core = core;
        std::sort(sorted_core.begin(), sorted_core.end());
        for (size_t i = 0; i < sorted_core.size(); ++i)
        {
            if (i > 0)
                oss << " ";
            oss << sorted_core[i];
        }
        core_boxes.push_back(make_box_lines(oss.str(), core_box_w));
    }
    std::vector<std::string> core_column = join_vertical(core_boxes);

    std::vector<std::vector<std::string>> cache_columns;
    std::vector<int> column_widths = {core_box_w};
    std::vector<std::string> headers;

    std::string core_header = " Core(s) ";
    int core_header_len = core_header.length();
    int core_padding_left = (core_box_w - core_header_len) / 2;
    int core_padding_right = core_box_w - core_header_len - core_padding_left;
    if (core_padding_left < 0)
        core_padding_left = 0;
    if (core_padding_right < 0)
        core_padding_right = 0;
    headers.push_back(std::string(core_padding_left, ' ') + core_header + std::string(core_padding_right, ' '));

    for (int level : cache_levels)
    {
        const auto &level_types = cache_hierarchy.at(level);
        std::string size_str;

        if (level_types.find("Unified") != level_types.end())
        {
            size_str = std::to_string(level_types.at("Unified")[0].size_kb) + "K";
        }
        else if (level_types.find("Data") != level_types.end() && level_types.find("Instruction") != level_types.end())
        {
            int d_size = level_types.at("Data")[0].size_kb;
            int i_size = level_types.at("Instruction")[0].size_kb;
            size_str = "d:" + std::to_string(d_size) + "K+i:" + std::to_string(i_size) + "K";
        }
        else
        {
            size_str = std::to_string(level_types.begin()->second[0].size_kb) + "K";
        }

        int box_w = std::max(static_cast<int>(size_str.length()) + 2, 9);
        column_widths.push_back(box_w);

        std::string header = " L" + std::to_string(level) + " Cache ";
        int header_len = header.length();
        int header_padding_left = (box_w - header_len) / 2;
        int header_padding_right = box_w - header_len - header_padding_left;
        if (header_padding_left < 0)
            header_padding_left = 0;
        if (header_padding_right < 0)
            header_padding_right = 0;
        headers.push_back(std::string(header_padding_left, ' ') + header + std::string(header_padding_right, ' '));

        if (level == cache_levels.back())
        {
            cache_columns.push_back(make_cache_box(size_str, box_w, num_cores * 3));
        }
        else
        {
            std::vector<std::vector<std::string>> cache_boxes;
            for (size_t i = 0; i < core_groups.size(); ++i)
            {
                cache_boxes.push_back(make_box_lines(size_str, box_w));
            }
            cache_columns.push_back(join_vertical(cache_boxes));
        }
    }

    std::vector<std::vector<std::string>> all_columns = {core_column};
    all_columns.insert(all_columns.end(), cache_columns.begin(), cache_columns.end());

    int max_height = 0;
    for (const auto &col : all_columns)
    {
        max_height = std::max(max_height, static_cast<int>(col.size()));
    }

    std::vector<std::vector<std::string>> padded_columns;
    for (const auto &col : all_columns)
    {
        padded_columns.push_back(pad_lines(col, max_height));
    }

    std::cout << "\nSocket " << socket_id << ":\n";
    for (size_t i = 0; i < headers.size(); ++i)
    {
        if (i > 0)
            std::cout << " ";
        std::cout << headers[i];
    }
    std::cout << "\n\n";

    for (int i = 0; i < max_height; ++i)
    {
        for (size_t j = 0; j < padded_columns.size(); ++j)
        {
            if (j > 0)
                std::cout << " ";
            std::cout << padded_columns[j][i];
            if (static_cast<int>(padded_columns[j][i].length()) < column_widths[j])
            {
                std::cout << std::string(column_widths[j] - padded_columns[j][i].length(), ' ');
            }
        }
        std::cout << "\n";
    }
}

void print_cache_topology_for_socket_compact(int socket_id, const std::map<int, std::map<std::string, std::vector<CacheInstance>>> &socket_caches)
{
    std::cout << "\n"
              << std::string(60, '=') << "\n";
    std::cout << "Cache Topology Summary for Socket " << socket_id << "\n";
    std::cout << std::string(60, '=') << "\n";

    struct CacheGroupInfo
    {
        std::vector<std::string> shared_cpus_list;
        int shared_threads;
        int count;
    };

    std::map<std::tuple<int, std::string, std::string, std::string, std::string, std::string, std::string, int>, CacheGroupInfo> groups;

    for (const auto &level_entry : socket_caches)
    {
        int level = level_entry.first;
        const auto &types = level_entry.second;
        for (const auto &type_entry : types)
        {
            const std::string &cache_type = type_entry.first;
            const std::vector<CacheInstance> &instances = type_entry.second;
            for (const auto &inst : instances)
            {
                std::string cpu_dir = inst.source.substr(0, inst.source.find('/'));
                std::string index_dir = inst.source.substr(inst.source.find('/') + 1);
                std::string base_path = "/sys/devices/system/cpu/" + cpu_dir + "/cache/" + index_dir;

                std::string associativity = read_cache_attribute(base_path, "ways_of_associativity");
                std::string number_of_sets = read_cache_attribute(base_path, "number_of_sets");
                std::string line_size = read_cache_attribute(base_path, "coherency_line_size");
                std::string inclusive_flag = read_cache_attribute(base_path, "inclusive");
                std::string cache_type_desc = (inclusive_flag == "1") ? "Inclusive" : "Non Inclusive";
                std::string shared_cpus = inst.shared_cpus;

                std::set<int> cpus_set;
                std::istringstream iss(shared_cpus);
                std::string part;
                while (std::getline(iss, part, ','))
                {
                    if (part.find('-') != std::string::npos)
                    {
                        int start = std::stoi(part.substr(0, part.find('-')));
                        int end = std::stoi(part.substr(part.find('-') + 1));
                        for (int i = start; i <= end; ++i)
                        {
                            cpus_set.insert(i);
                        }
                    }
                    else
                    {
                        cpus_set.insert(std::stoi(part));
                    }
                }
                int shared_threads = cpus_set.size();

                int size_kb = inst.size_kb;
                std::string size_str = (size_kb >= 1024) ? std::to_string(size_kb / 1024) + " MB" : std::to_string(size_kb) + " kB";

                std::ostringstream cpu_list;
                bool first = true;
                for (int cpu : cpus_set)
                {
                    if (!first)
                        cpu_list << ",";
                    cpu_list << cpu;
                    first = false;
                }

                auto key = std::make_tuple(level, cache_type, associativity, number_of_sets, line_size, cache_type_desc, size_str, shared_threads);
                groups[key].shared_cpus_list.push_back(cpu_list.str());
                groups[key].shared_threads = shared_threads;
                groups[key].count++;
            }
        }
    }

    for (const auto &group_entry : groups)
    {
        const auto &key = group_entry.first;
        const auto &data = group_entry.second;
        int level = std::get<0>(key);
        std::string cache_type = std::get<1>(key);
        std::string associativity = std::get<2>(key);
        std::string number_of_sets = std::get<3>(key);
        std::string line_size = std::get<4>(key);
        std::string cache_type_desc = std::get<5>(key);
        std::string size_str = std::get<6>(key);
        int shared_threads = std::get<7>(key);

        std::cout << "Level:\t\t\t" << level << "\n";
        std::cout << "Size:\t\t\t" << size_str << "\n";
        std::cout << "Type:\t\t\t" << cache_type << " cache\n";
        std::cout << "Associativity:\t\t" << associativity << "\n";
        std::cout << "Number of sets:\t\t" << number_of_sets << "\n";
        std::cout << "Cache line size:\t" << line_size << "\n";
        std::cout << "Cache type:\t\t" << cache_type_desc << "\n";
        std::cout << "Shared by threads:\t" << shared_threads << "\n";
        std::cout << "Cache groups:\t\t";
        for (size_t i = 0; i < data.shared_cpus_list.size(); ++i)
        {
            if (i > 0)
                std::cout << " ";
            std::cout << "(" << data.shared_cpus_list[i] << ")";
        }
        std::cout << "\n"
                  << std::string(80, '-') << "\n";
    }
}

void print_cache_summary(const CacheHierarchy &cache_hierarchy)
{
    std::cout << "\nCache Information (sum of all sockets):\n";
    std::cout << std::string(60, '=') << "\n";

    printf("%-8s %-15s %-15s %-12s %-15s\n", "Level", "Type", "Unit Size", "Instances", "Total Size");
    std::cout << std::string(60, '=') << "\n";

    std::map<int, std::map<std::string, std::pair<int, int>>> summary;

    for (const auto &socket_entry : cache_hierarchy)
    {
        int socket = socket_entry.first;
        const auto &levels = socket_entry.second;
        for (const auto &level_entry : levels)
        {
            int level = level_entry.first;
            const auto &types = level_entry.second;
            for (const auto &type_entry : types)
            {
                const std::string &cache_type = type_entry.first;
                const std::vector<CacheInstance> &instances = type_entry.second;
                int unit_kb = instances[0].size_kb;
                int count = instances.size();
                summary[level][cache_type].first = unit_kb;
                summary[level][cache_type].second += count;
            }
        }
    }

    for (const auto &level_entry : summary)
    {
        int level = level_entry.first;
        const auto &types = level_entry.second;
        for (const auto &type_entry : types)
        {
            const std::string &cache_type = type_entry.first;
            const std::pair<int, int> &counts = type_entry.second;
            int unit_kb = counts.first;
            int count = counts.second;
            int total_kb = unit_kb * count;

            std::string level_str = "L" + std::to_string(level) + std::string(1, std::tolower(cache_type[0]));
            printf("%-8s %-15s %-15s %-12s %-15s\n",
                   level_str.c_str(),
                   cache_type.c_str(),
                   (std::to_string(unit_kb) + " KiB").c_str(),
                   ("(" + std::to_string(count) + ")").c_str(),
                   (std::to_string(total_kb) + " KiB").c_str());
        }
    }

    for (const auto &socket_entry : cache_hierarchy)
    {
        int socket = socket_entry.first;
        const auto &levels = socket_entry.second;
        print_cache_topology_for_socket_compact(socket, levels);
    }
}

std::string get_gpu_type(const std::string &card_path)
{
    try
    {
        std::string vendor_file = card_path + "/device/vendor";
        std::string vendor_id = optkit::utils::read_file(vendor_file);
        // trim
        while (!vendor_id.empty() && (vendor_id.back() == '\n' || vendor_id.back() == '\r' || vendor_id.back() == ' ' || vendor_id.back() == '\t'))
            vendor_id.pop_back();

        if (vendor_id == "0x10de")
            return "nvidia";
        if (vendor_id == "0x1002")
            return "amd";
        if (vendor_id == "0x8086")
            return "intel";
    }
    catch (...)
    {
    }
    return "unknown";
}
void call_cpu_info_tools()
{
    std::cout << std::string(60, '=') << "\n";
    std::cout << "CPU Topology and Cache Information\n";
    std::cout << std::string(60, '=') << "\n";

    Topology topology = get_system_topology();
    CacheHierarchy cache_hierarchy = get_cache_hierarchy();

    std::map<int, std::map<int, std::vector<int>>> sockets;
    for (const auto &entry : topology)
    {
        int socket = entry.first.first;
        int core = entry.first.second;
        const std::vector<int> &cpus = entry.second;
        sockets[socket][core] = cpus;
        std::sort(sockets[socket][core].begin(), sockets[socket][core].end());
    }

    for (const auto &socket_entry : sockets)
    {
        int socket = socket_entry.first;
        const std::map<int, std::vector<int>> &cores = socket_entry.second;
        std::vector<std::vector<int>> core_groups;
        for (const auto &core_entry : cores)
        {
            core_groups.push_back(core_entry.second);
        }
        draw_dynamic_socket_layout(socket, core_groups, cache_hierarchy[socket]);
    }
    print_cache_summary(cache_hierarchy);
}

void call_gpu_info_tools()
{
    std::cout << "\n"
              << std::string(60, '=') << "\n";
    std::cout << "Detailed GPU Info\n";
    std::cout << std::string(60, '=') << "\n";

    {
        std::vector<std::string> entries = optkit::utils::get_all_files("/sys/class/drm");
        for (size_t i = 0; i < entries.size(); ++i)
        {
            std::string filename = entries[i];
            if (filename.find("card") != 0 || filename.find('-') != std::string::npos)
                continue;

            std::string card_path = std::string("/sys/class/drm/") + filename;
            std::string gpu_type = get_gpu_type(card_path);

            std::cout << "\nGPU Card: " << filename << " | Type: ";
            for (char c : gpu_type)
                std::cout << static_cast<char>(std::toupper(c));
            std::cout << "\n";

            if (gpu_type == "nvidia")
            {
                int ret = system("nvidia-smi 2>/dev/null");
                if (ret != 0)
                {
                    std::cout << "NVIDIA tool not found. Please install it.\n";
                }
            }
            else if (gpu_type == "amd")
            {
                int ret = system("rocm-smi 2>/dev/null");
                if (ret != 0)
                {
                    std::cout << "AMD tool not found. Please install it.\n";
                }
            }
            else if (gpu_type == "intel")
            {
                std::cout << "Intel GPU detected. Use `intel_gpu_top` or check /sys/class/drm for stats.\n";
            }
            else
            {
                std::cout << "Unknown GPU type. Manual inspection required.\n";
            }
        }
    }
}