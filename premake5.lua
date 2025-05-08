---@diagnostic disable: undefined-global

print("Current platform: " .. os.target())
-- Check if the platform is Linux
if os.target() ~= "linux" then
    print("❌ This script is only supported on Linux platforms.")
    os.exit(1) -- Exit with a non-zero status to terminate the script
end


local LIB_MSR_SAFE_PATH = './lib/msr-safe'
local LIB_SPD_PATH = './lib/spdlog'
local LIB_PFM_PATH = './lib/libpfm4'
local CORE_EVENTS_DIR = './src/core/events'
local UTILS_DIR = './src/utils'
local LIB_X86_ADAPT_PATH = './lib/x86-adapt'
local WHOAMI = io.popen("whoami"):read("*a"):gsub("\n", "")

local OPTKIT_APP = "OptimizerToolkitApp"
local OPTKIT_LIB_DYNAMIC = "OptimizerToolkitShared"
local OPTKIT_LIB_STATIC = "OptimizerToolkitStatic"


function BaseProjectSetup()
     
    language "C++"
    cppdialect "C++11"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj"

    includedirs {
        "./src/"
    }

    files {
        "./src/**.cc",
        "./src/**.hh"
    }

    -- Linking dynamic libraries for Intel systems
    linkoptions { "-fopenmp" }
    links { "pthread", "dl", "tensorflow", "spdlog", "pfm" }

-- Compiler options
    filter "configurations:Debug"
        symbols "On"
        defines { "OPTKIT_MODE_DEBUG" }
        buildoptions { "-Wall", "-O0", "-g", "-fopenmp", "-msse", "-march=native" }

    filter "configurations:Release"
        optimize "On"
        symbols "Off"
        defines { "OPTKIT_MODE_NDEBUG" }
        buildoptions { "-Wall", "-O2", "-fopenmp", "-msse", "-march=native" }

    filter {} -- stop filtering, below is globally accessible


    prebuildcommands {
        -- Use global variables in the prebuildcommands

        "@echo [CHECK MSR-SAFE]",
        "if [ ! -f " .. LIB_MSR_SAFE_PATH .. "/all_set ]; then \\",
        "    cd " .. LIB_MSR_SAFE_PATH .. " && make clean && make && make install && (sudo rmmod msr-safe || true) && \\",
        "    (sudo insmod ./msr-safe.ko || true) && sudo chmod g+rw /dev/cpu/*/msr_safe /dev/cpu/msr_* && \\",
        "    sudo chgrp " .. WHOAMI .. " /dev/cpu/*/msr_safe /dev/cpu/msr_batch /dev/cpu/msr_safe_version && \\",
        "    sudo chgrp " .. WHOAMI .. " /dev/cpu/msr_allowlist && \\",
        "    echo \"0x00000620 0xFFFFFFFFFFFFFFFF\" > /dev/cpu/msr_allowlist && \\",
        "    touch all_set; \\",
        "fi",

        "@echo [CHECK SPDLOG]",
        "if [ ! -f " .. LIB_SPD_PATH .. "/build/libspdlog.a ]; then \\",
        "    cd " .. LIB_SPD_PATH .. " && ./install.sh; \\",
        "fi",

        "@echo [CHECK LIBPFM]",
        "if [ ! -f " .. LIB_PFM_PATH .. "/all_set ]; then \\",
        "    cd " .. LIB_PFM_PATH .. " && ./install.sh; \\",
        "fi",

        "@echo [CHECK EVENTS]",
        "if [ ! -f " .. CORE_EVENTS_DIR .. "/all_set ]; then \\",
        "    echo \"⛏️ Exporting events from libpfm4\" && \\",
        "    mkdir -p " .. CORE_EVENTS_DIR .. " && \\",
        "    cd " .. UTILS_DIR .. " && \\",
        "    python3 pmu_parser.py $(shell find " .. LIB_PFM_PATH .. "/lib/events -type f \\( -name \"intel*.h\" -or -name \"amd*.h\" -or -name \"arm*.h\" -or -name \"power*.h\" \\) -exec echo \"../../{}\" \\;) && \\",
        "    touch ../../" .. CORE_EVENTS_DIR .. "/all_set; \\",
        "fi",
    } 

    local actions = {
        clean = "clean",          -- clean the optkit build.
        return0 = "return0",      -- clean the build, events, and libs, simply un-build everything
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
            print("[CLEANING] ./bin")
            os.rmdir("./bin")

            print("[CLEANING] ./build")
            os.rmdir("./build")

            print("[REMOVE] Makefile")
            os.remove("Makefile")

            print("[REMOVE] "..OPTKIT_APP..".make")
            os.remove(OPTKIT_APP..".make")

            print("[REMOVE] "..OPTKIT_LIB_DYNAMIC..".make")
            os.remove(OPTKIT_LIB_DYNAMIC..".make")

            print("[REMOVE] lib"..OPTKIT_LIB_STATIC..".make")
            os.remove(OPTKIT_LIB_STATIC..".make")

            print("🧹 Cleaned build directories!")
        end
    }

    newaction {
        trigger = actions.return0,
        description = "un-build everything",
        execute = function()
            print("[CLEANING]: ./bin")
            os.rmdir("./bin")

            print("[CLEANING]: ./build")
            os.rmdir("./build")

            print("[CLEANING]: ./src/core/events")
            os.rmdir("./src/core/events")

            print("[CLEANING]: " .. LIB_SPD_PATH)
            os.execute("cd " .. LIB_SPD_PATH .. " && ./install.sh clean")

            print("[CLEANING]: " .. LIB_PFM_PATH)
            os.execute("cd " .. LIB_PFM_PATH .. " && ./install.sh clean")

            print("[CLEANING]: " .. LIB_MSR_SAFE_PATH)
            os.execute("cd " .. LIB_MSR_SAFE_PATH .. " && make clean && rm all_set && sudo rmmod msr-safe")

            print("[REMOVE]: Makefile")
            os.remove("Makefile")

            print("[REMOVE]: OptimizerToolkit.make")
            os.remove("OptimizerToolkit.make")

            print("[Cleaned 🧹]: build directories!")
        end
    }

    newaction {
        trigger = actions.install,
        description = "Install headers and libraries to system directories",
        execute = function()
            -- Check if the Release directory exists
            if not os.isdir("./bin/Release") then
                print("❌ Release directory not found!")
                return
            end

            print("[Installing]: headers and libraries!")
            os.execute("sudo rm -rf /usr/local/include/optkit/ && sudo mkdir -p /usr/local/include/optkit")                                                                   -- create optkit directory for headers
            os.execute("cd ./src; sudo find ./ -type f -name \"*.hh\" -exec cp --parents {} \"/usr/local/include/optkit/\" \\;")    -- copy all header files by keeping the file structure as-is
            os.execute("sudo cp -R ./bin/Release/lib" ..OPTKIT_LIB_STATIC..".a /usr/local/lib")                                        -- copy static library
            os.execute("sudo cp -R ./bin/Release/lib"..OPTKIT_LIB_DYNAMIC..".so /usr/local/lib")                                       -- copy dynamic library
            print("[Installed ✅]: headers and libraries!")
            
            -- BUILD TOOLS AND ALSO INSTALL THEM, TOOLS WILL BE USING THE STATIC-DYNAMIC LIBRARY THAT'S INSTALLED.
            print("[Installing]: utility tools!")
            os.execute("cd ./tools && ./install.sh")
            print("[Installed ✅]: utility tools!")
        end
    }

    newaction {
        trigger = actions.remove,
        description = "Remove OPTKIT from the system. (deletes all OPTKIT-cli and libraries from the system)",
        execute = function()
            print("[Removing]: OPTKIT from the system")
            os.execute("rm -rf /usr/local/include/optkit") -- removes optkit headers
            os.execute("rm -f /usr/local/bin/optkit*")     -- removes optkit binaries
            os.execute("rm -f /usr/local/lib/liboptkit.a") -- removes optkit library
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

workspace "OptimizerToolkit"
    configurations { "Debug", "Release" }
    architecture "x64"

    project (OPTKIT_APP)
        kind "ConsoleApp"
        BaseProjectSetup()

    project (OPTKIT_LIB_DYNAMIC)
        kind "SharedLib"
        BaseProjectSetup()
        removefiles{"./src/main.cc"}

    project (OPTKIT_LIB_STATIC)
        kind "StaticLib"
        BaseProjectSetup()
        removefiles{"./src/main.cc"}