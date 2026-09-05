#pragma once

#include "core/connection_manager.hpp"
#include <string>

namespace smart_ssh::modules {

void run_file_manager(const core::active_session& session);
[[nodiscard]] std::string list_remote_directory(const core::active_session& session, const std::string& remote_path);

} // namespace smart_ssh::modules
