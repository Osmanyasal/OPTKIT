---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

dofile("premake5_utilities.lua")
dofile("premake5_libsetup.lua")
dofile("premake5_actions.lua")

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
libdirs { LIB_PFM_PATH .. "/lib" } -- so that pfm.so can be found
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
filter {} -- stop filtering


project(OPTKIT_TEST)
kind "ConsoleApp"
test_project_setup()