# System Architecture & Thesis Specification

## Overview
This document outlines the architecture for the Engineering Thesis Project:
**"Projekt i implementacja zdalnego dostępu do prywatnych zasobów plikowych z wykorzystaniem platformy Raspberry Pi"**

## System Components
1. **Client CLI (`smart-ssh`)**:
   - Written in **C++20** using Enterprise target-based CMake structure.
   - Manages `~/.ssh/config` connection host entries.
   - SSH Key Pair Generator (`ed25519` / `rsa`) & deployment (`ssh-copy-id`).
   - Encrypted local Password & Passphrase Vault with `ssh-agent` integration.
   - Remote health & quota commands (`df -h`, user disk space monitoring).
   - Samba Network Storage shortcuts & ZeroTier VPN node probing.

2. **Server (Raspberry Pi)**:
   - Operating System: Debian / Raspbian Linux.
   - User Isolation: Separate home directories `/home/<username>` with disk quotas.
   - Shared Access: Group `/srv/shared` accessible via Samba (`smb.conf`).
   - VPN Access: ZeroTier peer-to-peer connection for access from any network without port forwarding.
