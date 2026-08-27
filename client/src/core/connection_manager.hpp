#pragma once

#include "ssh_config.hpp"
#include <string>

namespace smart_ssh::core {

[[nodiscard]] int connect_host(const host_entry& host);
[[nodiscard]] int connect_host(const std::string& host_alias_or_address);
[[nodiscard]] bool test_connection(const std::string& host_address, int port = 22);

} // namespace smart_ssh::core
