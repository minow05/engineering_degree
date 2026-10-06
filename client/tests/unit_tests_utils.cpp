#include <gtest/gtest.h>
#include "utils/exec.hpp"
#include <string>

using namespace smart_ssh;

// ============================================================================
// UNIT TESTS: utils::exec
// ============================================================================

TEST(UtilsUnitExecTest, RunSimpleEchoCommand) {
    auto res = utils::run_command("echo SMART_SSH_UNIT_TEST_OUTPUT");
    EXPECT_EQ(res.exit_code, 0);
    EXPECT_NE(res.stdout_str.find("SMART_SSH_UNIT_TEST_OUTPUT"), std::string::npos);
}

TEST(UtilsUnitExecTest, RunCommandWithExitCodeCheck) {
#ifdef _WIN32
    auto res = utils::run_command("cmd /c exit 42");
    EXPECT_NE(res.exit_code, 0);
#else
    auto res = utils::run_command("sh -c 'exit 42'");
    EXPECT_EQ(res.exit_code, 42);
#endif
}

TEST(UtilsUnitExecTest, SystemCommandPresenceCheck) {
    EXPECT_TRUE(utils::has_command("cmake") || utils::has_command("git") || utils::has_command("ssh"));
    EXPECT_FALSE(utils::has_command("non_existent_binary_xyz_12345"));
}

TEST(UtilsUnitExecTest, RunEmptyCommandReturnsError) {
    auto res = utils::run_command("");
    EXPECT_NE(res.exit_code, 0);
}
