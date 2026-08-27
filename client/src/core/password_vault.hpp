#pragma once

#include <string>
#include <map>
#include <vector>
#include <filesystem>

namespace smart_ssh::core {

class password_vault {
public:
    explicit password_vault(std::filesystem::path vault_path = get_default_vault_path());

    [[nodiscard]] bool load_vault(const std::string& master_password);
    [[nodiscard]] bool save_vault(const std::string& master_password);
    [[nodiscard]] bool vault_exists() const;

    void set_credential(const std::string& host_alias, const std::string& passphrase_or_password);
    [[nodiscard]] std::string get_credential(const std::string& host_alias) const;
    [[nodiscard]] bool remove_credential(const std::string& host_alias);
    [[nodiscard]] std::vector<std::string> list_stored_aliases() const;

    [[nodiscard]] static bool add_key_to_ssh_agent(const std::filesystem::path& key_path);
    [[nodiscard]] static std::filesystem::path get_default_vault_path();

private:
    std::filesystem::path m_vault_path;
    std::map<std::string, std::string> m_credentials;

    [[nodiscard]] static std::string encrypt_decrypt(const std::string& text, const std::string& key);
};

} // namespace smart_ssh::core
