---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

function base_project_setup()
    language "C++"
    cppdialect "C++11"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj"

    includedirs {
        "./src/",
        LIB_PFM_PATH .. "/include",
        LIB_SPD_PATH .. "/include"
    }

    files {
        "./src/**.cc",
        "./src/**.hh"
    }

    removefiles
    {
        "./src/core/**/*_governor.cc",
        "./src/core/**/*_governor.hh",
    }

    -- Always needed
    links { "pthread", "dl" }
    linkoptions { "-fopenmp" }

    -- Always link static spdlog manually
    linkoptions { LIB_SPD_PATH .. "/build/libspdlog.a" }

    -- Compiler options
    filter "configurations:Debug"
    symbols "On"
    defines { "OPTKIT_MODE_DEBUG" }
    buildoptions {
        "-Wall",
        "-O0",
        "-g",
        "-fopenmp",
        "-fPIC",
        "-msse",
        "-march=native",
        "-DCONF_LOG_PRINT_GUID_LENGTH=5",
        "-DCONF_LOG_DISABLE_DEBUG=0",
        "-DCONF_LOG_DISABLE_TRACE=0",
        "-DCONF_LOG_DISABLE_INFO=0",
        "-DCONF_LOG_DISABLE_WARN=0",
        "-DCONF_LOG_DISABLE_ERROR=0"
    }
    filter {} -- stop filtering

    filter "configurations:Release"
    optimize "On"
    symbols "Off"
    defines { "OPTKIT_MODE_NDEBUG" }
    buildoptions { 
        "-Wall", 
        "-O2", 
        "-fopenmp", 
        "-fPIC", 
        "-msse", 
        "-march=native",
        "-DCONF_LOG_PRINT_GUID_LENGTH=5",
        "-DCONF_LOG_DISABLE_DEBUG=1",
        "-DCONF_LOG_DISABLE_TRACE=1",
        "-DCONF_LOG_DISABLE_INFO=0",
        "-DCONF_LOG_DISABLE_WARN=0",
        "-DCONF_LOG_DISABLE_ERROR=0" }
    filter {} -- stop filtering

    prebuildcommands {

        -- "@echo [CHECK MSR-SAFE]",
        -- "if [ ! -f " .. LIB_MSR_SAFE_PATH .. "/all_set ]; then \\",
        -- "    cd " .. LIB_MSR_SAFE_PATH .. " && make clean && make && make install && (sudo rmmod msr-safe || true) && \\",
        -- "    (sudo insmod ./msr-safe.ko || true) && sudo chmod g+rw /dev/cpu/*/msr_safe /dev/cpu/msr_* && \\",
        -- "    sudo chgrp " .. WHOAMI .. " /dev/cpu/*/msr_safe /dev/cpu/msr_batch /dev/cpu/msr_safe_version && \\",
        -- "    sudo chgrp " .. WHOAMI .. " /dev/cpu/msr_allowlist && \\",
        -- "    echo \"0x00000620 0xFFFFFFFFFFFFFFFF\" > /dev/cpu/msr_allowlist && \\",
        -- "    touch all_set; \\",
        -- "fi",

        -- "@echo [COMPILE SPDLOG]",
        -- "if [ ! -f " .. LIB_SPD_PATH .. "/build/libspdlog.a ]; then \\",
        -- "    cd " .. LIB_SPD_PATH .. " && ./compile.sh; \\",
        -- "fi",
        -- "@echo [✅ COMPILE SPDLOG]",

        -- SPDLOG compilation
        -- "@echo [COMPILE SPDLOG]",
        "@if [ ! -f \"" .. LIB_SPD_PATH .. "/build/libspdlog.a\" ]; then cd " .. LIB_SPD_PATH .. " && ./compile.sh; fi && echo [✅ COMPILE SPDLOG] || echo [❌ COMPILE SPDLOG ERROR]",

        -- LIBPFM compilation
        -- "@echo [COMPILE LIBPFM]",
        "@if [ ! -f \"" .. LIB_PFM_PATH .. "/lib/libpfm.a\" ]; then cd " .. LIB_PFM_PATH .. " && ./compile.sh; fi && echo [✅ COMPILE LIBPFM] || echo [❌ COMPILE LIBPFM ERROR]",

        -- Check and export events from libpfm4
        -- "@echo [CHECK EVENTS]",
        "@if [ ! -f " .. CORE_EVENTS_DIR .. "/all_set ]; then \\",
        "    echo \"⛏️ Exporting events from libpfm4\" && \\",
        "    mkdir -p " .. CORE_EVENTS_DIR .. " && \\",
        "    cd " .. UTILS_DIR .. " && \\",
        "    python3 pmu_parser.py $(shell find " .. LIB_PFM_PATH .. "/lib/events -type f \\( -name \"intel*.h\" -or -name \"amd*.h\" -or -name \"arm*.h\" -or -name \"power*.h\" \\) -exec echo \"../../{}\" \\;) && \\",
        "    touch ../../" .. CORE_EVENTS_DIR .. "/all_set;\\",
        "fi && \\",
        "echo [✅ CHECK EVENTS] || echo [❌ CHECK EVENTS ERROR]",
    }
end

function test_project_setup()
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj"

    files {
        "./test/***test.cc",
        "./test/***test.hh"
    }

    includedirs {
        "./test/",
        LIB_PFM_PATH .. "/include",
        LIB_SPD_PATH .. "/include",
        LIB_GOOGLETEST_PATH .. "/googletest/include",
        LIB_GOOGLETEST_PATH .. "/googlemock/include",
    }

    libdirs {
        LIB_GOOGLETEST_PATH .. "/build/lib"
    }

    links {
        "gtest", "gtest_main", "pthread"
    }

    linkoptions { "-fopenmp" }

    filter "configurations:Release"
    optimize "On"
    symbols "Off"
    defines { "OPTKIT_MODE_NDEBUG" }
    buildoptions { "-Wall", "-O2", "-fopenmp", "-fPIC", "-msse", "-march=native -funroll-loops -ftree-vectorize -fopt-info-vec" }

    prebuildcommands
    {
        -- compile googletest and load its static library
        "@if [ ! -f \"" .. LIB_GOOGLETEST_PATH .. "/build/lib/libgtest.a\" ]; then cd " ..
        LIB_GOOGLETEST_PATH .. " && git branch v1.17.0 && mkdir build && cd build && cmake .. && make -j$(nproc);" ..
        "fi && echo [✅ COMPILE SPDLOG] || echo [❌ COMPILE SPDLOG ERROR]",
    }
end
