#include <gtest/gtest.h>
#include "ssh_stub.hpp"
#include "utils/exec.hpp"
#include "ui/terminal.hpp"
#include "core/configuration.hpp"
#include "core/application.hpp"
#include "core/ssh_config.hpp"
#include "core/key_manager.hpp"
#include "core/connection_manager.hpp"
#include "core/system_operations.hpp"
#include "installer/setup_wizard.hpp"
#include "modules/file_manager.hpp"
#include "modules/process_manager.hpp"
#include "modules/resource_monitor.hpp"
#include <filesystem>

using namespace smart_ssh;

class SSHStubEnvironmentTest : public ::testing::Test {
protected:
    tests::ssh_stub stub;
    core::active_session stub_session;

    void SetUp() override {
        stub_session = stub.create_stub_session();
    }
};

TEST_F(SSHStubEnvironmentTest, StubSessionConnectionActive) {
    EXPECT_TRUE(stub_session.is_connected);
    EXPECT_EQ(stub_session.host_alias, "mockRPI");
    EXPECT_EQ(stub_session.host_address, "127.0.0.1");
    EXPECT_EQ(stub_session.user, "mockuser");
    EXPECT_EQ(stub_session.port, 2222);
}

TEST_F(SSHStubEnvironmentTest, MockDiskSpaceOutput) {
    std::string mock_df = tests::ssh_stub::get_mock_output("df -h /");
    EXPECT_NE(mock_df.find("Filesystem"), std::string::npos);
    EXPECT_NE(mock_df.find("/dev/root"), std::string::npos);
}

TEST_F(SSHStubEnvironmentTest, MockDockerProcessOutput) {
    std::string mock_docker = tests::ssh_stub::get_mock_output("docker ps");
    EXPECT_NE(mock_docker.find("web_app_container"), std::string::npos);
}

TEST_F(SSHStubEnvironmentTest, MockUserQuotaOutput) {
    std::string mock_quota = tests::ssh_stub::get_mock_output("quota");
    EXPECT_NE(mock_quota.find("mockuser"), std::string::npos);
}

TEST_F(SSHStubEnvironmentTest, MockFileListingOutput) {
    std::string mock_ls = tests::ssh_stub::get_mock_output("ls -la /home/mockuser");
    EXPECT_NE(mock_ls.find("test_document.txt"), std::string::npos);
}

TEST(SmartSSHConfigurationTest, SaveLoadAndResetLocalConfig) {
    std::filesystem::path test_cfg_path = "stub_app_config.cfg";
    core::configuration cfg(test_cfg_path);

    // Initial state should be unconfigured
    EXPECT_FALSE(cfg.load());
    EXPECT_FALSE(cfg.is_configured());

    // Populate data
    core::app_config_data data;
    data.host_alias = "testRPI";
    data.host_address = "192.168.1.100";
    data.username = "pi";
    data.port = 22;
    data.identity_file = "~/.ssh/id_ed25519_testRPI";
    data.is_configured = true;

    cfg.set_data(data);
    EXPECT_TRUE(cfg.save());

    // Load & verify
    core::configuration loaded_cfg(test_cfg_path);
    EXPECT_TRUE(loaded_cfg.load());
    EXPECT_TRUE(loaded_cfg.is_configured());
    EXPECT_EQ(loaded_cfg.get_data().host_alias, "testRPI");
    EXPECT_EQ(loaded_cfg.get_data().host_address, "192.168.1.100");

    // Reset local configuration
    EXPECT_TRUE(loaded_cfg.reset());
    EXPECT_FALSE(loaded_cfg.is_configured());

    std::filesystem::remove(test_cfg_path);
}

TEST(SmartSSHExecTest, CommandExecutionCheck) {
    EXPECT_TRUE(utils::has_command("ssh") || utils::has_command("cmd") || utils::has_command("bash"));
    auto res = utils::run_command("echo Stub_Test_OK");
    EXPECT_NE(res.stdout_str.find("Stub_Test_OK"), std::string::npos);
}

TEST(SmartSSHConfigTest, IsolatedConfigParserAndSave) {
    core::ssh_config cfg("stub_test_config.tmp");
    
    core::host_entry entry;
    entry.name = "stub_RPI";
    entry.host_name = "127.0.0.1";
    entry.user = "stub_user";
    entry.port = 2222;

    cfg.add_or_update_entry(entry);
    EXPECT_TRUE(cfg.save());

    core::ssh_config loaded_cfg("stub_test_config.tmp");
    EXPECT_TRUE(loaded_cfg.load());
    
    const auto* found = loaded_cfg.find_entry("stub_RPI");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->user, "stub_user");
    EXPECT_EQ(found->host_name, "127.0.0.1");

    std::filesystem::remove("stub_test_config.tmp");
}
