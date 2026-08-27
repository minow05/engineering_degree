#include "ssh_config.hpp"

namespace smart_ssh::core {

std::filesystem::path ssh_config::get_default_config_path() {
    // TODO: User implementation
    return "config";
}

ssh_config::ssh_config(std::filesystem::path config_path)
    : m_config_path(std::move(config_path)) {}

bool ssh_config::load() {
    // TODO: User implementation
    return true;
}

bool ssh_config::save() const {
    // TODO: User implementation
    return true;
}

void ssh_config::add_or_update_entry(const host_entry& entry) {
    // TODO: User implementation
}

bool ssh_config::remove_entry(const std::string& host_alias) {
    // TODO: User implementation
    return false;
}

const host_entry* ssh_config::find_entry(const std::string& host_alias) const {
    // TODO: User implementation
    return nullptr;
}

} // namespace smart_ssh::core
