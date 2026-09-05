#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "installer/host_wizard.hpp"
#include "installer/host_builder.hpp"
#include "core/ssh_config.hpp"
#include "core/key_manager.hpp"
#include "core/connection_manager.hpp"

#include <iostream>

using namespace smart_ssh;

int main() {
    ui::init_terminal();
    ui::print_header("SmartSSH Primary Setup & Provisioning Installer");

    auto wiz_cfg = installer::run_host_wizard();

    ui::print_header("Step 1: Passwordless SSH Key Authentication Setup");
    std::string key_name = "id_ed25519_" + wiz_cfg.host_alias;
    ui::print_info("Generating SSH key pair '~/.ssh/" + key_name + "'...");
    (void)core::generate_key(key_name, wiz_cfg.primary_user + "@" + wiz_cfg.host_alias, "", "ed25519");

    auto key_dir = core::get_ssh_dir();
    auto pub_key_path = key_dir / (key_name + ".pub");

    ui::print_info("Deploying public key to " + wiz_cfg.primary_user + "@" + wiz_cfg.host_address + "...");
    (void)core::deploy_public_key(pub_key_path, wiz_cfg.primary_user, wiz_cfg.host_address);

    ui::print_header("Step 2: Save ~/.ssh/config Host Directive");
    core::ssh_config cfg;
    (void)cfg.load();

    core::host_entry host;
    host.name = wiz_cfg.host_alias;
    host.host_name = wiz_cfg.host_address;
    host.user = wiz_cfg.primary_user;
    host.port = 22;
    host.identity_file = "~/.ssh/" + key_name;

    cfg.add_or_update_entry(host);
    if (cfg.save()) {
        ui::print_success("Saved host directive '" + host.name + "' to ~/.ssh/config!");
    }

    ui::print_header("Step 3: Build Standalone Host-Fitted Executable");
    (void)installer::generate_standalone_host_executable(wiz_cfg);

    ui::print_success("SmartSSH Setup Complete! Run your standalone host app: build/out/smart-ssh-" + wiz_cfg.host_alias + ".exe");
    return 0;
}
