#include "configuration.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace smart_ssh::core {

std::filesystem::path configuration::get_default_config_path() {
#ifdef _WIN32
    const char* app_data = std::getenv("APPDATA");
    if (app_data) {
        return std::filesystem::path(app_data) / "SmartSSH" / "smartssh_config.cfg";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".config" / "smartssh" / "smartssh_config.cfg";
    }
#endif
    return std::filesystem::current_path() / "smartssh_config.cfg";
}

configuration::configuration(std::filesystem::path config_file_path)
    : m_config_path(std::move(config_file_path)) {}

static std::string trim(const std::string& str) {
    auto first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    auto last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool configuration::load() {
    m_data = app_config_data{};
    if (!std::filesystem::exists(m_config_path)) {
        return false;
    }

    std::ifstream file(m_config_path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        auto eq_pos = trimmed.find('=');
        if (eq_pos == std::string::npos) continue;

        std::string key = trim(trimmed.substr(0, eq_pos));
        std::string val = trim(trimmed.substr(eq_pos + 1));

        if (key == "host_alias") {
            m_data.host_alias = val;
        } else if (key == "host_address") {
            m_data.host_address = val;
        } else if (key == "username") {
            m_data.username = val;
        } else if (key == "port") {
            try { m_data.port = std::stoi(val); } catch (...) { m_data.port = 22; }
        } else if (key == "identity_file") {
            m_data.identity_file = val;
        } else if (key == "is_configured") {
            m_data.is_configured = (val == "true" || val == "1");
        } else {
            m_data.custom_settings[key] = val;
        }
    }

    return m_data.is_configured;
}

bool configuration::save() const {
    if (m_config_path.has_parent_path()) {
        std::filesystem::create_directories(m_config_path.parent_path());
    }

    std::ofstream file(m_config_path);
    if (!file.is_open()) return false;

    file << "# SmartSSH Single-Host Application Configuration\n";
    file << "host_alias = " << m_data.host_alias << "\n";
    file << "host_address = " << m_data.host_address << "\n";
    file << "username = " << m_data.username << "\n";
    file << "port = " << m_data.port << "\n";
    file << "identity_file = " << m_data.identity_file << "\n";
    file << "is_configured = " << (m_data.is_configured ? "true" : "false") << "\n";

    for (const auto& [k, v] : m_data.custom_settings) {
        file << k << " = " << v << "\n";
    }

    return true;
}

bool configuration::reset() {
    m_data = app_config_data{};
    m_data.is_configured = false;

    std::error_code ec;
    if (std::filesystem::exists(m_config_path)) {
        std::filesystem::remove(m_config_path, ec);
    }
    return !ec;
}

} // namespace smart_ssh::core
