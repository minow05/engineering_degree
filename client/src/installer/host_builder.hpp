#pragma once

#include "host_wizard.hpp"
#include <string>

namespace smart_ssh::installer {

[[nodiscard]] bool generate_standalone_host_executable(const wizard_config& config);

} // namespace smart_ssh::installer
