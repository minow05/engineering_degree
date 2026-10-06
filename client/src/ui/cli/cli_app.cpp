#include "cli_app.hpp"
#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "modules/file_manager.hpp"
#include "modules/process_manager.hpp"
#include "modules/resource_monitor.hpp"
#include <iostream>

namespace smart_ssh::ui::cli {

cli_app::cli_app(core::application& app)
    : m_app(app) {}

int cli_app::run() {
    init_terminal();

    if (!m_app.is_configured()) {
        print_info("No local single-host configuration found. Launching Setup Wizard...");
        if (!m_app.run_first_time_wizard()) {
            print_error("Setup Wizard failed or was cancelled. Exiting.");
            return 1;
        }
    }

    render_main_menu();
    return 0;
}

void cli_app::render_main_menu() {
    bool running = true;
    while (running) {
        const auto& session = m_app.get_session();
        print_header("SmartSSH Client - Bound Host: " + session.host_alias);

        if (session.is_connected) {
            print_success("Connected to Host: " + session.user + "@" + session.host_address + ":" + std::to_string(session.port) + " [ONLINE]");
        } else {
            print_warning("Target Host Status: " + session.host_address + " [OFFLINE / CHECK KEY & NETWORK]");
        }

        menu main_menu("Instant Host Operations Menu (" + session.host_alias + ")");

        main_menu.add_item("Interactive SSH Terminal Shell", [&]() {
            print_info("Launching SSH TTY shell to " + session.user + "@" + session.host_address + "...");
            (void)core::start_interactive_shell(session);
        });

        main_menu.add_item("In-App Remote File & Network Storage Manager", [&]() {
            modules::run_file_manager(session);
        });

        main_menu.add_item("Resource & Storage Quota Monitor (CPU, RAM, Disk)", [&]() {
            modules::run_resource_monitor(session);
        });

        main_menu.add_item("Settings & Reconfiguration (Reset Host Binding)", [&]() {
            render_settings_menu();
            if (!m_app.is_configured()) {
                running = false;
            }
        });

        main_menu.run();
        running = false;
    }
    print_info("Exiting SmartSSH Client. Goodbye!");
}

void cli_app::render_settings_menu() {
    menu settings("SmartSSH Settings & Reconfiguration");

    settings.add_item("View Local Host Configuration Details", [&]() {
        const auto& data = m_app.get_config().get_data();
        std::cout << "\nHost Alias:     " << data.host_alias << "\n"
                  << "Host Address:   " << data.host_address << "\n"
                  << "Username:       " << data.username << "\n"
                  << "Port:           " << data.port << "\n"
                  << "Identity File:  " << data.identity_file << "\n\n";
        wait_for_key();
    });

    settings.add_item("Reset Configuration & Re-run Setup Wizard", [&]() {
        if (confirm("Are you sure you want to reset local single-host configuration?", false)) {
            (void)m_app.reset_configuration();
            print_info("Re-launching Setup Wizard...");
            (void)m_app.run_first_time_wizard();
        }
        wait_for_key();
    });

    settings.run();
}

} // namespace smart_ssh::ui::cli
