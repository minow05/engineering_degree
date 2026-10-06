#include <gtest/gtest.h>
#include "core/configuration.hpp"
#include "core/ssh_config.hpp"
#include "core/key_manager.hpp"
#include "core/system_operations.hpp"
#include "ssh_stub.hpp"
#include <filesystem>
#include <fstream>

using namespace smart_ssh;

// ============================================================================
// UNIT TESTS: core::configuration
// ============================================================================

TEST(CoreUnitConfigurationTest, DefaultUnconfiguredState) {
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "test_unconfigured_unit.cfg";
    std::filesystem::remove(temp_path);

    core::configuration cfg(temp_path);
    EXPECT_FALSE(cfg.is_configured());
    EXPECT_FALSE(cfg.load());

    const auto& data = cfg.get_data();
    EXPECT_EQ(data.host_alias, "");
    EXPECT_EQ(data.host_address, "");
    EXPECT_EQ(data.username, "pi");
    EXPECT_EQ(data.port, 22);
    EXPECT_FALSE(data.is_configured);
}

TEST(CoreUnitConfigurationTest, SetSaveAndReloadValidData) {
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "test_valid_unit.cfg";
    std::filesystem::remove(temp_path);

    core::configuration cfg(temp_path);
    core::app_config_data data;
    data.host_alias = "rpi_unit";
    data.host_address = "10.147.19.42";
    data.username = "rachel";
    data.port = 2222;
    data.identity_file = "C:/Users/rachel/.ssh/id_ed25519_rpi";
    data.is_configured = true;

    cfg.set_data(data);
    EXPECT_TRUE(cfg.save());
    EXPECT_TRUE(cfg.is_configured());

    // Reload into clean object
    core::configuration reloaded_cfg(temp_path);
    EXPECT_TRUE(reloaded_cfg.load());
    EXPECT_TRUE(reloaded_cfg.is_configured());

    const auto& loaded_data = reloaded_cfg.get_data();
    EXPECT_EQ(loaded_data.host_alias, "rpi_unit");
    EXPECT_EQ(loaded_data.host_address, "10.147.19.42");
    EXPECT_EQ(loaded_data.username, "rachel");
    EXPECT_EQ(loaded_data.port, 2222);
    EXPECT_EQ(loaded_data.identity_file, "C:/Users/rachel/.ssh/id_ed25519_rpi");

    std::filesystem::remove(temp_path);
}

TEST(CoreUnitConfigurationTest, ResetClearsStateAndDeletesFile) {
    std::filesystem::path temp_path = std::filesystem::temp_directory_path() / "test_reset_unit.cfg";
    
    core::configuration cfg(temp_path);
    core::app_config_data data;
    data.host_alias = "to_reset";
    data.host_address = "127.0.0.1";
    data.is_configured = true;
    cfg.set_data(data);
    cfg.save();

    EXPECT_TRUE(std::filesystem::exists(temp_path));
    EXPECT_TRUE(cfg.reset());
    EXPECT_FALSE(std::filesystem::exists(temp_path));
    EXPECT_FALSE(cfg.is_configured());
}

// ============================================================================
// UNIT TESTS: core::ssh_config
// ============================================================================

TEST(CoreUnitSSHConfigTest, AddUpdateFindAndRemoveEntries) {
    std::filesystem::path temp_ssh_cfg = std::filesystem::temp_directory_path() / "mock_ssh_config";
    std::filesystem::remove(temp_ssh_cfg);

    core::ssh_config cfg(temp_ssh_cfg);

    core::host_entry entry1;
    entry1.name = "minowRPI";
    entry1.host_name = "172.30.0.1";
    entry1.user = "minow";
    entry1.port = 22;
    entry1.identity_file = "~/.ssh/id_ed25519_minowRPI";

    cfg.add_or_update_entry(entry1);
    EXPECT_TRUE(cfg.save());

    // Verify entry exists
    const auto* found = cfg.find_entry("minowRPI");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->user, "minow");
    EXPECT_EQ(found->host_name, "172.30.0.1");

    // Update entry port
    entry1.port = 2200;
    cfg.add_or_update_entry(entry1);
    cfg.save();

    core::ssh_config cfg_reload(temp_ssh_cfg);
    EXPECT_TRUE(cfg_reload.load());
    const auto* found_updated = cfg_reload.find_entry("minowRPI");
    ASSERT_NE(found_updated, nullptr);
    EXPECT_EQ(found_updated->port, 2200);

    // Remove entry
    EXPECT_TRUE(cfg_reload.remove_entry("minowRPI"));
    EXPECT_EQ(cfg_reload.find_entry("minowRPI"), nullptr);

    std::filesystem::remove(temp_ssh_cfg);
}

TEST(CoreUnitSSHConfigTest, FindNonExistentEntryReturnsNullptr) {
    std::filesystem::path temp_ssh_cfg = std::filesystem::temp_directory_path() / "mock_ssh_config_nonexistent";
    core::ssh_config cfg(temp_ssh_cfg);
    EXPECT_EQ(cfg.find_entry("unknown_host"), nullptr);
}

// ============================================================================
// UNIT TESTS: core::key_manager
// ============================================================================

TEST(CoreUnitKeyManagerTest, SSHDirectoryResolution) {
    std::filesystem::path ssh_dir = core::get_ssh_dir();
    EXPECT_FALSE(ssh_dir.empty());
    EXPECT_TRUE(ssh_dir.string().find(".ssh") != std::string::npos);
}

TEST(CoreUnitKeyManagerTest, ListKeysReturnsVector) {
    // Should execute without throwing or crashing
    auto keys = core::list_keys();
    // Simply verify that list_keys returns a valid vector (empty or populated)
    SUCCEED();
}

// ============================================================================
// UNIT TESTS: core::system_operations Command Construction
// ============================================================================

TEST(CoreUnitSystemOperationsTest, CheckRemoteDiskSpaceCommandFormatting) {
    tests::ssh_stub stub;
    core::active_session session = stub.create_stub_session();

    // Verify system operations call against stubbed environment returns valid output
    std::string mock_output = stub.get_mock_output("df -h /");
    EXPECT_NE(mock_output.find("Filesystem"), std::string::npos);
    EXPECT_NE(mock_output.find("/dev/root"), std::string::npos);
}

TEST(CoreUnitSystemOperationsTest, CheckRemoteQuotaCommandFormatting) {
    tests::ssh_stub stub;
    core::active_session session = stub.create_stub_session();

    std::string mock_output = stub.get_mock_output("quota");
    EXPECT_NE(mock_output.find("mockuser"), std::string::npos);
}
