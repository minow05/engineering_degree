#pragma once

#include "core/connection_manager.hpp"
#include "core/ssh_config.hpp"
#include <string>
#include <map>
#include <filesystem>

namespace smart_ssh::tests {

class ssh_stub {
public:
    ssh_stub();
    ~ssh_stub();

    void setup_stub_environment();
    void teardown_stub_environment();

    [[nodiscard]] core::active_session create_stub_session() const;
    [[nodiscard]] static std::string get_mock_output(const std::string& command);

private:
    std::filesystem::path m_temp_dir;
    std::map<std::string, std::string> m_mock_responses;
};

} // namespace smart_ssh::tests
