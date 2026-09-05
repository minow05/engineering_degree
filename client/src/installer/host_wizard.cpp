#include "host_wizard.hpp"
#include "ui/terminal.hpp"
#include <iostream>

namespace smart_ssh::installer {

[[nodiscard]] wizard_config run_host_wizard() {
    wizard_config cfg;

    ui::print_header("SmartSSH Interactive Host Installer Wizard");

    cfg.host_alias = ui::prompt("Enter Host Alias");
    cfg.host_address = ui::prompt("Enter Host IP / Domain");
    cfg.primary_user = ui::prompt("Enter Primary Username");

    std::string extra_user = ui::prompt("Enter Additional User to Create");
    if (!extra_user.empty()) {
        cfg.additional_users.push_back(extra_user);
    }

    cfg.enable_shared_folder = ui::confirm("Enable Global Shared Folder (/home/shared) for all users?", true);

    ui::print_header("Select Modular Services to Enable");
    cfg.features.ssh_keys = ui::confirm("Enable Passwordless SSH Key Pair Setup & Deployment?", true);
    cfg.features.samba_storage = ui::confirm("Enable Network Storage (Samba & File Manager)?", true);
    cfg.features.process_hosting = ui::confirm("Enable Process & Docker Container Management?", true);

    ui::print_success("Wizard configuration completed!");
    return cfg;
}

} // namespace smart_ssh::installer
