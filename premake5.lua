---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

dofile("premake5_utilities.lua")
dofile("premake5_libsetup.lua")
dofile("premake5_actions.lua")

system_checks()
system_init()
define_custom_actions()

workspace "OPTKIT"
configurations { "Debug", "Release", "Test" }
-- architecture "x86_64" -- this doesn't have to be. It can be risc-v, arm, etc.

project(OPTKIT_APP)
kind "ConsoleApp"
base_project_setup()
linkoptions { LIB_PFM_PATH .. "/lib/libpfm.a" }

project(OPTKIT_LIB_DYNAMIC)
kind "SharedLib"
base_project_setup()
libdirs { LIB_PFM_PATH .. "/lib" } -- so that pfm.so can be found, we link this agains shared pfm
links { "pfm" }
removefiles { "./src/main.cc" }

project(OPTKIT_LIB_STATIC)
kind "StaticLib"
base_project_setup()
removefiles { "./src/main.cc" }
-- linkoptions { LIB_PFM_PATH .. "/lib/libpfm.a" } -- this one doesn't work with StaticLib
filter { "configurations:Release or configurations:Test" }
postbuildcommands {
    -- Create object extraction directory
    "mkdir -p bin/obj/pfm_extract",

    -- Extract objects from libpfm.a
    "cd bin/obj/pfm_extract && ar -x " .. path.getabsolute(LIB_PFM_PATH .. "/lib/libpfm.a"),

    -- Merge libpfm objects into static library
    "cd bin/obj/pfm_extract && ar -r ../../%{cfg.buildcfg}/liboptkit_static.a *.o",

    -- Optional cleanup
    "rm -rf bin/obj/pfm_extract"
}

-- Add tool build step only for Release
filter { "configurations:Release" }
postbuildcommands {
    "@echo [COMPILE UTILITY TOOLS]",
    "@cd ./tools && ./compile.sh && echo [✅ COMPILE UTILITY TOOLS]"
}
-- Reset filter to avoid affecting other sections
filter {}

project(OPTKIT_TEST)
kind "ConsoleApp"
test_project_setup()