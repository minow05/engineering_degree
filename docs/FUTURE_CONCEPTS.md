# SmartSSH Future Concepts & Thesis Extensions

This document details advanced architecture concepts, background daemon services, hardware integrations, OS keychain integrations, and automated server-side installer tools designed as future extensions for the SmartSSH ecosystem.

---

## 1. Print Server & Hardware Resource Management (CUPS & Bambu Lab 3D Printer)

### A. Standard Document Printer Management (CUPS Integration)
* **Overview**: Remote 2D document print server management via **CUPS (Common Unix Printing System)** hosted on the server / Raspberry Pi.
* **Capabilities**:
  * Submit print jobs remotely (`lp`, `lpr`) over SSH or local HTTP API (`http://<host>:631`).
  * Inspect active print queues (`lpstat -o`), printer statuses (`lpstat -p`), and paper/ink levels.
  * Cancel or reprioritize print jobs (`cancel <job_id>`).

### B. 3D Printer Remote Management (Bambu Lab Integration)
* **Overview**: Dedicated 3D printing module integrating with **Bambu Lab 3D Printers** (e.g. X1-Carbon, P1P, P1S, A1 series) over local network MQTT/TLS protocol.
* **Capabilities**:
  * **Real-time Telemetry Monitoring**: Monitor hotend/nozzle temperature, heatbed temperature, print fan speed, chamber temperature, and print progress percentage.
  * **AMS (Automatic Material System) Status**: View loaded filament colors, material types (PLA, PETG, ABS, TPU), and active spool slot.
  * **3D Print Job Dispatch**: Upload 3D sliced G-code / `.gcode.3mf` files and launch prints remotely.
  * **Print Controls**: Start, pause, resume, or emergency stop active 3D print jobs.
  * **GitHub Open-Source Libraries**: Integration leveraging Bambu Lab local MQTT protocol integrations (e.g., `bambu-cli`, `bambubus`, `bambu-farm` community libraries).

---

## 2. Layer 2: Password Auto-Suggester & OS Daemon

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

## 3. Server-Side Automated Provisioning Tool (`rpi_server_installer.sh`)

### Overview
A dedicated bare-metal installer script for setting up a fresh, unconfigured Raspberry Pi or Linux server for multi-user storage, process hosting, and secure network access.

### Capabilities:
- Creates isolated Linux user accounts with separate home partitions (`/home/<user>`).
- Configures global shared storage (`/home/shared`) with group permissions (`ssh-shared`).
- Provisions Samba (`/etc/samba/smb.conf`) for encrypted SMB network drives.
- Configures ZeroTier VPN and `wsdd` (WS-Discovery daemon) for automatic Windows Explorer network discovery.
- Configures Docker container runtime and user group access for hosting services.

---

## 4. Advanced Multi-Tenant Quotas & Cgroup Constraints

- Real-time user disk quota enforcement (`quota -u <user>`).
- Per-user Linux cgroup CPU & Memory limits for process hosting and containers.
