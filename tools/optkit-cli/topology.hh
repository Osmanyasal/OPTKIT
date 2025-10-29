#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <regex>
#include <cstdlib>
#include <array>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.hh"

struct CacheInstance
{
    int size_kb;
    std::string shared_cpus;
    std::string source;
};

using Topology = std::map<std::pair<int, int>, std::vector<int>>;                                       // socket -> core -> list of CPUs
using CacheHierarchy = std::map<int, std::map<int, std::map<std::string, std::vector<CacheInstance>>>>; // socket -> level -> type -> list of CacheInstances

Topology get_system_topology();
CacheHierarchy get_cache_hierarchy();
std::string read_cache_attribute(const std::string &base_path, const std::string &attribute);
std::vector<std::string> make_box_lines(const std::string &content, int width = 9);
std::vector<std::string> join_vertical(const std::vector<std::vector<std::string>> &boxes);
std::vector<std::string> pad_lines(std::vector<std::string> lines, int height);
std::vector<std::string> make_cache_box(const std::string &size, int box_w, int height);
void draw_dynamic_socket_layout(int socket_id, const std::vector<std::vector<int>> &core_groups,
                                const std::map<int, std::map<std::string, std::vector<CacheInstance>>> &cache_hierarchy);
void print_cache_topology_for_socket_compact(int socket_id, const std::map<int, std::map<std::string, std::vector<CacheInstance>>> &socket_caches);
void print_cache_summary(const CacheHierarchy &cache_hierarchy);
std::string get_gpu_type(const std::string &card_path);
void call_cpu_info_tools();
void call_gpu_info_tools();
