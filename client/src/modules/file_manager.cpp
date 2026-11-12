#include "file_manager.hpp"
#include "core/system_operations.hpp"
#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include <iostream>

namespace smart_ssh::modules {

static void open_or_mount_share(const core::active_session& session,
                                const std::string& share_name,
                                const std::string& label) {
    std::string mounted = core::get_mounted_samba_path(session, share_name);
    if (mounted.empty()) {
        ui::print_info(label + " is not mounted yet. Attempting automatic mount...");
        mounted = core::mount_samba_share(session, share_name);
        if (mounted.empty()) {
            std::string pass = ui::prompt_password("Enter Samba password for '" + session.user + "' (or press Enter to open UNC directly)");
            if (!pass.empty()) {
                mounted = core::mount_samba_share(session, share_name, pass);
            }
        }
    }

    if (!mounted.empty()) {
        ui::print_info("Opening mounted " + label + " at " + mounted + "...");
    } else {
        std::string host = core::resolve_samba_host(session);
        ui::print_info("Opening Samba share \\\\" + host + "\\" + share_name + "...");
    }

    if (core::open_samba_share(session, share_name)) {
        ui::print_success(label + " opened successfully!");
    } else {
        ui::print_error("Failed to open " + label + ".");
    }
    ui::wait_for_key();
}

void run_file_manager(const core::active_session& session) {
    ui::menu f_menu("Network Storage Manager (Samba Shares)");

    f_menu.add_item("Open Samba Home Drive (Explorer)", [&]() {
        open_or_mount_share(session, session.user, "User Samba drive");
    });

    f_menu.add_item("Open Controlled Shared Samba Drive (Explorer)", [&]() {
        open_or_mount_share(session, "shared", "Shared Samba drive");
    });

    f_menu.add_item("Mount / Remount Samba Network Drives", [&]() {
        std::string home_mnt = core::get_mounted_samba_path(session, session.user);
        std::string shared_mnt = core::get_mounted_samba_path(session, "shared");
        ui::print_info("Current mounts -> Home: " + (home_mnt.empty() ? "Not mounted" : home_mnt) +
                       " | Shared: " + (shared_mnt.empty() ? "Not mounted" : shared_mnt));

        std::string pass = ui::prompt_password("Enter Samba password for '" + session.user + "' (leave empty for cached OS credentials)");
        std::string res_home = core::mount_samba_share(session, session.user, pass);
        std::string res_shared = core::mount_samba_share(session, "shared", pass);

        if (!res_home.empty()) {
            ui::print_success("Home drive mounted at: " + res_home);
        } else {
            ui::print_error("Failed to mount Home drive.");
        }
        if (!res_shared.empty()) {
            ui::print_success("Shared drive mounted at: " + res_shared);
        } else {
            ui::print_error("Failed to mount Shared drive.");
        }
        ui::wait_for_key();
    });

    f_menu.add_item("Unmount Samba Network Drives", [&]() {
        bool u_home = core::unmount_samba_share(session, session.user);
        bool u_shared = core::unmount_samba_share(session, "shared");
        if (u_home || u_shared) {
            ui::print_success("Mounted Samba drive(s) unmounted cleanly.");
        } else {
            ui::print_info("No mounted Samba drives found to unmount.");
        }
        ui::wait_for_key();
    });

    f_menu.run();
}

} // namespace smart_ssh::modules
