#include "ssh_stub.hpp"
#include <fstream>
#include <iostream>

namespace smart_ssh::tests {

ssh_stub::ssh_stub() {
    m_temp_dir = std::filesystem::temp_directory_path() / "smart_ssh_test_env";
    setup_stub_environment();
}

ssh_stub::~ssh_stub() {
    teardown_stub_environment();
}

void ssh_stub::setup_stub_environment() {
    std::filesystem::create_directories(m_temp_dir / ".ssh");

    // Create mock config
    std::ofstream cfg_file(m_temp_dir / ".ssh" / "config");
    cfg_file << "Host mockRPI\n"
             << "  HostName 127.0.0.1\n"
             << "  User mockuser\n"
             << "  Port 2222\n"
             << "  IdentityFile " << (m_temp_dir / ".ssh" / "id_stub").string() << "\n";
    cfg_file.close();

    // Create mock SSH key
    std::ofstream key_file(m_temp_dir / ".ssh" / "id_stub");
    key_file << "-----BEGIN OPENSSH PRIVATE KEY-----\nMOCK_KEY_DATA\n-----END OPENSSH PRIVATE KEY-----\n";
    key_file.close();

    // Setup mock response table
    m_mock_responses["df -h /"] = "Filesystem      Size  Used Avail Use% Mounted on\n/dev/root        117G   13G  101G  11% /\n";
    m_mock_responses["docker ps"] = "NAMES               STATUS          PORTS\nweb_app_container   Up 4 hours      0.0.0.0:80->80/tcp\n";
    m_mock_responses["quota"] = "Disk quotas for user mockuser (uid 1001):\n     Filesystem  blocks   quota   limit   grace   files   quota   limit   grace\n      /dev/root   13000  100000  120000            1200       0       0\n";
    m_mock_responses["ls -la /home/mockuser"] = "total 8\ndrwxr-xr-x 2 mockuser mockuser 4096 Sep 5 18:00 .\n-rw-r--r-- 1 mockuser mockuser 1024 Sep 5 18:00 test_document.txt\n";
}

void ssh_stub::teardown_stub_environment() {
    std::error_code ec;
    std::filesystem::remove_all(m_temp_dir, ec);
}

[[nodiscard]] core::active_session ssh_stub::create_stub_session() const {
    core::active_session session;
    session.host_alias = "mockRPI";
    session.host_address = "127.0.0.1";
    session.user = "mockuser";
    session.port = 2222;
    session.identity_file = (m_temp_dir / ".ssh" / "id_stub").string();
    session.is_connected = true; // Pseudo-environment connection active
    return session;
}

[[nodiscard]] std::string ssh_stub::get_mock_output(const std::string& command) {
    if (command.find("df") != std::string::npos) {
        return "Filesystem      Size  Used Avail Use% Mounted on\n/dev/root        117G   13G  101G  11% /\n";
    }
    if (command.find("docker") != std::string::npos) {
        return "NAMES               STATUS          PORTS\nweb_app_container   Up 4 hours      0.0.0.0:80->80/tcp\n";
    }
    if (command.find("quota") != std::string::npos) {
        return "Disk quotas for user mockuser (uid 1001):\n     Filesystem  blocks   quota   limit   grace   files   quota   limit   grace\n      /dev/root   13000  100000  120000            1200       0       0\n";
    }
    if (command.find("ls") != std::string::npos) {
        return "total 8\ndrwxr-xr-x 2 mockuser mockuser 4096 Sep 5 18:00 .\n-rw-r--r-- 1 mockuser mockuser 1024 Sep 5 18:00 test_document.txt\n";
    }
    return "STUB_COMMAND_SUCCESS\n";
}

} // namespace smart_ssh::tests
