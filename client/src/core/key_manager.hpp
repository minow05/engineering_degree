#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace smart_ssh::core {

struct ssh_key_pair {
    std::string key_name;
    std::filesystem::path private_key_path;
    std::filesystem::path public_key_path;
    std::string key_type;
};

[[nodiscard]] std::filesystem::path get_ssh_dir();
[[nodiscard]] std::vector<ssh_key_pair> list_keys();

[[nodiscard]] bool generate_key(const std::string& key_name,
                                const std::string& comment = "",
                                const std::string& passphrase = "",
                                const std::string& key_type = "ed25519");

[[nodiscard]] bool deploy_public_key(const std::filesystem::path& pub_key_path,
                                     const std::string& user,
                                     const std::string& host,
                                     int port = 22);

} // namespace smart_ssh::core
