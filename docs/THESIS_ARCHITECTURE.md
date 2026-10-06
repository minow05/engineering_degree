# Engineering Thesis Architecture Specification: SmartSSH

**Thesis Title:** *Projekt i implementacja zdalnego dostępu do prywatnych zasobów plikowych z wykorzystaniem platformy Raspberry Pi*  
**Platform Target:** Single-Host Client Application for Remote Raspberry Pi Resource Management  
**Language & Build:** C++20 | CMake | Clang (`clang-cl`) | GoogleTest

---

## 1. Executive Summary & System Model

SmartSSH is a dedicated single-host client application designed for secure remote access, storage management, process hosting, and resource monitoring on a private Raspberry Pi server.

Unlike multi-host SSH connection managers (e.g. PuTTY, Termius), SmartSSH operates under a **Single-Host Binding Model**:
* Upon first execution, the client enters a **Setup Wizard** to configure the target Raspberry Pi IP/address, SSH username, SSH key pair (`ed25519`), and network storage shares.
* Once configured, the local application persists its configuration (`smartssh_config.cfg`) and directly connects to the bound Raspberry Pi host on subsequent launches without prompting the user.
* Reconfiguration or binding to a different server is accomplished explicitly via **Settings → Reset Configuration**, which clears local client binding while leaving all remote Raspberry Pi files intact.

```
┌─────────────────────────────────────────────────────────┐
│                     SmartSSH Client                     │
│                                                         │
│   ┌─────────────────────────────────────────────────┐   │
│   │               Presentation Layer                │   │
│   │        ┌───────────────┐     ┌───────────────┐  │   │
│   │        │ CLI (Current) │     │ GUI (Target)  │  │   │
│   │        └───────┬───────┘     └───────┬───────┘  │   │
│   └────────────────┼─────────────────────┼──────────┘   │
│                    │                     │              │
│   ┌────────────────▼─────────────────────▼──────────┐   │
│   │                Application Core                 │   │
│   │  ┌──────────────┐  ┌──────────────┐  ┌────────┐ │   │
│   │  │ Application  │  │ Configuration│  │ Session│ │   │
│   │  └──────────────┘  └──────────────┘  └────────┘ │   │
│   └────────────────────────┬────────────────────────┘   │
│                            │                            │
│   ┌────────────────────────▼────────────────────────┐   │
│   │                Module Services                  │   │
│   │ ┌──────────────┐ ┌────────────────┐ ┌─────────┐ │   │
│   │ │ File Manager │ │Process Manager │ │ Monitors│ │   │
│   │ └──────────────┘ └────────────────┘ └─────────┘ │   │
│   └────────────────────────┬────────────────────────┘   │
└────────────────────────────┼────────────────────────────┘
                             │ SSH (Port 22) / ZeroTier VPN
                             v
┌─────────────────────────────────────────────────────────┐
│              Raspberry Pi Server (Remote)               │
│                                                         │
│  - Linux Kernel & OpenSSH Server                        │
│  - Isolated User Homes (/home/<user>)                   │
│  - Samba Encrypted SMB Shares (/home/<user>/shared)     │
│  - Docker Container Runtime & systemd Services          │
│  - Linux Storage Quotas (quota -u)                      │
└─────────────────────────────────────────────────────────┘
```

---

## 2. Core Architecture & Layer Separation

### A. Presentation Layer (`ui/`)
- **CLI (`ui/cli/`)**: Provides an interactive terminal menu system with non-canonical instant keypress reading (`read_key_immediate()`).
- **GUI (`ui/gui/`)**: Decoupled presentation target designed to sit on top of `core::application` and `modules::*` without duplicating core business logic.

### B. Application Core (`core/`)
- **`application`**: Manages initialization, single-host configuration loading, first-run detection, session establishment, and configuration reset.
- **`configuration`**: Handles local persistent storage of host binding parameters (`host_alias`, `host_address`, `username`, `port`, `identity_file`, `is_configured`).
- **`connection_manager`**: Establishes `active_session`, executes background SSH commands, and handles interactive SSH TTY passthrough.
- **`key_manager`**: Automates `ed25519` key pair generation (`ssh-keygen`) and public key deployment (`ssh-copy-id`). Private keys remain strictly external local credentials.
- **`ssh_config`**: Interoperability parser and writer for OpenSSH configuration files (`~/.ssh/config`).
- **`system_operations`**: Maps Windows UNC paths (`\\<IP>\<share>`) and triggers native file explorer shortcuts.

### C. Feature Modules (`modules/`)
- **`file_manager`**: Remote directory browsing (`ls -la`), file uploads/downloads, directory creation/deletion, and Samba share shortcuts.
- **`process_manager`**: Inspection of running Docker containers (`docker ps -a`), systemd services (`systemctl`), log retrieval (`docker logs`), and container action controls (`start`, `stop`, `restart`).
- **`resource_monitor`**: Remote CPU/RAM utilization (`ps aux`, `free -h`), disk partition usage (`df -h`), and user storage quotas (`quota -u`).

---

## 3. Security Boundaries & Credential Isolation

1. **Private Key Isolation**: Private SSH keys (`id_ed25519_<host>`) are stored as external local files in the user's protected `.ssh` directory (`~/.ssh/`) with restricted permissions (`0600`). Private keys are **never** embedded in binary files, logged, or exposed in UI.
2. **Local vs Remote Data Separation**: Resetting client configuration explicitly removes the local binding file (`smartssh_config.cfg`) and resets local app state without modifying or deleting remote Raspberry Pi user data.
3. **Passwordless Authentication**: Passwordless SSH key pairs eliminate plain-text password storage in memory or configuration files.

---

## 4. Testing & Verification Architecture

The project integrates GoogleTest (`smart-ssh-tests`) with an isolated **SSH Stub Environment (`tests::ssh_stub`)**:
- **Deterministic Unit Testing**: Executes 100% offline in <200ms using mock responses for `df -h`, `docker ps`, `quota`, and `ls -la`.
- **Configuration Tests (`SmartSSHConfigurationTest`)**: Verifies save, load, and reset cycles for single-host configuration files without network side effects.
