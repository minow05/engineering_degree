#include "system_operations.hpp"
#include "utils/exec.hpp"
#include <cstdlib>
#include <algorithm>

namespace smart_ssh::core {

static std::string build_ssh_target(const active_session& session) {
    if (!session.user.empty() && !session.host_address.empty()) {
        return session.user + "@" + session.host_address;
    }
    return session.host_alias;
}

[[nodiscard]] std::string check_remote_disk_space(const active_session& session) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"df -h /\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

[[nodiscard]] std::string check_remote_quota(const active_session& session) {
    std::string target = build_ssh_target(session);
    std::string user = session.user.empty() ? "pi" : session.user;
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"quota -u " + user + " 2>/dev/null || du -sh /home/" + user + "\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

[[nodiscard]] bool open_samba_share(const active_session& session, const std::string& share_name) {
    std::string host = session.host_address.empty() ? session.host_alias : session.host_address;

    // Windows Samba UNC path: Use IP instead of DNS (e.g. 172.30.0.1 instead of minow.rpi)
    if (host == "minow.rpi" || host == "minowRPI" || host == "rpi") {
        host = "172.30.0.1";
    }

#ifdef _WIN32
    std::string unc_path = "\\\\" + host + "\\" + share_name;
    std::string cmd = "explorer.exe \"" + unc_path + "\"";
    return (utils::run_interactive(cmd) == 0);
#elif __APPLE__
    std::string smb_url = "smb://" + host + "/" + share_name;
    std::string cmd = "open \"" + smb_url + "\"";
    return (utils::run_interactive(cmd) == 0);
#else
    std::string smb_url = "smb://" + host + "/" + share_name;
    std::string cmd = "xdg-open \"" + smb_url + "\"";
    return (utils::run_interactive(cmd) == 0);
#endif
}

} // namespace smart_ssh::core
