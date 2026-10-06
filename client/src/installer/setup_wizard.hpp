#pragma once

#include "core/configuration.hpp"
#include <string>

namespace smart_ssh::installer {

struct setup_wizard_result {
    bool success{false};
    core::app_config_data config_data;
    std::string error_message;
};

class setup_wizard {
public:
    static setup_wizard_result run_interactive_wizard();
    static bool configure_and_bind_host(const core::app_config_data& input_data, core::configuration& app_config);
};

} // namespace smart_ssh::installer
