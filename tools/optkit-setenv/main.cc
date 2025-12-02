#include <iostream>
#include "sysconfig.hh"
#include "config_loader.hh"
#include "config_writer.hh"

int help(int argc, char **argv)
{
    std::cerr << "Usage: " << argv[0] << " [OPTIONS] <config.json>\n"
              << "\nOptions:\n"
              << "  --help              Display this help message\n"
              << "  --init              Create empty config template (default_config.json)\n"
              << "  --restore           Restore environment from backup (/tmp/optkit_env_backup.json)\n"
              << "  --backup            Backup current environment to /tmp/optkit_env_backup.json\n"
              << "  <config.json>       Load and display configuration from file\n"
              << "\nExamples:\n"
              << "  " << argv[0] << " --init\n"
              << "  " << argv[0] << " --restore\n"
              << "  " << argv[0] << " my_config.json\n"
              << std::endl;
    return 1;
}
int main(int argc, char **argv)
{
    OPTKIT_INIT(false);
    if (argc == 2 && std::string(argv[1]) == "--backup")
    {
        SysConfig config = create_empty_config();
        config.load_current_settings(getpid());
        save_system_config(config, "/tmp/optkit_env_backup.json");
        std::cout << "Created backup of current environment at /tmp/optkit_env_backup.json\n";
    }
    else if (argc == 2 && std::string(argv[1]) == "--init")
    {
        SysConfig config = create_empty_config();
        save_system_config(config, "default_config.json");
        std::cout << "Created default_config.json template" << std::endl;
    }
    else if (argc == 2 && std::string(argv[1]) == "--restore")
    {
        SysConfig backup_config = load_system_config("/tmp/optkit_env_backup.json");
        std::cout << "Restored environment from /tmp/optkit_env_backup.json\n";
    }
    else if (argc == 2 && std::string(argv[1]).find(".json") != std::string::npos)
    {
        EXEC_IF_ROOT_RETURN(false);
        SysConfig config = load_system_config(argv[1]);
        std::cout << config << "\n";
        if (config.is_valid())
        {
            // config.apply();
            std::cout << "Configuration is valid and has been applied successfully.\n";
        }
    }
    else
        return help(argc, argv);
    return 0;
}