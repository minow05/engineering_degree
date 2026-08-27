#pragma once

#include <string>

namespace smart_ssh::core {

[[nodiscard]] std::string check_remote_disk_space(const std::string& host_alias);
[[nodiscard]] std::string check_remote_quota(const std::string& host_alias, const std::string& user);
[[nodiscard]] bool open_samba_share(const std::string& host_address, const std::string& share_name = "SharedStorage");

} // namespace smart_ssh::core
