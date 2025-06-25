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
        "-DCONF_LOG_PRINT_GUID_LENGTH=10",
        "-DCONF_LOG_DISABLE_DEBUG=0",
        "-DCONF_LOG_DISABLE_TRACE=0",
        "-DCONF_LOG_DISABLE_INFO=0",
        "-DCONF_LOG_DISABLE_WARN=0",
        "-DCONF_LOG_DISABLE_ERROR=0"

        --TODO: DOPTKIT_TESTING set this for testing
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
        "-DCONF_LOG_PRINT_GUID_LENGTH=10",
        "-DCONF_LOG_DISABLE_DEBUG=1",
        "-DCONF_LOG_DISABLE_TRACE=1",
        "-DCONF_LOG_DISABLE_INFO=0",
        "-DCONF_LOG_DISABLE_WARN=0",
        "-DCONF_LOG_DISABLE_ERROR=0" }
    filter {} -- stop filtering


    filter "configurations:Test"
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
        "-DCONF_LOG_PRINT_GUID_LENGTH=10",
        "-DCONF_LOG_DISABLE_DEBUG=1",
        "-DCONF_LOG_DISABLE_TRACE=1",
        "-DCONF_LOG_DISABLE_INFO=0",
        "-DCONF_LOG_DISABLE_WARN=0",
        "-DCONF_LOG_DISABLE_ERROR=0",
        "-DOPTKIT_TESTING=1" }
    filter {} -- stop filtering
end

function test_project_setup()
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj"

    files {
        "./test/***test.cc",
        "./test/***test.hh",
        "./test/common/**.cc"
    }

    includedirs {
        "./src",
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
        "gtest", "gtest_main", "pthread", "dl"
    }
    linkoptions { "-fopenmp" }
    linkoptions { "./bin/Test/liboptkit_static.a" }

    -- filter "configurations:Release"
    -- optimize "On"
    -- symbols "Off"
    -- defines { "OPTKIT_MODE_NDEBUG" }
    -- buildoptions { "-Wall", "-O2", "-fopenmp", "-fPIC", "-msse", "-march=native -funroll-loops -ftree-vectorize -fopt-info-vec" }

    filter "configurations:Debug"
    optimize "Off"
    symbols "Off"
    defines { "OPTKIT_MODE_NDEBUG" }
    buildoptions {
        "-Wall",        -- Enable all warnings
        "-O0",          -- Explicitly no optimization
        "-fopenmp",     -- Enable OpenMP if needed
        "-fPIC",        -- Position-independent code,
        "-DOPTKIT_TESTING=1"
    }
end