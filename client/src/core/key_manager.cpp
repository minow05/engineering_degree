#include "key_manager.hpp"
#include "utils/exec.hpp"
#include <cstdlib>
#include <fstream>

namespace smart_ssh::core {

std::filesystem::path get_ssh_dir() {
#ifdef _WIN32
    const char* user_profile = std::getenv("USERPROFILE");
    if (user_profile) {
        return std::filesystem::path(user_profile) / ".ssh";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".ssh";
    }
#endif
    return ".ssh";
}

std::vector<ssh_key_pair> list_keys() {
    std::vector<ssh_key_pair> keys;
    auto ssh_dir = get_ssh_dir();
    if (!std::filesystem::exists(ssh_dir)) return keys;

    for (const auto& entry : std::filesystem::directory_iterator(ssh_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pub") {
            ssh_key_pair kp;
            kp.public_key_path = entry.path();
            kp.private_key_path = entry.path().parent_path() / entry.path().stem();
            kp.key_name = entry.path().stem().string();
            kp.key_type = "SSH Public Key";
            keys.push_back(kp);
        }
    }
    return keys;
}

bool generate_key(const std::string& key_name,
                  const std::string& comment,
                  const std::string& passphrase,
                  const std::string& key_type) {
    auto ssh_dir = get_ssh_dir();
    std::filesystem::create_directories(ssh_dir);

    auto target_key_path = ssh_dir / key_name;
    std::string cmd = "ssh-keygen -t " + key_type + " -f \"" + target_key_path.string() + "\"";
    if (!comment.empty()) {
        cmd += " -C \"" + comment + "\"";
    }
    cmd += " -N \"" + passphrase + "\"";

    int exit_code = utils::run_interactive(cmd);
    return (exit_code == 0);
}

bool deploy_public_key(const std::filesystem::path& pub_key_path,
                       const std::string& user,
                       const std::string& host,
                       int port) {
#ifndef _WIN32
    if (utils::has_command("ssh-copy-id")) {
        std::string cmd = "ssh-copy-id -p " + std::to_string(port) + " -i \"" + pub_key_path.string() + "\" " + user + "@" + host;
        return (utils::run_interactive(cmd) == 0);
    }
#endif

    std::string cmd = "ssh -p " + std::to_string(port) + " " + user + "@" + host +
                      " \"mkdir -p ~/.ssh && chmod 700 ~/.ssh && cat >> ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys\" < \"" +
                      pub_key_path.string() + "\"";
    return (utils::run_interactive(cmd) == 0);
}

bool provision_remote_user(const std::string& admin_user,
                           const std::string& host,
                           int port,
                           const std::string& new_user,
                           const std::filesystem::path& pub_key_path,
                           const std::string& user_password) {
    std::string pub_key_content;
    if (std::filesystem::exists(pub_key_path)) {
        std::ifstream f(pub_key_path);
        if (f.is_open()) {
            std::getline(f, pub_key_content);
        }
    }

    std::string script_cmd = "sudo /home/" + admin_user + "/create_user.sh " + new_user +
                             " '" + pub_key_content + "' '" + user_password + "'";

    std::string inline_fallback = "sudo bash -c 'id -u " + new_user + " >/dev/null 2>&1 || useradd -m -s /bin/bash " + new_user +
                                  "; chmod 700 /home/" + new_user +
                                  "; mkdir -p /home/" + new_user + "/.ssh && chmod 700 /home/" + new_user + "/.ssh" +
                                  "; echo \\\"" + pub_key_content + "\\\" >> /home/" + new_user + "/.ssh/authorized_keys" +
                                  "; chmod 600 /home/" + new_user + "/.ssh/authorized_keys" +
                                  "; chown -R " + new_user + ":" + new_user + " /home/" + new_user + "/.ssh'";

    std::string remote_cmd = script_cmd + " || " + inline_fallback;
    std::string cmd = "ssh -t -p " + std::to_string(port) + " " + admin_user + "@" + host +
                      " \"" + remote_cmd + "\"";

    return (utils::run_interactive(cmd) == 0);
}

} // namespace smart_ssh::core
