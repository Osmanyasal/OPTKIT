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
removefiles { "./src/main.cc" }
-- Force use of the local shared library to avoid linking against system static libpfm.a (non-PIC)
linkoptions { path.getabsolute(LIB_PFM_PATH .. "/lib/libpfm.so") }

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
-- Reset filter to avoid affecting other sections
filter {}

project(OPTKIT_PY)
kind "SharedLib"
base_project_setup()
targetprefix ""
targetextension ".so"

includedirs { "lib/pybind11/include" }

-- We only want to compile the binding file here, the rest comes from the static lib
removefiles { "./src/**.cc" }
files { "src/optkit_py.cc" }
links { OPTKIT_LIB_DYNAMIC }

filter "system:linux"
local python_flags = os.outputof("python3-config --cflags")
local python_suffix = os.outputof("python3-config --extension-suffix")

if python_flags then
    buildoptions { python_flags }
end

if python_suffix then
    targetextension(python_suffix:gsub("%s+", ""))
end
filter {}

project(OPTKIT_C)
kind "SharedLib"
base_project_setup()

-- Build only the C wrapper; the core implementation comes from the main shared library.
removefiles { "./src/**.cc" }
files { "src/optkit_c.cc" }
links { OPTKIT_LIB_DYNAMIC }

filter "system:linux"
targetextension ".so"
filter {}

project(OPTKIT_TEST)
kind "ConsoleApp"
test_project_setup()
