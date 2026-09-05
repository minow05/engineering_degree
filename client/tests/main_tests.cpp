#include <gtest/gtest.h>
#include "ssh_stub.hpp"
#include "utils/exec.hpp"
#include "ui/terminal.hpp"
#include "core/ssh_config.hpp"
#include "core/key_manager.hpp"
#include "core/connection_manager.hpp"
#include "core/system_operations.hpp"
#include "modules/file_manager.hpp"
#include "modules/process_manager.hpp"
#include "modules/resource_monitor.hpp"

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
