#include "process_manager.hpp"
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

[[nodiscard]] std::string list_docker_containers(const core::active_session& session) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"docker ps -a --format 'table {{.Names}}\\t{{.Status}}\\t{{.Ports}}' 2>/dev/null || systemctl status --type=service --state=running --no-pager | head -n 20\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

[[nodiscard]] std::string get_docker_logs(const core::active_session& session, const std::string& container_name) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"docker logs --tail 50 " + container_name + " 2>&1\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

[[nodiscard]] bool control_docker_container(const core::active_session& session, const std::string& container_name, const std::string& action) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"docker " + action + " " + container_name + "\"";

    auto res = utils::run_command(cmd);
    return (res.exit_code == 0);
}

void run_process_manager(const core::active_session& session) {
    ui::menu p_menu("Process & Container Hosting Manager (Docker / Services)");

    p_menu.add_item("List Running Docker Containers & Active Services", [&]() {
        ui::print_info("Querying containers and systemd services on " + session.host_alias + "...");
        std::string output = list_docker_containers(session);
        std::cout << "\n" << output << "\n";
    });

    p_menu.add_item("Inspect Container Logs", [&]() {
        std::string container = ui::prompt("Enter Container Name / ID");
        if (container.empty()) return;
        ui::print_info("Fetching last 50 log lines for '" + container + "'...");
        std::string logs = get_docker_logs(session, container);
        std::cout << "\n" << logs << "\n";
    });

    p_menu.add_item("Start / Stop / Restart Container", [&]() {
        std::string container = ui::prompt("Enter Container Name / ID");
        if (container.empty()) return;
        std::string action = ui::prompt("Enter Action (start / stop / restart)");
        if (action.empty()) return;
        ui::print_info("Executing 'docker " + action + " " + container + "'...");
        if (control_docker_container(session, container, action)) {
            ui::print_success("Action executed successfully on container '" + container + "'!");
        } else {
            ui::print_error("Failed to execute action on container.");
        }
    });

    p_menu.run();
}

} // namespace smart_ssh::modules
