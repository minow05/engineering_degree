#pragma once

#include "feature_toggle.hpp"
#include <string>
#include <vector>

namespace smart_ssh::installer {

struct wizard_config {
    std::string host_address;
    std::string host_alias;
    std::string primary_user{"minow"};
    std::vector<std::string> additional_users;
    bool enable_shared_folder{true};
    feature_flags features;
};

[[nodiscard]] wizard_config run_host_wizard();

} // namespace smart_ssh::installer
