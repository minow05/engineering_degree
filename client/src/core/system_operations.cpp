#include "system_operations.hpp"
#include "utils/exec.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winnetwk.h>
#include <shellapi.h>
#else
#include <unistd.h>
#endif

namespace smart_ssh::core {

static std::string build_ssh_target(const active_session& session) {
    if (!session.user.empty() && !session.host_address.empty()) {
        return session.user + "@" + session.host_address;
    }
    return session.host_alias;
}

static bool equals_ignore_case(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

std::string check_remote_disk_space(const active_session& session) {
    std::string target = build_ssh_target(session);
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"df -h /\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

std::string check_remote_quota(const active_session& session) {
    std::string target = build_ssh_target(session);
    std::string user = session.user.empty() ? "pi" : session.user;
    std::string cmd = "ssh";
    if (session.port != 22) cmd += " -p " + std::to_string(session.port);
    if (!session.identity_file.empty()) cmd += " -i \"" + session.identity_file + "\"";
    cmd += " " + target + " \"sh -c 'quota -u " + user + " 2>/dev/null || du -sh /home/" + user + " 2>/dev/null || echo Brak danych'\"";

    auto res = utils::run_command(cmd);
    return res.stdout_str;
}

std::string resolve_samba_host(const active_session& session) {
    std::string host = session.host_address.empty() ? session.host_alias : session.host_address;

    // Windows Samba UNC path resolution: Maps hostname to raw IP for NetBIOS/Explorer compatibility
    if (host == "minow.rpi" || host == "minowRPI" || host == "rpi") {
        host = "172.30.0.1";
    }
    return host;
}

#ifdef _WIN32

static std::string build_unc_path(const std::string& host, const std::string& share_name) {
    return "\\\\" + host + "\\" + share_name;
}

static bool is_drive_letter_free(char letter) {
    char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
    if (upper < 'D' || upper > 'Z') return false;

    DWORD mask = GetLogicalDrives();
    if ((mask & (1u << (upper - 'A'))) != 0) {
        return false;
    }

    // Also ensure there is no remembered/disconnected network mapping on this letter
    std::string local = std::string(1, upper) + ":";
    char buf[MAX_PATH] = {0};
    DWORD len = MAX_PATH;
    DWORD res = WNetGetConnectionA(local.c_str(), buf, &len);
    return (res == ERROR_NOT_CONNECTED);
}

static char pick_free_drive_letter(const std::string& share_name, const std::string& preferred_drive) {
    if (!preferred_drive.empty()) {
        char p = static_cast<char>(std::toupper(static_cast<unsigned char>(preferred_drive[0])));
        if (is_drive_letter_free(p)) return p;
    }

    if (equals_ignore_case(share_name, "shared") && is_drive_letter_free('S')) {
        return 'S';
    }
    if (!equals_ignore_case(share_name, "shared") && is_drive_letter_free('M')) {
        return 'M';
    }

    for (char c = 'Z'; c >= 'E'; --c) {
        if (is_drive_letter_free(c)) return c;
    }
    return 0;
}

static std::string find_drive_for_unc(const std::string& unc_path, DWORD* out_status = nullptr) {
    for (char c = 'Z'; c >= 'D'; --c) {
        std::string local = std::string(1, c) + ":";
        char buf[MAX_PATH] = {0};
        DWORD len = MAX_PATH;
        DWORD res = WNetGetConnectionA(local.c_str(), buf, &len);
        if (res == NO_ERROR || res == ERROR_CONNECTION_UNAVAIL) {
            std::string remote(buf);
            while (!remote.empty() && remote.back() == '\\') {
                remote.pop_back();
            }
            if (equals_ignore_case(remote, unc_path)) {
                if (out_status) *out_status = res;
                return local + "\\";
            }
        }
    }
    return "";
}

#endif // _WIN32

std::string get_mounted_samba_path(const active_session& session, const std::string& share_name) {
    std::string host = resolve_samba_host(session);
    if (host.empty() || share_name.empty()) return "";

#ifdef _WIN32
    return find_drive_for_unc(build_unc_path(host, share_name));
#elif __APPLE__
    std::string vol_path = "/Volumes/" + share_name;
    auto res = utils::run_command("mount | grep -i \"" + vol_path + "\"");
    if (res.exit_code == 0 && !res.stdout_str.empty()) {
        return vol_path;
    }
    return "";
#else
    std::string gvfs_path = "/run/user/" + std::to_string(getuid()) +
                            "/gvfs/smb-share:server=" + host + ",share=" + share_name;
    auto res = utils::run_command("test -d \"" + gvfs_path + "\"");
    if (res.exit_code == 0) {
        return gvfs_path;
    }
    return "";
#endif
}

std::string mount_samba_share(const active_session& session,
                              const std::string& share_name,
                              const std::string& password,
                              const std::string& preferred_drive) {
    std::string host = resolve_samba_host(session);
    if (host.empty() || share_name.empty()) return "";

#ifdef _WIN32
    std::string unc_path = build_unc_path(host, share_name);
    DWORD status = 0;
    std::string existing = find_drive_for_unc(unc_path, &status);

    // If already connected and no explicit re-auth password was passed, return it immediately
    if (!existing.empty() && (status == NO_ERROR || password.empty())) {
        return existing;
    }

    char letter = 0;
    if (!existing.empty()) {
        letter = existing[0]; // Reuse existing drive letter to refresh credentials/connection
    } else {
        letter = pick_free_drive_letter(share_name, preferred_drive);
    }

    if (letter == 0) {
        return "";
    }

    std::string local_drive = std::string(1, letter) + ":";

    NETRESOURCEA nr{};
    nr.dwType = RESOURCETYPE_DISK;
    nr.lpLocalName = const_cast<LPSTR>(local_drive.c_str());
    nr.lpRemoteName = const_cast<LPSTR>(unc_path.c_str());
    nr.lpProvider = nullptr;

    const char* pass_ptr = password.empty() ? nullptr : password.c_str();
    const char* user_ptr = session.user.empty() ? nullptr : session.user.c_str();

    DWORD res = WNetAddConnection2A(&nr, pass_ptr, user_ptr, CONNECT_UPDATE_PROFILE);
    if (res == ERROR_SESSION_CREDENTIAL_CONFLICT) {
        // An existing SMB connection to the host already holds session credentials; reuse it
        res = WNetAddConnection2A(&nr, nullptr, nullptr, CONNECT_UPDATE_PROFILE);
    }

    if (res == NO_ERROR) {
        return local_drive + "\\";
    }
    return existing;
#elif __APPLE__
    std::string existing = get_mounted_samba_path(session, share_name);
    if (!existing.empty()) return existing;

    std::string auth;
    if (!session.user.empty()) {
        auth = session.user;
        if (!password.empty()) auth += ":" + password;
        auth += "@";
    }
    std::string smb_url = "smb://" + auth + host + "/" + share_name;
    auto res = utils::run_command("osascript -e 'mount volume \"" + smb_url + "\"'");
    if (res.exit_code == 0) {
        return get_mounted_samba_path(session, share_name);
    }
    return "";
#else
    std::string existing = get_mounted_samba_path(session, share_name);
    if (!existing.empty()) return existing;

    std::string smb_url = "smb://" + host + "/" + share_name;
    if (utils::has_command("gio")) {
        auto res = utils::run_command("gio mount \"" + smb_url + "\" < /dev/null");
        if (res.exit_code == 0) {
            return get_mounted_samba_path(session, share_name);
        }
    }
    return "";
#endif
}

bool unmount_samba_share(const active_session& session, const std::string& share_name) {
    std::string mounted = get_mounted_samba_path(session, share_name);
    if (mounted.empty()) return false;

#ifdef _WIN32
    std::string local_drive = mounted.substr(0, 2); // e.g. "M:"
    DWORD res = WNetCancelConnection2A(local_drive.c_str(), CONNECT_UPDATE_PROFILE, TRUE);
    return (res == NO_ERROR);
#elif __APPLE__
    auto res = utils::run_command("umount \"" + mounted + "\"");
    return (res.exit_code == 0);
#else
    std::string host = resolve_samba_host(session);
    std::string smb_url = "smb://" + host + "/" + share_name;
    if (utils::has_command("gio")) {
        auto res = utils::run_command("gio mount -u \"" + smb_url + "\"");
        return (res.exit_code == 0);
    }
    return false;
#endif
}

bool open_samba_share(const active_session& session, const std::string& share_name) {
    std::string host = resolve_samba_host(session);
    if (host.empty()) return false;

    // 1. Check if share is already mounted locally; if not, attempt silent mount
    std::string mounted_path = get_mounted_samba_path(session, share_name);
    if (mounted_path.empty()) {
        mounted_path = mount_samba_share(session, share_name);
    }

#ifdef _WIN32
    // 2. Open the mounted drive letter directly (e.g. "M:\\" or "S:\\"), falling back to UNC path
    std::string target_path = !mounted_path.empty() ? mounted_path : build_unc_path(host, share_name);
    HINSTANCE res = ShellExecuteA(NULL, "open", target_path.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(res) > 32) {
        return true;
    }
    // Fallback if ShellExecute failed to launch
    std::string cmd = "explorer.exe \"" + target_path + "\"";
    int ret = utils::run_interactive(cmd);
    return (ret == 0 || ret == 1);
#elif __APPLE__
    std::string target_path = !mounted_path.empty() ? mounted_path : ("smb://" + host + "/" + share_name);
    std::string cmd = "open \"" + target_path + "\"";
    return (utils::run_interactive(cmd) == 0);
#else
    std::string target_path = !mounted_path.empty() ? mounted_path : ("smb://" + host + "/" + share_name);
    std::string cmd = "xdg-open \"" + target_path + "\"";
    return (utils::run_interactive(cmd) == 0);
#endif
}

} // namespace smart_ssh::core
