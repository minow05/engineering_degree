#include "key_manager.hpp"

namespace smart_ssh::core {

std::filesystem::path get_ssh_dir() {
    // TODO: User implementation
    return ".ssh";
}

std::vector<ssh_key_pair> list_keys() {
    std::vector<ssh_key_pair> keys;
    // TODO: User implementation
    return keys;
}

bool generate_key(const std::string& key_name,
                  const std::string& comment,
                  const std::string& passphrase,
                  const std::string& key_type) {
    // TODO: User implementation
    return false;
}

bool deploy_public_key(const std::filesystem::path& pub_key_path,
                       const std::string& user,
                       const std::string& host,
                       int port) {
    // TODO: User implementation
    return false;
}

} // namespace smart_ssh::core
