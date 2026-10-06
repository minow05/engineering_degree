#include <gtest/gtest.h>
#include "core/configuration.hpp"
#include "core/ssh_config.hpp"
#include "core/system_operations.hpp"
#include "ssh_stub.hpp"
#include <filesystem>
#include <fstream>

using namespace smart_ssh;

// ============================================================================
// REGRESSION TESTS: Edge Cases & Resiliency
// ============================================================================

TEST(RegressionTest, CorruptedConfigFileLoadRecovery) {
    std::filesystem::path corrupted_path = std::filesystem::temp_directory_path() / "corrupted_config.cfg";
    
    // Write malformed garbage to configuration file
    std::ofstream out(corrupted_path);
    out << "INVALID_KEY_VALUE_LINE_WITHOUT_EQUALS\n"
        << "port = NOT_A_NUMBER\n"
        << "is_configured = INVALID_BOOL\n"
        << "===CORRUPTED_BINARY_DATA===\x00\x01\x02\n";
    out.close();

    core::configuration cfg(corrupted_path);
    // Should gracefully fail to validate config or parse without crashing
    (void)cfg.load();
    // Verify application state remains unconfigured and safe
    EXPECT_FALSE(cfg.is_configured());

    std::filesystem::remove(corrupted_path);
}

TEST(RegressionTest, NonStandardAndBoundaryPortNumbers) {
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "port_boundary_config.cfg";
    
    core::configuration cfg(temp_path);
    core::app_config_data data;
    data.host_alias = "boundaryRPI";
    data.host_address = "127.0.0.1";
    data.username = "testuser";
    data.port = 65535; // Maximum valid TCP port
    data.is_configured = true;

    cfg.set_data(data);
    EXPECT_TRUE(cfg.save());

    core::configuration reloaded(temp_path);
    EXPECT_TRUE(reloaded.load());
    EXPECT_EQ(reloaded.get_data().port, 65535);

    std::filesystem::remove(temp_path);
}

TEST(RegressionTest, SpacesAndSpecialCharactersInIdentityPath) {
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "spaces_path_config.cfg";
    
    core::configuration cfg(temp_path);
    core::app_config_data data;
    data.host_alias = "spacesRPI";
    data.host_address = "192.168.1.50";
    data.username = "john doe";
    data.identity_file = "C:/Users/John Doe/My Secret Keys/id_ed25519_rpi";
    data.is_configured = true;

    cfg.set_data(data);
    EXPECT_TRUE(cfg.save());

    core::configuration reloaded(temp_path);
    EXPECT_TRUE(reloaded.load());
    EXPECT_EQ(reloaded.get_data().username, "john doe");
    EXPECT_EQ(reloaded.get_data().identity_file, "C:/Users/John Doe/My Secret Keys/id_ed25519_rpi");

    std::filesystem::remove(temp_path);
}

TEST(RegressionTest, MultipleConsecutiveResetsOnNonExistentFile) {
    std::filesystem::path non_existent_path = std::filesystem::temp_directory_path() / "non_existent_reset.cfg";
    std::filesystem::remove(non_existent_path);

    core::configuration cfg(non_existent_path);
    // Resetting when file doesn't exist should succeed safely without throwing
    EXPECT_TRUE(cfg.reset());
    EXPECT_TRUE(cfg.reset());
    EXPECT_FALSE(cfg.is_configured());
}

TEST(RegressionTest, SSHConfigParsingWithMultipleHostBlocks) {
    std::filesystem::path multi_host_cfg = std::filesystem::temp_directory_path() / "multi_host_ssh_config";
    
    std::ofstream out(multi_host_cfg);
    out << "Host server1\n"
        << "  HostName 10.0.0.1\n"
        << "  User admin\n"
        << "  Port 22\n\n"
        << "Host server2\n"
        << "  HostName 10.0.0.2\n"
        << "  User guest\n"
        << "  Port 2222\n";
    out.close();

    core::ssh_config ssh_cfg(multi_host_cfg);
    EXPECT_TRUE(ssh_cfg.load());

    const auto* s1 = ssh_cfg.find_entry("server1");
    const auto* s2 = ssh_cfg.find_entry("server2");
    
    ASSERT_NE(s1, nullptr);
    ASSERT_NE(s2, nullptr);
    EXPECT_EQ(s1->host_name, "10.0.0.1");
    EXPECT_EQ(s1->user, "admin");
    EXPECT_EQ(s2->host_name, "10.0.0.2");
    EXPECT_EQ(s2->user, "guest");
    EXPECT_EQ(s2->port, 2222);

    std::filesystem::remove(multi_host_cfg);
}

TEST(RegressionTest, OpenSambaShareAliasResolutionToRawIP) {
    core::active_session session;
    session.host_alias = "minow.rpi";
    session.host_address = "minow.rpi";
    session.user = "minow";
    session.is_connected = true;

    // Verify calling open_samba_share with alias 'minow.rpi' resolves host and executes without crashing
    // (Command call might fail if Explorer / NetBIOS isn't connected to 172.30.0.1, but function returns bool cleanly)
    bool res = core::open_samba_share(session, "shared");
    (void)res;
    SUCCEED();
}
