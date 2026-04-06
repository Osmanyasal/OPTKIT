---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

function define_custom_actions()
    local custom_actions = {
        clean = "clean",               -- clean the optkit build.
        install = "install",           -- build & install libs
        remove = "remove",             -- remove installed libs from the system
        carm = "carm",                 -- run native roofline benchmarks
        generate_doc = "generate_doc", -- generate documentaiton
        remove_doc = "remove_doc",     -- delete documentaiton
        gmake = "gmake",               -- only gmake is enabled as build tool.
    }

    -- Tasks for clean, install, and generate docs
    newaction {
        trigger = custom_actions.clean,
        description = "clean bin and build directory",
        execute = function()
            print("[REMOVE]: ./bin")
            os.rmdir("./bin")

            print("[REMOVE]: ./build")
            os.rmdir("./build")

            print("[REMOVE]: ./Makefile")
            os.remove("./Makefile")

            print("[REMOVE]: ./src/utils/environment_config.hh")
            os.remove("./src/utils/environment_config.hh")

            -- print("[REMOVE]: ./test/utils/environment_config.hh")
            -- os.remove("./test/utils/environment_config.hh")

            print("[REMOVE]: " .. CPU_PMU_EVENTS_DIR)
            os.rmdir(CPU_PMU_EVENTS_DIR)

            -- print("[REMOVE]: " .. OPTKIT_APP .. ".make")
            -- os.remove(OPTKIT_APP .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_DYNAMIC .. ".make")
            os.remove(OPTKIT_LIB_DYNAMIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_STATIC .. ".make")
            os.remove(OPTKIT_LIB_STATIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_TEST .. ".make")
            os.remove(OPTKIT_TEST .. ".make")

            print("[REMOVE]: " .. OPTKIT_PY .. ".make")
            os.remove(OPTKIT_PY .. ".make")

            print("[REMOVE]: " .. OPTKIT_C .. ".make")
            os.remove(OPTKIT_C .. ".make")

            print("🧹 Cleaned build directories!")
        end
    }

    newaction {
        trigger = custom_actions.install,
        description = "Install OPTKIT headers and libs + dependencies to system directories",
        execute = function()
            -- Check if the Release build exists
            if not os.isdir("./bin/Release") then
                print("❌ Release directory not found! Only config=release builds can be installed to the system.")
                os.exit(1);
            end


            local lib_static = "./bin/Release/lib" .. OPTKIT_LIB_STATIC .. ".a"
            local lib_dynamic = "./bin/Release/lib" .. OPTKIT_LIB_DYNAMIC .. ".so"
            local has_dynamic = os.isfile(lib_dynamic)
            local has_static = os.isfile(lib_static)

            if not has_dynamic and not has_static then
                print("❌ Neither 'liboptkit_dynamic.so' nor 'liboptkit_static.a' found in ./bin/Release.")
                print("   Please build the project with `config=release` before installation.")
                os.exit(1);
            end

            -- -- Generating and installing documentation
            -- os.execute("rm -rf ./doc") -- removes doxygen file
            -- print("📄 Documentation Removed!")

            -- os.execute("doxygen ./doxyfile") -- create doxygen file
            -- print("📄 Documentation generated!")

            -- Check if spdlog installed globally, if not install it. (actually, no need to install it since it is statically linked already in both cases.)
            -- if dynamic_lib_exists("spdlog") or static_lib_exists("spdlog") then
            --     print("✅ spdlog is already installed globally.")
            -- else
            --     print("[Installing]: SPDLOG headers and static library")
            --     os.execute(
            --         "cd " ..
            --         LIB_SPD_PATH ..
            --         "/ && ./compile.sh && sudo cp -R ./include/spdlog /usr/local/include/ && sudo cp ./build/libspdlog.a /usr/local/lib")
            -- end

            -- Check if libpfm installed globally, if not install it.
            if dynamic_lib_exists("pfm") or static_lib_exists("pfm") then
                print("✅ libpfm4 already installed globally.")
            else
                print("[Installing]: PFM4 headers")
                os.execute("cd " .. LIB_PFM_PATH .. "/ && ./compile.sh && sudo make install && sudo make install-all")
            end

            -- print("[Installing]: MSR-SAFE")
            -- os.execute("cd ./lib/msr-safe/ && ./compile.sh && ./sudo cp -R ./include/spdlog /usr/local/include/")

            print("[Installing]: headers and libraries!")
            os.execute("sudo rm -rf /usr/local/include/optkit/ && sudo mkdir -p /usr/local/include/optkit")               -- create optkit directory for headers
            os.execute(
                "cd ./src; sudo find ./ -type f -name \"*.hh\" -exec cp --parents {} \"/usr/local/include/optkit/\" \\;") -- copy all header files by keeping the file structure as-is
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_STATIC .. ".a /usr/local/lib")                        -- copy static library
            os.execute("sudo cp -R ./bin/Release/lib" .. OPTKIT_LIB_DYNAMIC .. ".so /usr/local/lib")                      -- copy dynamic library
            print("[✅ Installed]: headers and libraries!")

            os.execute("sudo ldconfig") -- refresh dynamic link cache.

            -- BUILD TOOLS AND ALSO INSTALL THEM, TOOLS WILL BE USING THE STATIC-DYNAMIC LIBRARY THAT'S INSTALLED.
            print("[Installing]: utility tools!")
            os.execute("cd ./tools && ./install.sh")
            print("[✅ Installed]: utility tools!")
        end
    }

    newaction {
        trigger = custom_actions.remove,
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
        trigger = custom_actions.carm,
        description = "Run native CARM roofline benchmarking and refresh environment config",
        execute = function()
            if not os.isdir(CARM_UTILS_DIR) then
                print("❌ Native CARM utilities directory not found: " .. CARM_UTILS_DIR)
                os.exit(1)
            end

            local carm_num_runs = os.getenv("OPTKIT_CARM_NUM_RUNS")
            if not carm_num_runs or carm_num_runs == "" then
                carm_num_runs = "256"
            end

            local carm_extra_args = os.getenv("OPTKIT_CARM_ARGS") or ""

            print("[CARM] Running roofline benchmarks from " .. CARM_UTILS_DIR)
            print("[CARM] Using num_runs=" .. carm_num_runs)
            if carm_extra_args ~= "" then
                print("[CARM] Extra args: " .. carm_extra_args)
            end

            local benchmark_cmd =
                "cd " .. CARM_UTILS_DIR ..
                " && mkdir -p ./carm_results/roofline" ..
                " && python3 ./run.py -nr " .. carm_num_runs
            if carm_extra_args ~= "" then
                benchmark_cmd = benchmark_cmd .. " " .. carm_extra_args
            end

            local benchmark_ok = os.execute(benchmark_cmd)
            if benchmark_ok ~= true and benchmark_ok ~= 0 then
                print("❌ CARM roofline benchmarking failed.")
                os.exit(1)
            end

            if not os.isfile(CARM_RESULTS_CSV) then
                print("❌ Expected CARM results were not generated: " .. CARM_RESULTS_CSV)
                os.exit(1)
            end

            print("[CARM] Generated roofline CSV: " .. CARM_RESULTS_CSV)
            print("[CARM] Refreshing environment configuration")
            local config_ok = os.execute("./generate_environment_config.sh")
            if config_ok ~= true and config_ok ~= 0 then
                print("❌ Failed to refresh environment configuration after CARM run.")
                os.exit(1)
            end
        end
    }

    newaction {
        trigger = custom_actions.generate_doc,
        description = "Generate documentation",
        execute = function()
            os.execute("doxygen ./doxyfile") -- create doxygen file
            print("📄 Documentation generated!")
        end
    }

    newaction {
        trigger = custom_actions.remove_doc,
        description = "Remove documentation",
        execute = function()
            os.execute("rm -rf ./doc") -- create doxygen file
            print("📄 Documentation Removed!")
        end
    }
end
