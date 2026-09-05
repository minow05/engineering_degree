#pragma once

#include "core/connection_manager.hpp"
#include <string>

namespace smart_ssh::modules {

void run_process_manager(const core::active_session& session);

[[nodiscard]] std::string list_docker_containers(const core::active_session& session);
[[nodiscard]] std::string get_docker_logs(const core::active_session& session, const std::string& container_name);
[[nodiscard]] bool control_docker_container(const core::active_session& session, const std::string& container_name, const std::string& action);

} // namespace smart_ssh::modules
