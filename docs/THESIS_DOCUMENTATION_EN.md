# Engineering Thesis Documentation: SmartSSH Remote Resource Access System

**Thesis Title:** *Design and Implementation of Remote File Resource Access System using Raspberry Pi Platform*  
**Academic Degree:** Bachelor of Science in Computer Engineering / Software Engineering  
**Target Platform:** Raspberry Pi Server (Raspberry Pi OS / Debian Linux) & Cross-Platform Client (`smart-ssh`)  
**Programming Language & Standard:** C++20 | CMake | Clang (`clang-cl` / `clang++`) | GoogleTest  
**Document Language:** English  

---

## 1. Executive Summary & System Requirements

The **SmartSSH Remote Resource Access System** is an integrated, client-server platform designed to transform a low-cost Raspberry Pi microcomputer into a secure, enterprise-grade private network storage, process hosting, and system telemetry hub. 

The software architecture consists of two primary operational domains:
1. **Server Host Environment (Raspberry Pi Platform)**: A headless Linux instance running OpenSSH server, ZeroTier P2P mesh VPN, Samba SMB/CIFS network storage daemon, Docker container runtime, systemd service management, and POSIX filesystem quota controllers.
2. **Client Application (`smart-ssh`)**: A multiplatform C++20 application operating under a **Single-Host Binding Model**, providing unified, menu-driven CLI management of remote file resources, network storage shares, Docker/systemd processes, and hardware telemetry metrics over passwordless SSH channels.

### System Scope & Objectives
* **Secure Remote Data Access**: End-to-end encrypted file management and SMB/CIFS network storage sharing over untrusted networks.
* **Single-Host Binding Paradigm**: Eliminates multi-host connection sprawl by binding directly to a primary Raspberry Pi host via a first-run setup wizard and persisting local host state in `smartssh_config.cfg`.
* **Zero-Credential Disk Persistence**: Employs Ed25519 public-key authentication, eliminating plain-text password storage in configuration files or runtime memory.
* **Cross-Platform Execution**: Native execution across Windows, Linux, and macOS platforms from a single unified C++20 codebase.

---

## 2. Server Infrastructure & Host Technologies (Raspberry Pi)

The server infrastructure relies on lightweight, industry-standard Linux networking and storage daemon services hosted on Raspberry Pi OS.

![SmartSSH System Architecture](images/smartssh_architecture_diagram.jpg)

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      Raspberry Pi Host Platform                         │
│                                                                         │
│   ┌──────────────────────┐    ┌─────────────────────────────────────┐   │
│   │   ZeroTier P2P VPN   │    │           OpenSSH Server            │   │
│   │ (ChaCha20-Poly1305)  │    │      (Port 22, ed25519 Auth)        │   │
│   └──────────┬───────────┘    └──────────────────┬──────────────────┘   │
│              │ Virtual `zt0` Adapter             │ TTY / Execution      │
│              ▼                                   ▼                      │
│   ┌─────────────────────────────────────────────────────────────────┐   │
│   │                         Linux Kernel                            │   │
│   │                 (POSIX File System & ACLs)                      │   │
│   └───────┬─────────────────────────┬───────────────────────┬───────┘   │
│           │                         │                       │           │
│           ▼                         ▼                       ▼           │
│  ┌─────────────────┐       ┌─────────────────┐     ┌─────────────────┐  │
│  │ User Isolation  │       │ Samba SMB/CIFS  │     │ Docker Runtime  │  │
│  │ /home/minow     │       │ Encrypted Shares│     │ & systemd Units │  │
│  │ /home/rachel    │       │ /home/*/shared  │     │ Process Control │  │
│  └─────────────────┘       └─────────────────┘     └─────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.1 Operating System & Core Services
* **Raspberry Pi OS (Debian Linux)**: Provides POSIX process isolation, user accounts, systemd service supervision, and hardware interfacing.
* **OpenSSH Daemon (`sshd`)**: Handles encrypted command execution, SFTP file transfer channels, and interactive TTY allocation. Configured with forced public-key authentication (`PubkeyAuthentication yes`) and disabled root SSH logins.

### 2.2 Virtual Private Networking (ZeroTier P2P VPN)
* **Peer-to-Peer Mesh Encryption**: ZeroTier establishes a virtual Layer-2 Ethernet switch over UDP/IP utilizing STUN/TURN NAT traversal.
* **Cryptographic Protocol**: All traffic between the `smart-ssh` client and Raspberry Pi host is encrypted end-to-end using Curve25519 key exchange and ChaCha20-Poly1305 authenticated encryption.
* **Network Isolation**: Assigns a private virtual network interface (`zt0`) with a static IP address (e.g., `10.147.19.x`), bypassing port forwarding and firewall exposure on public IP addresses.

### 2.3 Samba SMB/CIFS Storage Architecture
To provide seamless network drive mounting on Windows and desktop platforms, Samba 4 is configured with strict multi-tenant directory separation:
* **User Directory Isolation**:
  * User `minow`: Private home directory mapped to `/home/minow`.
  * User `rachel`: Private home directory mapped to `/home/rachel`.
* **Administrator Shared Storage**:
  * Shared directory `/home/minow/shared` is co-located in the primary administrator's home path (`minow`), granting complete administrative control, permission management, and audit capability over public/shared files.
* **Samba Configuration (`/etc/samba/smb.conf`) Snippet**:
  ```ini
  [minow]
      path = /home/minow
      valid users = minow
      read only = no
      browsable = yes
      force create mode = 0600
      force directory mode = 0700

  [rachel]
      path = /home/rachel
      valid users = rachel
      read only = no
      browsable = yes
      force create mode = 0600
      force directory mode = 0700

  [shared]
      path = /home/minow/shared
      valid users = minow rachel
      read only = no
      browsable = yes
      force create mode = 0664
      force directory mode = 0775
  ```

### 2.4 Containerization & System Management
* **Docker Container Runtime**: Manages lightweight microservices hosted on the Pi. The client application inspects running containers via `docker ps -a`, retrieves logs with `docker logs`, and executes lifecycle state transitions (`docker start`, `stop`, `restart`).
* **systemd Init Daemon**: Monitors background system services (e.g., `smbd`, `zerotier-one`, `ssh`), enabling client-side status inspection.
* **POSIX Disk Quotas**: User storage allocations are tracked using Linux filesystem quotas (`quota -u <username>`), allowing the client to monitor user disk quotas and enforce storage caps.

### 2.5 Port Architecture, Connection Concurrency & Multi-Client Service Model

A foundational architectural consideration in SmartSSH is the concurrent handling of multiple client connections without requiring dynamic port reservation or dedicated port pooling mechanisms on the server host.

#### 2.5.1 TCP/IP Connection Identification (The 4-Tuple Model)
In standard TCP/IP networking, every individual logical connection is uniquely distinguished by a **4-tuple** in the IP and TCP headers:
$$\{\text{Source IP},\, \text{Source Ephemeral Port},\, \text{Destination Server IP},\, \text{Destination Service Port}\}$$

* **Server Destination Port**: Remote Raspberry Pi services listen on static, standardized ports (OpenSSH on port `22`, Samba SMB on port `445`). The server-side listening socket operates in the `LISTEN` state.
* **Client Source Port**: When a client initiates a connection, the local operating system kernel dynamically assigns an unreserved ephemeral port (typically from the range `49152–65535` according to IANA guidelines).
* **Elimination of Port Collisions**: When two distinct users (e.g., from IP addresses `10.147.19.11` and `10.147.19.12`) or two independent processes on the same host connect simultaneously to port `22`, each socket is uniquely identified in the Linux kernel connection tracking table in the `ESTABLISHED` state. No server-side port contention occurs.

#### 2.5.2 Concurrency Architecture of Server Daemons
1. **OpenSSH Multi-Process Model (`sshd`)**:
   * The master `sshd` daemon runs with administrative privileges and monitors the listening port for incoming `SYN` packets.
   * Upon accepting a connection via `accept()`, `sshd` executes a `fork()` system call, spawning an isolated child process dedicated specifically to that client session.
   * The child process completes cryptographic handshake negotiation, applies privilege separation, and—following successful Ed25519 public-key verification—executes user shells and commands within the security context of the user's POSIX `UID`/`GID`.
   * As a result, user sessions are memory-isolated and process-isolated.

2. **Samba Concurrency Model (`smbd`)**:
   * Similarly, the `smbd` daemon creates distinct processes or worker threads for each active SMB client connection over port `445`.
   * Multi-user file access synchronization is managed directly via Linux kernel advisory/mandatory locks and SMB opportunistic locks (*oplocks*).

#### 2.5.3 Evaluation of Dynamic Port Pools
During system design, the need for dynamic server port allocation or port pooling was evaluated:
* **Port Pooling is Redundant**: Server-side dynamic port allocation is only required in reverse proxy architectures or SSH remote port forwarding (`ssh -R`), where multiple clients attempt to bind identical listening port numbers on the remote host for forwarded services. SmartSSH uses direct client-to-service sessions, rendering server port pooling unnecessary.
* **Scalability**: Standard Linux daemons natively support concurrent connections on static ports `22` and `445`. Practical scalability limits on Raspberry Pi hardware are constrained by RAM capacity and CPU time rather than TCP socket constraints.

---

## 3. Client Software Architecture (`smart-ssh`)

The client application is implemented in clean C++20, utilizing a strictly decoupled multi-layer architecture.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        SmartSSH Client Core                            │
│                                                                        │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │                      Presentation Layer                        │   │
│   │  ┌──────────────────────────────────────────────────────────┐  │   │
│   │  │ CLI (`ui/cli/cli_app.cpp`, `ui/terminal.cpp`)            │  │   │
│   │  │ Non-Canonical ANSI Menu | Instant Keypress | Auto-Flush  │  │   │
│   │  └────────────────────────────┬─────────────────────────────┘  │   │
│   └───────────────────────────────┼────────────────────────────────┘   │
│                                   │                                    │
│   ┌───────────────────────────────▼────────────────────────────────┐   │
│   │                      Application Core                          │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    Application    │ │   Configuration   │ │ ConnectionMgr │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    KeyManager     │ │     SSHConfig     │ │ SystemOps     │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   └───────────────────────────────┬────────────────────────────────┘   │
│                                   │                                    │
│   ┌───────────────────────────────▼────────────────────────────────┐   │
│   │                       Feature Modules                          │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    FileManager    │ │  ProcessManager   │ │ ResourceMon   │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   └────────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────────┘
```

### 3.1 Single-Host Binding Model
Unlike standard multi-host SSH connection utilities, `smart-ssh` enforces a **Single-Host Binding Model**:
1. **First-Run Setup Wizard (`installer::setup_wizard`)**: If `smartssh_config.cfg` is missing or `is_configured = false`, the application enters an interactive setup wizard that asks for the target host IP/address, SSH username, SSH port, identity file path, and storage share aliases.
2. **Automated Key Setup (`core::key_manager`)**: The wizard invokes `ssh-keygen -t ed25519` to generate an asymmetric key pair locally (if none exists) and deploys the public key to the remote Pi via `ssh-copy-id`.
3. **Persistent Binding Configuration (`core::configuration`)**: The host configuration is saved to `smartssh_config.cfg`. All subsequent launches directly load this configuration without prompting the user.
4. **Configuration Reset**: Users can reset application state via **Settings → Reset Configuration**, which deletes the local configuration file and returns the client to setup state without modifying remote Raspberry Pi files.

### 3.2 Presentation Layer (`ui/`)
* **`ui/terminal.cpp`**: Implements non-canonical raw terminal input reading (`read_key_immediate()`) on POSIX (via `termios`) and Windows (via `_getch()`). Implements terminal clearing (`clear_screen()`) and exit key waits (`wait_for_key()`).
* **`ui/menu.cpp`**: Renders interactive terminal menus with ANSI color escape codes and clean menu choices.
* **`ui/cli/cli_app.cpp`**: Controls menu option selection and routes execution to feature modules. Flushes terminal output after each action choice.

### 3.3 Application Core Layer (`core/`)
* **`core::application`**: Main orchestrator managing application startup, first-run wizard detection, main menu event loops, and clean termination.
* **`core::configuration`**: Serializes and deserializes host binding parameters (`host_alias`, `host_address`, `username`, `port`, `identity_file`, `is_configured`).
* **`core::connection_manager`**: Executes remote shell commands over OpenSSH child processes (`exec_command()`) and provides interactive TTY terminal passthrough (`connect_interactive()`).
* **`core::key_manager`**: Automates Ed25519 SSH key pair generation and public key copying.
* **`core::ssh_config`**: Interoperability parser and generator for system OpenSSH configuration files (`~/.ssh/config`).
* **`core::system_operations`**: Maps Windows UNC paths (`\\<IP>\<share>`) and launches native OS file explorers (`explorer.exe`, `xdg-open`, `open`).

### 3.4 Feature Modules (`modules/`)
* **`modules::file_manager`**: Handles remote directory listing (`ls -la`), directory creation (`mkdir`), remote file deletion (`rm -rf`), file upload/download shortcuts (`scp`), and SMB network drive opening.
* **`modules::process_manager`**: Inspects running Docker containers (`docker ps -a`), streams container logs (`docker logs`), triggers container lifecycle controls (`start`, `stop`, `restart`), and checks systemd units.
* **`modules::resource_monitor`**: Gathers hardware metrics including CPU workload (`ps aux`), memory consumption (`free -h`), disk partition utilization (`df -h`), and POSIX disk quota limits (`quota -u`).

---

## 4. Multiplatform Design & Implementation

`smart-ssh` is written in standard C++20 with platform-specific abstraction branches:

### 4.1 Platform Abstractions

| Component | Windows Implementation | Linux / macOS Implementation |
| :--- | :--- | :--- |
| **Instant Key Reading** | `_getch()` from `<conio.h>` | `tcgetattr()` / `tcsetattr()` with `termios` raw mode |
| **Terminal Clearing** | ANSI `\033[2J\033[1;1H` + Windows Console Mode | ANSI escape code `\033[2J\033[1;1H` |
| **Process Execution** | `_popen()` / `_pclose()` via `utils::exec` | `popen()` / `pclose()` via `utils::exec` |
| **Network Storage Manager**| Windows UNC paths (`\\<IP>\<share>`) via `ShellExecuteW` | Native SMB URL schemes (`smb://<IP>/<share>`) via `xdg-open` / `open` |

### 4.2 Portable Codebase Structure
The build system uses CMake target abstractions (`smart_ssh_utils`, `smart_ssh_core`, `smart_ssh_ui`, `smart_ssh_installer`, `smart_ssh_modules`, `smart-ssh`), allowing compilation on Windows MSVC/ClangCL, Linux GCC/Clang, and macOS Apple Clang.

---

## 5. Standalone Single-Binary Release Build Procedures

To produce clean, self-contained Release executables without external debug symbol files (`.pdb` files on Windows), use the following platform-specific build instructions.

### 5.1 Windows Build (Standalone `.exe` without `.pdb`)
Using Visual Studio 2022 Command Prompt or PowerShell with ClangCL / MSVC:

```powershell
# 1. Configure Release build directory with ClangCL toolset
cmake -B build_release -S . -G "Visual Studio 17 2022" -T ClangCL -DCMAKE_BUILD_TYPE=Release

# 2. Compile standalone Release binary
cmake --build build_release --config Release

# 3. Output Binary Location:
# build_release\out\Release\smart-ssh.exe
```

> [!NOTE]
> The resulting executable `smart-ssh.exe` contains all compiled code statically linked against C++ runtime libraries and requires no external `.pdb` or `.dll` files for deployment.

### 5.2 Linux Build (Single Binary)
On Debian / Ubuntu / Raspberry Pi OS:

```bash
# 1. Configure Release build with Clang / GCC
cmake -B build_release -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++

# 2. Build Release binary
cmake --build build_release --config Release

# 3. Strip debug symbols for standalone distribution
strip build_release/out/smart-ssh

# Output Binary Location:
# build_release/out/smart-ssh
```

### 5.3 macOS Build (Single Binary)
On macOS (Intel / Apple Silicon):

```bash
# 1. Configure Release build with Apple Clang
cmake -B build_release -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++

# 2. Build Release binary
cmake --build build_release --config Release

# 3. Strip debug symbols
strip build_release/out/smart-ssh

# Output Binary Location:
# build_release/out/smart-ssh
```

---

## 6. Security & Credential Isolation Model

1. **Ed25519 Asymmetric Keys**: Authentication relies on 256-bit Edwards-curve Digital Signature Algorithm (Ed25519) keys.
2. **Local Credential Storage**: Private keys (`id_ed25519_<host>`) are stored strictly in the client user's protected `.ssh` directory (`~/.ssh/`) with `0600` permissions (or restricted Windows ACLs). Private keys are never embedded in the application binary or transmitted across the network.
3. **Data Separation on Reset**: Reconfiguring or resetting local client state (`smartssh_config.cfg`) deletes only local connection metadata. All remote Raspberry Pi files, Samba shares, and Docker containers remain intact.
4. **Transport Layer Security**: Dual-layer encryption: ZeroTier VPN encrypted tunnel (ChaCha20-Poly1305) + OpenSSH SSHv2 session transport (AES-GCM / ChaCha20-Poly1305).

---

## 7. Verification & Automated Testing

The project incorporates GoogleTest (`gtest`) framework unit and regression tests paired with an **SSH Stub Environment (`tests::ssh_stub`)**:

```
[==========] Running 31 tests from 11 test suites ran. (2023 ms total)
[  PASSED  ] 31 tests.
```

### Test Suite Summary (11 Test Suites)
* **`FunctionalModuleTest` (4 tests)**: Verifies CPU/RAM monitoring, disk formatting, POSIX disk quota parsing, and the full application lifecycle.
* **`SSHStubEnvironmentTest` (5 tests)**: Simulates SSH server responses (stubbing) for disk space, Docker containers, user quotas, and directory file listings.
* **`SmartSSHConfigurationTest` (1 test)**: Verifies saving, loading, and resetting configuration files.
* **`SmartSSHExecTest` (1 test)**: Validates child process command execution.
* **`SmartSSHConfigTest` (1 test)**: Verifies isolated parsing and generation of OpenSSH configuration files.
* **`RegressionTest` (6 tests)**: Validates resilience against corrupted configuration files, non-standard/boundary port numbers, whitespace in identity file paths, consecutive reset operations, and Samba alias-to-IP resolution.
* **`CoreUnitConfigurationTest` (3 tests)**: Verifies default state, saving valid data, and reset cleanup.
* **`CoreUnitSSHConfigTest` (2 tests)**: Verifies adding, updating, and querying SSH configuration entries.
* **`CoreUnitKeyManagerTest` (2 tests)**: Verifies `.ssh` path resolution and local key enumeration.
* **`CoreUnitSystemOperationsTest` (2 tests)**: Verifies command formatting for remote disk and quota checks.
* **`UtilsUnitExecTest` (4 tests)**: Verifies shell command execution, exit codes, and system tool availability.
