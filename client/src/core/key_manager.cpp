#include "key_manager.hpp"
#include "utils/exec.hpp"
#include <cstdlib>

namespace smart_ssh::core {

[[nodiscard]] std::filesystem::path get_ssh_dir() {
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

[[nodiscard]] std::vector<ssh_key_pair> list_keys() {
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

[[nodiscard]] bool generate_key(const std::string& key_name,
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

[[nodiscard]] bool deploy_public_key(const std::filesystem::path& pub_key_path,
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

} // namespace smart_ssh::core
