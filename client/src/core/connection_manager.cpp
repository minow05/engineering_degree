#include "connection_manager.hpp"
#include "utils/exec.hpp"

namespace smart_ssh::core {

[[nodiscard]] bool establish_session(active_session& session, const host_entry& host) {
    session.host_alias = host.name;
    session.host_address = host.host_name.empty() ? host.name : host.host_name;
    session.user = host.user.empty() ? "pi" : host.user;
    session.port = host.port == 0 ? 22 : host.port;
    session.identity_file = host.identity_file;

    session.is_connected = test_connection(session.host_address, session.port);
    return session.is_connected;
}

[[nodiscard]] int start_interactive_shell(const active_session& session) {
    std::string cmd = "ssh";
    if (session.port != 22) {
        cmd += " -p " + std::to_string(session.port);
    }
    if (!session.identity_file.empty()) {
        cmd += " -i \"" + session.identity_file + "\"";
    }

    if (!session.user.empty() && !session.host_address.empty()) {
        cmd += " " + session.user + "@" + session.host_address;
    } else {
        cmd += " " + session.host_alias;
    }

    return utils::run_interactive(cmd);
}

[[nodiscard]] bool test_connection(const std::string& host_address, int port) {
#ifdef _WIN32
    std::string cmd = "powershell -Command \"Test-NetConnection -ComputerName '" + host_address + "' -Port " + std::to_string(port) + " | Select-Object -ExpandProperty TcpTestSucceeded\"";
    auto res = utils::run_command(cmd);
    return (res.stdout_str.find("True") != std::string::npos);
#else
    std::string cmd = "nc -z -w 3 " + host_address + " " + std::to_string(port);
    auto res = utils::run_command(cmd);
    return (res.exit_code == 0);
#endif
}

} // namespace smart_ssh::core
