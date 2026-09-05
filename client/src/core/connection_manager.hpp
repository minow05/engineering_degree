#pragma once

#include "ssh_config.hpp"
#include <string>

namespace smart_ssh::core {

struct active_session {
    std::string host_alias;
    std::string host_address;
    std::string user{"pi"};
    int port{22};
    std::string identity_file;
    bool is_connected{false};
};

[[nodiscard]] bool establish_session(active_session& session, const host_entry& host);
[[nodiscard]] int start_interactive_shell(const active_session& session);
[[nodiscard]] bool test_connection(const std::string& host_address, int port = 22);

} // namespace smart_ssh::core
