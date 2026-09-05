#include "resource_monitor.hpp"
#include "core/system_operations.hpp"
#include "ui/terminal.hpp"
#include "ui/menu.hpp"
#include "utils/exec.hpp"
#include <iostream>

namespace smart_ssh::modules {

static std::string build_ssh_target(const core::active_session& session) {
    if (!session.user.empty() && !session.host_address.empty()) {
        return session.user + "@" + session.host_address;
    }
    return session.host_alias;
}

[[nodiscard]] std::string check_cpu_memory_usage(const core::active_session& session) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"ps aux --sort=-%cpu | head -n 12; echo ''; free -h\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

void run_resource_monitor(const core::active_session& session) {
    ui::menu r_menu("Resource & Quota Monitor (CPU, RAM, Disk)");

    r_menu.add_item("View Real-Time CPU & RAM Utilization (Top Processes)", [&]() {
        ui::print_info("Querying CPU/RAM on " + session.host_alias + "...");
        std::string output = check_cpu_memory_usage(session);
        std::cout << "\n" << output << "\n";
    });

    r_menu.add_item("View Disk Space Usage (df -h)", [&]() {
        ui::print_info("Querying disk usage on " + session.host_alias + "...");
        std::string output = core::check_remote_disk_space(session);
        std::cout << "\n" << output << "\n";
    });

    r_menu.add_item("View User Storage Quota", [&]() {
        ui::print_info("Querying quota for user '" + session.user + "'...");
        std::string output = core::check_remote_quota(session);
        std::cout << "\n" << output << "\n";
    });

    r_menu.run();
}

} // namespace smart_ssh::modules
