#include "application.hpp"
#include "installer/setup_wizard.hpp"
#include "ui/terminal.hpp"
#include <iostream>

namespace smart_ssh::core {

application::application() {}

bool application::initialize() {
    if (!m_config.load()) {
        return false;
    }

    const auto& data = m_config.get_data();
    host_entry entry;
    entry.name = data.host_alias;
    entry.host_name = data.host_address;
    entry.user = data.username;
    entry.port = data.port;
    entry.identity_file = data.identity_file;

    (void)establish_session(m_session, entry);
    return true;
}

bool application::is_configured() const {
    return m_config.is_configured();
}

bool application::run_first_time_wizard() {
    auto wiz_res = installer::setup_wizard::run_interactive_wizard();
    if (!wiz_res.success) {
        return false;
    }

    if (!installer::setup_wizard::configure_and_bind_host(wiz_res.config_data, m_config)) {
        return false;
    }

    return initialize();
}

bool application::reset_configuration() {
    ui::print_warning("Resetting local SmartSSH client configuration...");
    bool ok = m_config.reset();
    m_session = active_session{};
    if (ok) {
        ui::print_success("Local client configuration reset successfully! (Remote Pi data preserved)");
    } else {
        ui::print_error("Failed to reset local client configuration.");
    }
    return ok;
}

} // namespace smart_ssh::core
