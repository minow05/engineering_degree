#include "ssh_config.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

namespace smart_ssh::core {

[[nodiscard]] std::filesystem::path ssh_config::get_default_config_path() {
#ifdef _WIN32
    const char* user_profile = std::getenv("USERPROFILE");
    if (user_profile) {
        return std::filesystem::path(user_profile) / ".ssh" / "config";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".ssh" / "config";
    }
#endif
    return "config";
}

ssh_config::ssh_config(std::filesystem::path config_path)
    : m_config_path(std::move(config_path)) {}

static std::string trim(const std::string& str) {
    auto first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    auto last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

[[nodiscard]] bool ssh_config::load() {
    m_entries.clear();
    if (!std::filesystem::exists(m_config_path)) {
        return true;
    }

    std::ifstream file(m_config_path);
    if (!file.is_open()) return false;

    std::string line;
    host_entry current_entry;
    bool has_current = false;

    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        std::istringstream iss(trimmed);
        std::string key, val;
        iss >> key;
        std::getline(iss, val);
        val = trim(val);

        std::string lower_key = key;
        std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (lower_key == "host") {
            if (has_current && !current_entry.name.empty()) {
                m_entries.push_back(current_entry);
            }
            current_entry = host_entry{};
            current_entry.name = val;
            has_current = true;
        } else if (has_current) {
            if (lower_key == "hostname") {
                current_entry.host_name = val;
            } else if (lower_key == "user") {
                current_entry.user = val;
            } else if (lower_key == "port") {
                try { current_entry.port = std::stoi(val); } catch(...) { current_entry.port = 22; }
            } else if (lower_key == "identityfile") {
                current_entry.identity_file = val;
            } else {
                current_entry.extra_options[key] = val;
            }
        }
    }

    if (has_current && !current_entry.name.empty()) {
        m_entries.push_back(current_entry);
    }

    return true;
}

[[nodiscard]] bool ssh_config::save() const {
    if (m_config_path.has_parent_path()) {
        std::filesystem::create_directories(m_config_path.parent_path());
    }

    std::ofstream file(m_config_path);
    if (!file.is_open()) return false;

    file << "# SmartSSH Config File\n\n";
    for (const auto& entry : m_entries) {
        file << "Host " << entry.name << "\n";
        if (!entry.host_name.empty())     file << "  HostName " << entry.host_name << "\n";
        if (!entry.user.empty())         file << "  User " << entry.user << "\n";
        if (entry.port != 22)            file << "  Port " << entry.port << "\n";
        if (!entry.identity_file.empty()) file << "  IdentityFile " << entry.identity_file << "\n";
        for (const auto& [k, v] : entry.extra_options) {
            file << "  " << k << " " << v << "\n";
        }
        file << "\n";
    }

    return true;
}

void ssh_config::add_or_update_entry(const host_entry& entry) {
    auto it = std::find_if(m_entries.begin(), m_entries.end(), [&](const host_entry& e) {
        return e.name == entry.name;
    });

    if (it != m_entries.end()) {
        *it = entry;
    } else {
        m_entries.push_back(entry);
    }
}

[[nodiscard]] bool ssh_config::remove_entry(const std::string& host_alias) {
    auto it = std::remove_if(m_entries.begin(), m_entries.end(), [&](const host_entry& e) {
        return e.name == host_alias;
    });
    if (it != m_entries.end()) {
        m_entries.erase(it, m_entries.end());
        return true;
    }
    return false;
}

[[nodiscard]] const host_entry* ssh_config::find_entry(const std::string& host_alias) const {
    for (const auto& e : m_entries) {
        if (e.name == host_alias) return &e;
    }
    return nullptr;
}

} // namespace smart_ssh::core
