#pragma once

#include "connection_manager.hpp"
#include <string>

namespace smart_ssh::core {

std::string check_remote_disk_space(const active_session& session);
std::string check_remote_quota(const active_session& session);
bool open_samba_share(const active_session& session, const std::string& share_name = "SharedStorage");

} // namespace smart_ssh::core
