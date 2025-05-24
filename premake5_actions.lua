---@diagnostic disable: undefined-global, lowercase-global
---@diagnostic disable: undefined-field

function define_custom_actions()

    local custom_actions = {
        clean = "clean",          -- clean the optkit build.
        install = "install",      -- build & install libs
        remove = "remove",        -- remove installed libs from the system
        generate_doc = "generate_doc",      -- generate documentaiton
        remove_doc = "remove_doc", -- delete documentaiton
        gmake="gmake",
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

            print("[REMOVE]: ./test/utils/environment_config.hh")
            os.remove("./test/utils/environment_config.hh")

            print("[REMOVE]: " .. CORE_EVENTS_DIR)
            os.rmdir(CORE_EVENTS_DIR)

            print("[REMOVE]: " .. OPTKIT_APP .. ".make")
            os.remove(OPTKIT_APP .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_DYNAMIC .. ".make")
            os.remove(OPTKIT_LIB_DYNAMIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_LIB_STATIC .. ".make")
            os.remove(OPTKIT_LIB_STATIC .. ".make")

            print("[REMOVE]: " .. OPTKIT_TEST .. ".make")
            os.remove(OPTKIT_TEST .. ".make")

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
            
            -- Check if spdlog installed globally, if not install it.
            if dynamic_lib_exists("spdlog") or static_lib_exists("spdlog") then
                print("✅ spdlog is already installed globally.")
            else
                print("[Installing]: SPDLOG headers and static library")
                os.execute(
                    "cd " ..
                    LIB_SPD_PATH ..
                    "/ && ./compile.sh && sudo cp -R ./include/spdlog /usr/local/include/ && sudo cp ./build/libspdlog.a /usr/local/lib")
            end
 
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