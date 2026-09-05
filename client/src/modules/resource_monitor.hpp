#pragma once

#include "core/connection_manager.hpp"
#include <string>

namespace smart_ssh::modules {

void run_resource_monitor(const core::active_session& session);

[[nodiscard]] std::string check_cpu_memory_usage(const core::active_session& session);

} // namespace smart_ssh::modules
