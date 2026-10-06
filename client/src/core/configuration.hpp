#pragma once

#include <string>
#include <filesystem>
#include <map>

namespace smart_ssh::core {

struct app_config_data {
    std::string host_alias;
    std::string host_address;
    std::string username{"pi"};
    int port{22};
    std::string identity_file;
    bool is_configured{false};
    std::map<std::string, std::string> custom_settings;
};

class configuration {
public:
    explicit configuration(std::filesystem::path config_file_path = get_default_config_path());

    bool load();
    bool save() const;
    bool reset(); // Reset local configuration without deleting remote data

    bool is_configured() const { return m_data.is_configured; }
    const app_config_data& get_data() const { return m_data; }
    app_config_data& get_data_mutable() { return m_data; }

    void set_data(const app_config_data& data) { m_data = data; }

    static std::filesystem::path get_default_config_path();

private:
    std::filesystem::path m_config_path;
    app_config_data m_data;
};

} // namespace smart_ssh::core
