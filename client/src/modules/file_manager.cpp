#include "file_manager.hpp"
#include "core/system_operations.hpp"
#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include <iostream>

namespace smart_ssh::modules {

void run_file_manager(const core::active_session& session) {
    ui::menu f_menu("Network Storage Manager (Samba Shares)");

    f_menu.add_item("Open Samba Home Drive (Explorer)", [&]() {
        ui::print_info("Opening Samba share \\\\" + session.host_address + "\\" + session.user + "...");
        if (core::open_samba_share(session, session.user)) {
            ui::print_success("User Samba drive opened successfully!");
        } else {
            ui::print_error("Failed to open user Samba drive.");
        }
        ui::wait_for_key();
    });

    f_menu.add_item("Open Controlled Shared Samba Drive (Explorer)", [&]() {
        ui::print_info("Opening Samba share \\\\" + session.host_address + "\\shared...");
        if (core::open_samba_share(session, "shared")) {
            ui::print_success("Shared Samba drive opened successfully!");
        } else {
            ui::print_error("Failed to open shared Samba drive.");
        }
        ui::wait_for_key();
    });

    f_menu.run();
}

} // namespace smart_ssh::modules
