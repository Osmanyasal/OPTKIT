---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

function system_checks()
    print("Current platform: " .. os.target())
    -- Check if the platform is Linux
    if os.target() ~= "linux" then
        print("❌ This script is only supported on Linux platforms.")
        os.exit(1) -- Exit with a non-zero status to terminate the script
    end
end

function ld_has_library(libname)
    local pipe = io.popen("ldconfig -p 2>/dev/null | grep lib" .. libname .. ".so")
    if not pipe then return false end

    local result = pipe:read("*a")
    pipe:close()

    return result ~= nil and result ~= ""
end

-- Use global variables in the prebuildcommands

local LIB_MSR_SAFE_PATH = './lib/msr-safe'
local LIB_SPD_PATH = './lib/spdlog'
local LIB_PFM_PATH = './lib/libpfm4'
local CORE_EVENTS_DIR = './src/core/events'
local UTILS_DIR = './src/utils'
local LIB_GOOGLETEST_PATH = "./lib/googletest"
local LIB_X86_ADAPT_PATH = './lib/x86-adapt'
local WHOAMI = io.popen("whoami"):read("*a"):gsub("\n", "")

local OPTKIT_APP = "optkit_app"
local OPTKIT_LIB_DYNAMIC = "optkit_dynamic"
local OPTKIT_LIB_STATIC = "optkit_static"


function define_custom_actions()
    local action = _ACTION
    if action ~= "clean" then
        print("🛠️ Generating environment config...")
        os.execute("./generate_environment_config.sh")
    end

    local actions = {
        clean = "clean",          -- clean the optkit build.
        install = "install",      -- build & install libs
        remove = "remove",        -- remove installed libs from the system
        gen_doc = "gen-doc",      -- generate documentaiton
        remove_doc = "remove-doc" -- delete documentaiton
    }

    -- Tasks for clean, install, and generate docs
    newaction {
        trigger = actions.clean,
        description = "clean bin and build directory",
        execute = function()
            print("[REMOVE]: ./bin")
            os.rmdir("./bin")

            print("[REMOVE]: ./build")
            os.rmdir("./build")

            print("[REMOVE]: ./Makefile")
            os.remove("./Makefile")

            print("[REMOVE]: ./src/environment_config.hh")
            os.remove("./src/environment_config.hh")

            print("[REMOVE]: " .. CORE_EVENTS_DIR)
            os.rmdir(CORE_EVENTS_DIR)

            print("[REMOVE]: " .. OPTKIT_APP .. ".make")
            os.remove(OPTKIT_APP .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_DYNAMIC .. ".make")
            os.remove(OPTKIT_LIB_DYNAMIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_STATIC .. ".make")
            os.remove(OPTKIT_LIB_STATIC .. ".make")

            print("🧹 Cleaned build directories!")
        end
    }

    newaction {
        trigger = actions.install,
        description = "Install OPTKIT headers and libs + dependencies to system directories",
        execute = function()
            -- Check if the Release directory exists
            if not os.isdir("./bin/Release") then
                print("❌ Release directory not found! Only config=release builds can be installed to the system.")
                return
            end

            print("[Installing]: SPDLOG headers")
            os.execute(
                "cd " ..
                LIB_SPD_PATH ..
                "/ && ./compile.sh && sudo cp -R ./include/spdlog /usr/local/include/ && sudo cp ./build/libspdlog.a /usr/local/lib")

            print("[Installing]: PFM4 headers")
            os.execute("cd " .. LIB_PFM_PATH .. "/ && ./compile.sh && sudo make install && sudo make install-all")

            -- print("[Installing]: MSR-SAFE")
            -- os.execute("cd ./lib/msr-safe/ && ./compile.sh && ./sudo cp -R ./include/spdlog /usr/local/include/")

            print("[Installing]: headers and libraries!")
            os.execute("sudo rm -rf /usr/local/include/optkit/ && sudo mkdir -p /usr/local/include/optkit")               -- create optkit directory for headers
            os.execute(
                "cd ./src; sudo find ./ -type f -name \"*.hh\" -exec cp --parents {} \"/usr/local/include/optkit/\" \\;") -- copy all header files by keeping the file structure as-is
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_STATIC .. ".a /usr/local/lib")                        -- copy static library
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_DYNAMIC .. ".so /usr/local/lib")                      -- copy dynamic library
            print("[✅ Installed]: headers and libraries!")

            -- BUILD TOOLS AND ALSO INSTALL THEM, TOOLS WILL BE USING THE STATIC-DYNAMIC LIBRARY THAT'S INSTALLED.
            print("[Installing]: utility tools!")
            os.execute("cd ./tools && ./install.sh")
            print("[✅ Installed]: utility tools!")
        end
    }

    newaction {
        trigger = actions.remove,
        description = "Remove OPTKIT from the system. (deletes all OPTKIT-cli and libraries from the system)",
        execute = function()
            print("[Removing]: OPTKIT from the system")
            os.execute("sudo rm -rf /usr/local/include/optkit") -- removes optkit headers
            os.execute("sudo rm -f /usr/local/bin/optkit*")     -- removes optkit binaries
            os.execute("sudo rm -f /usr/local/lib/liboptkit.a") -- removes optkit library
            print("[Removed 🧹]: OPTKIT from the system")
        end
    }

    newaction {
        trigger = actions.gen_doc,
        description = "Generate documentation",
        execute = function()
            os.execute("doxygen ./doxyfile") -- create doxygen file
            print("📄 Documentation generated!")
        end
    }

    newaction {
        trigger = actions.remove_doc,
        description = "Remove documentation",
        execute = function()
            os.execute("rm -rf ./doc") -- create doxygen file
            print("📄 Documentation Removed!")
        end
    }
end

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
    buildoptions { "-Wall", "-O0", "-g", "-fopenmp", "-fPIC", "-msse", "-march=native" }
    filter {} -- stop filtering

    filter "configurations:Release"
    optimize "On"
    symbols "Off"
    defines { "OPTKIT_MODE_NDEBUG" }
    buildoptions { "-Wall", "-O2", "-fopenmp", "-fPIC", "-msse", "-march=native" }
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

system_checks()
define_custom_actions()

workspace "OPTKIT"
configurations { "Debug", "Release" }
-- architecture "x86_64"

project(OPTKIT_APP)
kind "ConsoleApp"
base_project_setup()
linkoptions { LIB_PFM_PATH .. "/lib/libpfm.a" }

project(OPTKIT_LIB_DYNAMIC)
kind "SharedLib"
base_project_setup()
libdirs { LIB_PFM_PATH .. "/lib" }   -- so that pfm.so can be found
links { "pfm" }
removefiles { "./src/main.cc" }

project(OPTKIT_LIB_STATIC)
kind "StaticLib"
base_project_setup()
removefiles { "./src/main.cc" }
linkoptions { LIB_PFM_PATH .. "/lib/libpfm.a" }
filter { "configurations:Release" }
postbuildcommands {
    "@echo [COMPILE UTILITY TOOLS]",
    "@cd ./tools && ./compile.sh && echo [✅ COMPILE UTILITY TOOLS]"
}
filter {}     -- stop filtering

project(OPTKIT_TEST)
kind "ConsoleApp"
language "C++"
cppdialect "C++17"
targetdir "bin/%{cfg.buildcfg}"
objdir "bin/obj"

files {
    "./test/**.cc",
    "./test/**.hh"
}

includedirs {
    "./test/",
    "./src/",
    LIB_GOOGLETEST_PATH .. "/googletest/include",
    LIB_GOOGLETEST_PATH .. "/googlemock/include"
}

libdirs {
    LIB_GOOGLETEST_PATH .. "/build/lib"
}

links {
    "gtest", "gtest_main", "pthread"
}

linkoptions { "-fopenmp" }

filter "configurations:Debug"
symbols "On"
defines { "OPTKIT_MODE_DEBUG", "GTEST_DEBUG" }
buildoptions { "-Wall", "-O0", "-g", "-fopenmp", "-fPIC", "-msse", "-march=native" }

filter "configurations:Release"
optimize "On"
symbols "Off"
defines { "OPTKIT_MODE_NDEBUG" }
buildoptions { "-Wall", "-O2", "-fopenmp", "-fPIC", "-msse", "-march=native" }
