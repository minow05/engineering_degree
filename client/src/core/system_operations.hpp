#pragma once

#include "connection_manager.hpp"
#include <string>

namespace smart_ssh::core {

std::string check_remote_disk_space(const active_session& session);
std::string check_remote_quota(const active_session& session);

/**
 * @brief Resolves the target Samba host/IP from session configuration.
 */
std::string resolve_samba_host(const active_session& session);

/**
 * @brief Returns the local mounted path (e.g., "M:\\" on Windows) if the Samba share
 *        is currently mapped/mounted, or an empty string otherwise.
 */
std::string get_mounted_samba_path(const active_session& session, const std::string& share_name);

/**
 * @brief Mounts the Samba share locally as a persistent network drive/volume.
 * @return The local mounted path on success, or an empty string on failure.
 */
std::string mount_samba_share(const active_session& session,
                              const std::string& share_name,
                              const std::string& password = "",
                              const std::string& preferred_drive = "");

/**
 * @brief Unmounts a locally mounted Samba share.
 */
bool unmount_samba_share(const active_session& session, const std::string& share_name);

/**
 * @brief Ensures the Samba share is mounted (if possible) and opens it in the OS file manager.
 */
bool open_samba_share(const active_session& session, const std::string& share_name = "SharedStorage");

} // namespace smart_ssh::core
