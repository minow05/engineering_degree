#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "installer/host_wizard.hpp"
#include "modules/file_manager.hpp"
#include "modules/process_manager.hpp"
#include "modules/resource_monitor.hpp"
#include "core/ssh_config.hpp"
#include "core/key_manager.hpp"
#include "core/connection_manager.hpp"

#include <iostream>

using namespace smart_ssh;

void initialize_host_session(core::ssh_config& cfg, core::active_session& session) {
    (void)cfg.load();
    const auto& entries = cfg.get_entries();

    if (!entries.empty()) {
        const auto& host = entries[0];
        (void)core::establish_session(session, host);
    } else {
        core::host_entry default_host;
        default_host.name = "minowRPI";
        default_host.host_name = "172.30.0.1";
        default_host.user = "minow";
        default_host.port = 22;
        default_host.identity_file = "~/.ssh/ssh_key_rpi";
        (void)core::establish_session(session, default_host);
    }
}

int main() {
    ui::init_terminal();
    core::ssh_config cfg;
    (void)cfg.load();

    core::active_session session;
    initialize_host_session(cfg, session);

    ui::print_header("SmartSSH Host-Bound Manager (" + session.host_alias + ")");
    if (session.is_connected) {
        ui::print_success("Connected to Host: " + session.user + "@" + session.host_address + ":" + std::to_string(session.port) + " [ONLINE]");
    } else {
        ui::print_warning("Target Host Status: " + session.host_address + " [OFFLINE / KEY CHECK NEEDED]");
    }

    ui::menu main_menu("Instant Host Operations Menu (" + session.host_alias + ")");

    main_menu.add_item("Instant Interactive SSH Terminal Shell", [&]() {
        ui::print_info("Launching SSH TTY shell to " + session.user + "@" + session.host_address + "...");
        (void)core::start_interactive_shell(session);
    });

    main_menu.add_item("In-App Remote File & Network Storage Manager", [&]() {
        modules::run_file_manager(session);
    });

    main_menu.add_item("Process & Container Hosting Manager (Docker / Services)", [&]() {
        modules::run_process_manager(session);
    });

    main_menu.add_item("Resource & Storage Quota Monitor (CPU, RAM, Disk)", [&]() {
        modules::run_resource_monitor(session);
    });

    main_menu.add_item("Run Interactive Host Setup & User Creation Wizard", [&]() {
        auto wiz_cfg = installer::run_host_wizard();
        ui::print_info("Re-binding host configuration...");
        core::host_entry new_host;
        new_host.name = wiz_cfg.host_alias;
        new_host.host_name = wiz_cfg.host_address;
        new_host.user = wiz_cfg.primary_user;
        cfg.add_or_update_entry(new_host);
        (void)cfg.save();
        (void)core::establish_session(session, new_host);
    });

    main_menu.run();

    ui::print_info("Exiting SmartSSH CLI. Goodbye!");
    return 0;
}
