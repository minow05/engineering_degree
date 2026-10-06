#include "setup_wizard.hpp"
#include "ui/terminal.hpp"
#include "core/key_manager.hpp"
#include "core/ssh_config.hpp"
#include "core/connection_manager.hpp"
#include <iostream>

namespace smart_ssh::installer {

setup_wizard_result setup_wizard::run_interactive_wizard() {
    setup_wizard_result res;
    ui::print_header("SmartSSH First-Run Setup & Configuration Wizard");

    std::string alias = ui::prompt("Enter Host Alias (e.g. minowRPI)");
    if (alias.empty()) alias = "minowRPI";

    std::string address = ui::prompt("Enter Raspberry Pi Address / IP");
    if (address.empty()) address = "172.30.0.1";

    std::string username = ui::prompt("Enter Username");
    if (username.empty()) username = "minow";

    res.config_data.host_alias = alias;
    res.config_data.host_address = address;
    res.config_data.username = username;
    res.config_data.port = 22;

    ui::print_header("Step 1: SSH Key Authentication Setup");
    std::string key_name = "id_ed25519_" + alias;
    auto ssh_dir = core::get_ssh_dir();
    auto priv_key_path = ssh_dir / key_name;
    auto pub_key_path = ssh_dir / (key_name + ".pub");

    ui::print_info("Generating SSH key pair '" + priv_key_path.string() + "'...");
    (void)core::generate_key(key_name, username + "@" + alias, "", "ed25519");
    res.config_data.identity_file = priv_key_path.string();

    ui::print_info("Deploying public key to " + username + "@" + address + "...");
    (void)core::deploy_public_key(pub_key_path, username, address);

    ui::print_header("Step 2: Connection Verification");
    ui::print_info("Testing connection to " + address + ":22...");
    if (core::test_connection(address, 22)) {
        ui::print_success("Connection to Raspberry Pi verified successfully!");
    } else {
        ui::print_warning("Connection check timed out or host unreachable. Setup will continue.");
    }

    res.config_data.is_configured = true;
    res.success = true;
    return res;
}

bool setup_wizard::configure_and_bind_host(const core::app_config_data& input_data, core::configuration& app_config) {
    app_config.set_data(input_data);
    if (!app_config.save()) {
        return false;
    }

    // Also update OpenSSH ~/.ssh/config for CLI compatibility
    core::ssh_config ssh_cfg;
    (void)ssh_cfg.load();

    core::host_entry entry;
    entry.name = input_data.host_alias;
    entry.host_name = input_data.host_address;
    entry.user = input_data.username;
    entry.port = input_data.port;
    entry.identity_file = input_data.identity_file;

    ssh_cfg.add_or_update_entry(entry);
    (void)ssh_cfg.save();

    return true;
}

} // namespace smart_ssh::installer
