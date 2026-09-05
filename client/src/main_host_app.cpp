#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "modules/file_manager.hpp"
#include "modules/process_manager.hpp"
#include "modules/resource_monitor.hpp"
#include "core/ssh_config.hpp"
#include "core/connection_manager.hpp"

#include <iostream>

using namespace smart_ssh;

int main() {
    ui::init_terminal();
    core::ssh_config cfg;
    (void)cfg.load();

    core::active_session session;

    // Fast passwordless startup connection to host minowRPI
    const auto* host = cfg.find_entry("minowRPI");
    if (host) {
        (void)core::establish_session(session, *host);
    } else if (!cfg.get_entries().empty()) {
        (void)core::establish_session(session, cfg.get_entries()[0]);
    } else {
        core::host_entry default_host;
        default_host.name = "minowRPI";
        default_host.host_name = "172.30.0.1";
        default_host.user = "minow";
        default_host.port = 22;
        default_host.identity_file = "~/.ssh/ssh_key_rpi";
        (void)core::establish_session(session, default_host);
    }

    ui::print_header("SmartSSH Host-Bound Manager (" + session.host_alias + ")");
    if (session.is_connected) {
        ui::print_success("Passwordless Connection Active: " + session.user + "@" + session.host_address + " [ONLINE]");
    } else {
        ui::print_warning("Target Host Status: " + session.host_address + " [OFFLINE / CHECK ZERO-TIER]");
    }

    ui::menu host_menu("Instant Host Operations (" + session.host_alias + ")");

    host_menu.add_item("Instant Interactive SSH Terminal Shell", [&]() {
        ui::print_info("Launching SSH TTY shell to " + session.user + "@" + session.host_address + "...");
        (void)core::start_interactive_shell(session);
    });

    host_menu.add_item("In-App Remote File & Network Storage Manager", [&]() {
        modules::run_file_manager(session);
    });

    host_menu.add_item("Process & Container Hosting Manager (Docker / Services)", [&]() {
        modules::run_process_manager(session);
    });

    host_menu.add_item("Resource & Storage Quota Monitor (CPU, RAM, Disk)", [&]() {
        modules::run_resource_monitor(session);
    });

    host_menu.run();

    ui::print_info("Exiting SmartSSH. Goodbye!");
    return 0;
}
