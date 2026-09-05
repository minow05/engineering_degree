# SmartSSH Future Concepts & Thesis Extensions

This document details advanced architecture concepts, background daemon services, OS keychain integrations, and automated server-side installer tools designed as future extensions for the SmartSSH ecosystem.

---

## 1. Layer 2: Password Auto-Suggester & OS Daemon

### Overview
A cross-platform background daemon service (`password_daemon`) that automates password generation, credential auto-suggestion, and secure OS keyring persistence.

### Key Capabilities:
1. **Cryptographic Password Auto-Generation**:
   - Generates 20–32 character high-entropy passwords using `std::random_device` and `std::mt19937`.
2. **Platform OS Keychain Integration**:
   - **Windows**: Persists credentials to Windows Credential Manager (`cmdkey /generic:...`).
   - **Linux**: Integrates with Linux Secret Service API / GNOME Keyring / KWallet via D-Bus.
   - **macOS**: Persists credentials to macOS System Keychain via `security` Keychain Services API.
3. **Background OS Service Registration**:
   - **Windows**: Windows Scheduled Task (`schtasks /Create /TN "SmartSSHPasswordDaemon" /SC ONLOGON`).
   - **Linux**: Systemd user service unit (`~/.config/systemd/user/smart-ssh-daemon.service`).
   - **macOS**: Launchd agent plist (`~/Library/LaunchAgents/com.smartssh.daemon.plist`).

---

## 2. Server-Side Automated Provisioning Tool (`rpi_server_installer.sh`)

### Overview
A dedicated bare-metal installer script for setting up a fresh, unconfigured Raspberry Pi or Linux server for multi-user storage, process hosting, and secure network access.

### Capabilities:
- Creates isolated Linux user accounts with separate home partitions (`/home/<user>`).
- Configures global shared storage (`/home/shared`) with group permissions (`ssh-shared`).
- Provisions Samba (`/etc/samba/smb.conf`) for encrypted SMB network drives.
- Configures ZeroTier VPN and `wsdd` (WS-Discovery daemon) for automatic Windows Explorer network discovery.
- Configures Docker container runtime and user group access for hosting services.

---

## 3. Advanced Multi-Tenant Quotas & Cgroup Constraints

- Real-time user disk quota enforcement (`quota -u <user>`).
- Per-user Linux cgroup CPU & Memory limits for process hosting and containers.
