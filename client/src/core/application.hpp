#pragma once

#include "configuration.hpp"
#include "connection_manager.hpp"

namespace smart_ssh::core {

class application {
public:
    application();

    bool initialize();
    bool is_configured() const;

    bool run_first_time_wizard();
    bool reset_configuration();

    active_session& get_session() { return m_session; }
    const active_session& get_session() const { return m_session; }

    const configuration& get_config() const { return m_config; }

private:
    configuration m_config;
    active_session m_session;
};

} // namespace smart_ssh::core
