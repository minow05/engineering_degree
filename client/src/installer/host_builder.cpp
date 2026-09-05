#include "host_builder.hpp"
#include "ui/terminal.hpp"
#include "utils/exec.hpp"
#include <filesystem>
#include <iostream>

namespace smart_ssh::installer {

[[nodiscard]] bool generate_standalone_host_executable(const wizard_config& config) {
    ui::print_info("Building Standalone Host Executable for '" + config.host_alias + "'...");

    std::string binary_name = "smart-ssh-" + config.host_alias;
#ifdef _WIN32
    binary_name += ".exe";
#endif

    std::string build_cmd = "cmake --build build --config Debug";
    int res = utils::run_interactive(build_cmd);
    if (res == 0) {
        ui::print_success("Standalone Host Executable generated successfully at build/out/" + binary_name);
        return true;
    }

    ui::print_error("Failed to compile standalone host executable.");
    return false;
}

} // namespace smart_ssh::installer
