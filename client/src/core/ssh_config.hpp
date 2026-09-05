#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <map>

namespace smart_ssh::core {

struct host_entry {
    std::string name;          // Host alias (e.g. minowRPI)
    std::string host_name;     // HostName/IP (e.g. minow.rpi or 172.30.0.1)
    std::string user;          // SSH User (e.g. minow)
    int port{22};              // Port
    std::string identity_file; // Private key path
    std::map<std::string, std::string> extra_options;
};

class ssh_config {
public:
    explicit ssh_config(std::filesystem::path config_path = get_default_config_path());

    [[nodiscard]] bool load();
    [[nodiscard]] bool save() const;

    [[nodiscard]] const std::vector<host_entry>& get_entries() const { return m_entries; }
    void add_or_update_entry(const host_entry& entry);
    [[nodiscard]] bool remove_entry(const std::string& host_alias);
    [[nodiscard]] const host_entry* find_entry(const std::string& host_alias) const;

    [[nodiscard]] static std::filesystem::path get_default_config_path();

private:
    std::filesystem::path m_config_path;
    std::vector<host_entry> m_entries;
};

} // namespace smart_ssh::core
