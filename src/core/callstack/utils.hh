#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <dirent.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace optkit::callstack
{
    namespace proc
    {
        inline int32_t effective_pid(int32_t pid)
        {
            return (pid != 0) ? pid : static_cast<int32_t>(::getpid());
        }

        inline std::string task_dir(int32_t pid)
        {
            return "/proc/" + std::to_string(effective_pid(pid)) + "/task";
        }

        inline std::string maps_path(int32_t pid)
        {
            return "/proc/" + std::to_string(effective_pid(pid)) + "/maps";
        }
    } // namespace proc
}