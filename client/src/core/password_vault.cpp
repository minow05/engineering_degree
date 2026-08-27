#include "password_vault.hpp"

namespace smart_ssh::core {

std::filesystem::path password_vault::get_default_vault_path() {
    // TODO: User implementation
    return "vault.dat";
}

password_vault::password_vault(std::filesystem::path vault_path)
    : m_vault_path(std::move(vault_path)) {}

bool password_vault::vault_exists() const {
    return std::filesystem::exists(m_vault_path);
}

std::string password_vault::encrypt_decrypt(const std::string& text, const std::string& key) {
    // TODO: User implementation
    return text;
}

bool password_vault::load_vault(const std::string& master_password) {
    // TODO: User implementation
    return true;
}

bool password_vault::save_vault(const std::string& master_password) {
    // TODO: User implementation
    return true;
}

void password_vault::set_credential(const std::string& host_alias, const std::string& passphrase_or_password) {
    m_credentials[host_alias] = passphrase_or_password;
}

std::string password_vault::get_credential(const std::string& host_alias) const {
    auto it = m_credentials.find(host_alias);
    if (it != m_credentials.end()) return it->second;
    return "";
}

bool password_vault::remove_credential(const std::string& host_alias) {
    return m_credentials.erase(host_alias) > 0;
}

std::vector<std::string> password_vault::list_stored_aliases() const {
    std::vector<std::string> list;
    for (const auto& [alias, _] : m_credentials) {
        list.push_back(alias);
    }
    return list;
}

bool password_vault::add_key_to_ssh_agent(const std::filesystem::path& key_path) {
    // TODO: User implementation
    return false;
}

} // namespace smart_ssh::core
