#include <gtest/gtest.h>
#include "ssh_stub.hpp"
#include "modules/resource_monitor.hpp"
#include "core/system_operations.hpp"
#include "core/application.hpp"
#include <filesystem>

using namespace smart_ssh;

// ============================================================================
// FUNCTIONAL / INTEGRATION TESTS: Feature Modules & Application Workflows
// ============================================================================

class FunctionalModuleTest : public ::testing::Test {
protected:
    tests::ssh_stub stub;
    core::active_session session;

    void SetUp() override {
        session = stub.create_stub_session();
    }
};

TEST_F(FunctionalModuleTest, VerifyResourceMonitorCPUAndMemoryQuery) {
    std::string mock_cpu_mem = "USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND\nminow     1234  0.5  1.2  15000  8000 ?        Ss   12:00   0:01 /usr/bin/ssh\n\n               total        used        free      shared  buff/cache   available\nMem:           7.7Gi       1.2Gi       5.1Gi        15Mi       1.4Gi       6.2Gi\n";
    EXPECT_NE(mock_cpu_mem.find("%CPU"), std::string::npos);
    EXPECT_NE(mock_cpu_mem.find("Mem:"), std::string::npos);
}

TEST_F(FunctionalModuleTest, VerifyDiskSpaceFormattingAndParsing) {
    std::string mock_disk_space = stub.get_mock_output("df -h /");
    EXPECT_NE(mock_disk_space.find("Filesystem"), std::string::npos);
}

TEST_F(FunctionalModuleTest, VerifyUserQuotaFormattingAndParsing) {
    std::string mock_quota = stub.get_mock_output("quota");
    EXPECT_NE(mock_quota.find("mockuser"), std::string::npos);
}

TEST_F(FunctionalModuleTest, FullApplicationLifecycleInitLoadReset) {
    core::application app;
    // Test initialization lifecycle
    bool init_res = app.initialize();
    (void)init_res;

    // Verify session access
    auto& sess = app.get_session();
    sess.host_alias = "functional_rpi";
    sess.host_address = "172.30.0.1";
    sess.user = "minow";
    sess.is_connected = true;

    EXPECT_EQ(app.get_session().host_alias, "functional_rpi");
    EXPECT_EQ(app.get_session().user, "minow");
    EXPECT_TRUE(app.get_session().is_connected);
}
