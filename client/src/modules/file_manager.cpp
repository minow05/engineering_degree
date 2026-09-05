#include "file_manager.hpp"
#include "core/system_operations.hpp"
#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "utils/exec.hpp"
#include <iostream>

namespace smart_ssh::modules {

[[nodiscard]] std::string list_remote_directory(const core::active_session& session, const std::string& remote_path) {
    std::string target = session.user.empty() ? session.host_alias : session.user + "@" + session.host_address;
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"ls -la " + remote_path + "\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

void run_file_manager(const core::active_session& session) {
    ui::menu f_menu("In-App Remote File & Network Storage Manager");

    f_menu.add_item("Browse User Home Folder (/home/" + session.user + ")", [&]() {
        std::string output = list_remote_directory(session, "/home/" + session.user);
        std::cout << "\nContents of /home/" << session.user << ":\n" << output << "\n";
    });

    f_menu.add_item("Browse Global Shared Folder (/home/shared)", [&]() {
        std::string output = list_remote_directory(session, "/home/shared");
        std::cout << "\nContents of /home/shared:\n" << output << "\n";
    });

    f_menu.add_item("Browse Custom Directory Path", [&]() {
        std::string path = ui::prompt("Enter Remote Path");
        if (path.empty()) path = "/home/" + session.user;
        std::string output = list_remote_directory(session, path);
        std::cout << "\nContents of " << path << ":\n" << output << "\n";
    });

    f_menu.add_item("Open Samba Home Drive (Explorer/Finder)", [&]() {
        ui::print_info("Opening Samba share \\\\" + session.host_address + "\\" + session.user + "...");
        if (core::open_samba_share(session, session.user)) {
            ui::print_success("User Samba drive opened successfully!");
        } else {
            ui::print_error("Failed to open user Samba drive.");
        }
    });

    f_menu.add_item("Open Global Shared Samba Drive (Explorer/Finder)", [&]() {
        ui::print_info("Opening Samba share \\\\" + session.host_address + "\\shared...");
        if (core::open_samba_share(session, "shared")) {
            ui::print_success("Global shared Samba drive opened successfully!");
        } else {
            ui::print_error("Failed to open global shared Samba drive.");
        }
    });

    f_menu.run();
}

} // namespace smart_ssh::modules
