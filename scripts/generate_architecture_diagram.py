import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, ArrowStyle

def create_architecture_diagram():
    # Crisp 16:9 engineering canvas, NO title at all
    fig, ax = plt.subplots(figsize=(19, 10.5), dpi=300)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis('off')
    fig.patch.set_facecolor('#FFFFFF')

    # Color definitions - strict, publication-grade academic palette
    c_ue_border = '#1565C0'
    c_ue_bg = '#F8FAFC'
    c_ue_header = '#E3F2FD'
    
    c_net_border = '#6A1B9A'
    c_net_bg = '#FAF5FB'
    c_net_header = '#F3E5F5'
    
    c_rpi_border = '#1B5E20'
    c_rpi_bg = '#F6FAF6'
    c_rpi_header = '#E8F5E9'
    
    c_box_bg = '#FFFFFF'
    c_text_main = '#1A202C'
    c_text_muted = '#4A5568'
    c_accent_ssh = '#0D47A1'
    c_accent_smb = '#C2185B'
    c_accent_zt = '#4A148C'

    def draw_group_box(x, y, w, h, border_color, bg_color, header_color, title_text, subtitle_text=""):
        p_bg = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.2,rounding_size=1.2",
                              facecolor=bg_color, edgecolor=border_color, linewidth=2.0, zorder=1)
        ax.add_patch(p_bg)
        
        header_h = 5.2
        p_hdr = FancyBboxPatch((x, y + h - header_h), w, header_h,
                               boxstyle="round,pad=0.2,rounding_size=1.2",
                               facecolor=header_color, edgecolor=border_color, linewidth=1.5, zorder=2)
        ax.add_patch(p_hdr)
        
        ax.text(x + w/2, y + h - header_h/2 + (0.5 if subtitle_text else 0), title_text,
                ha='center', va='center', fontsize=12.5, fontweight='bold', color=c_text_main, zorder=3)
        if subtitle_text:
            ax.text(x + w/2, y + h - header_h/2 - 1.3, subtitle_text,
                    ha='center', va='center', fontsize=8.8, color=c_text_muted, zorder=3)

    def draw_inner_box(x, y, w, h, title, lines, header_color='#ECEFF1', border_color='#90A4AE', title_color=c_text_main):
        p = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.1,rounding_size=0.8",
                           facecolor=c_box_bg, edgecolor=border_color, linewidth=1.3, zorder=3)
        ax.add_patch(p)
        
        hdr_h = 3.5
        p_hdr = FancyBboxPatch((x, y + h - hdr_h), w, hdr_h, boxstyle="round,pad=0.1,rounding_size=0.8",
                               facecolor=header_color, edgecolor=border_color, linewidth=1.0, zorder=4)
        ax.add_patch(p_hdr)
        ax.text(x + w/2, y + h - hdr_h/2, title, ha='center', va='center',
                fontsize=9.8, fontweight='bold', color=title_color, zorder=5)
        
        start_y = y + h - hdr_h - 1.8
        line_spacing = (h - hdr_h - 2.6) / max(len(lines), 1)
        for i, line in enumerate(lines):
            # Check for section headers inside the block
            is_subhdr = line.startswith('[') and line.endswith(']')
            font_weight = 'bold' if is_subhdr else 'normal'
            line_color = '#0F172A' if is_subhdr else '#334155'
            ax.text(x + 1.2, start_y - i * line_spacing, line,
                    ha='left', va='center', fontsize=8.2, fontweight=font_weight,
                    color=line_color, zorder=5, family='monospace')

    # ==========================================================
    # 3 COLUMNS: LEFT (UE: 2-30), MID (NET: 33-67), RIGHT (RPI: 70-98)
    # 3 TIERS:
    #   Tier 1 (Top):    y=82 to 91 (h=9)   - Virtual Network Adapters & Mesh
    #   Tier 2 (Middle): y=44 to 78 (h=34)  - SSH Core, Session, Terminal & sshd
    #   Tier 3 (Bottom): y=4  to 40 (h=36)  - Storage, SMB Shares, Quotas & Services
    # ==========================================================

    # 1. LEFT CONTAINER: User Equipment (UE)
    draw_group_box(2, 2, 28, 96, c_ue_border, c_ue_bg, c_ue_header,
                   "User Equipment (UE)", "Workstation / Laptop / Android Smartphone")

    # Tier 1 Left: Virtual Adapter
    draw_inner_box(4, 82, 24, 9, "Virtual Network Adapter", [
        "[ZeroTier Virtual Interface]",
        "Adapter: zt0 (TAP/TUN Driver)",
        "Assigned IP: 10.147.19.x /24"
    ], header_color='#E1F5FE', border_color='#0288D1', title_color='#01579B')

    # Tier 2 Left: SmartSSH Core & OpenSSH
    draw_inner_box(4, 44, 24, 34, "SmartSSH Client & SSH Core", [
        "[Presentation Layer]",
        " * Interactive CLI (ANSI / raw termios)",
        " * Jetpack Compose Mobile GUI (Android)",
        "",
        "[Application Core (C++20 / Kotlin)]",
        " * Single-Host Binding Model Controller",
        " * Session & Command Execution Manager",
        " * Ed25519 Asymmetric Key Manager",
        " * Local Config: smartssh_config.cfg",
        " * Protected Keys: ~/.ssh/id_ed25519_*",
        "",
        "[SSH Protocol Client Driver]",
        " * OpenSSH Engine / sshj / jsch",
        " * Interactive TTY & non-blocking exec",
        " * SFTP File Transfer Subsystem"
    ], header_color='#BBDEFB', border_color='#1976D2', title_color='#0D47A1')

    # Tier 3 Left: Storage & System Subsystems
    draw_inner_box(4, 4, 24, 36, "Storage Client & Monitoring", [
        "[SMB / CIFS Client Engine]",
        " * Windows UNC Path Mapper (\\\\IP\\share)",
        " * Native SMB Client Driver (smbj)",
        " * File upload & download worker",
        " * Remote file creation & deletion",
        "",
        "[Telemetry & Resource Parsers]",
        " * CPU / RAM load parser (ps aux, free)",
        " * Disk partition usage parser (df -h)",
        " * POSIX storage quota parser (quota -u)",
        " * Docker container inspector",
        "",
        "[ZeroTier One Client Service]",
        " * P2P Layer-2 virtual Ethernet daemon"
    ], header_color='#CFD8DC', border_color='#78909C', title_color='#37474F')


    # 2. MIDDLE CONTAINER: Network Transport
    draw_group_box(33, 2, 34, 96, c_net_border, c_net_bg, c_net_header,
                   "Secure Network Transport Layer", "ZeroTier P2P Overlay Mesh & Encrypted Transport Streams")

    # Tier 1 Middle: ZeroTier Mesh Specification
    draw_inner_box(35, 82, 30, 9, "ZeroTier P2P Virtual Mesh Layer", [
        "[Carrier Layer: UDP / STUN NAT Traversal]",
        "Key Exchange: Curve25519 (ECDH)",
        "Payload Cipher: ChaCha20-Poly1305 AEAD"
    ], header_color='#E1BEE7', border_color='#8E24AA', title_color='#4A148C')

    # Tier 2 Middle: SSH Tunnel Channel
    draw_inner_box(35, 44, 30, 34, "SSH Tunnel Channel (Port 22/TCP)", [
        "[Protocol Specification: SSHv2]",
        "Transport: TCP stream over zt0 (IP:22)",
        "Handshake: Curve25519-SHA256 Key Exchange",
        "Server Fingerprint: Ed25519 Host Key",
        "Client Authentication: Ed25519 Public Key",
        "",
        "[Transmitted Operations]",
        " -> Interactive Shell Sessions (bash / TTY)",
        " -> Background Command Execution (popen)",
        " -> Encrypted SFTP File Transfers",
        " -> Administrative Remote User Creation",
        "",
        "[Concurrency]",
        " * Multi-client connections on port 22"
    ], header_color='#E3F2FD', border_color=c_accent_ssh, title_color=c_accent_ssh)

    # Tier 3 Middle: Samba SMB Channel
    draw_inner_box(35, 4, 30, 36, "Samba SMB Channel (Port 445/TCP)", [
        "[Protocol Specification: SMB 3.1.1 / CIFS]",
        "Transport: TCP stream over zt0 (IP:445)",
        "Security Mode: SMB Encrypted Transport",
        "Authentication: Multi-user NTLMv2 / POSIX",
        "",
        "[Exposed Network Shares]",
        " [1] \\\\<IP>\\<username> (Private Storage)",
        "     * Target: /home/<username>",
        "     * Permissions: 0600 (file) / 0700 (dir)",
        "",
        " [2] \\\\<IP>\\shared (Team Storage)",
        "     * Target: /home/minow/shared",
        "     * Permissions: 0664 (file) / 0775 (dir)",
        "     * Authorized Group: ssh-users"
    ], header_color='#FCE4EC', border_color=c_accent_smb, title_color=c_accent_smb)


    # 3. RIGHT CONTAINER: Raspberry Pi 5
    draw_group_box(70, 2, 28, 96, c_rpi_border, c_rpi_bg, c_rpi_header,
                   "Raspberry Pi 5 Server Platform", "Raspberry Pi OS (64-bit Debian Linux)")

    # Tier 1 Right: Server Network Interface
    draw_inner_box(72, 82, 24, 9, "Server Network Interface", [
        "[ZeroTier Virtual Node Daemon]",
        "Adapter: zt0 (Virtual TAP/TUN Driver)",
        "Assigned IP: 10.147.19.100 /24"
    ], header_color='#C8E6C9', border_color='#43A047', title_color='#1B5E20')

    # Tier 2 Right: OpenSSH Server (sshd)
    draw_inner_box(72, 44, 24, 34, "OpenSSH Server Daemon (sshd)", [
        "[Listening Socket]",
        " * Address: 0.0.0.0:22 (Static Port 22)",
        "",
        "[Concurrency & Process Isolation]",
        " * accept() -> fork() child per session",
        " * Dedicated memory & user namespace",
        " * Sandboxed privilege separation",
        "",
        "[Access & Authentication Control]",
        " * PubkeyAuthentication: yes (forced)",
        " * PasswordAuthentication: no (hardened)",
        " * AuthorizedKeys: ~/.ssh/authorized_keys",
        " * Execution Shell: /bin/bash"
    ], header_color='#E8F5E9', border_color='#388E3C', title_color='#1B5E20')

    # Tier 3 Right: Storage & Daemon Services
    draw_inner_box(72, 4, 24, 36, "Storage, Docker & Host Services", [
        "[Samba 4 File Server (smbd)]",
        " * Listening Socket: Port 445/TCP",
        " * Multi-client thread & process pool",
        " * Linux kernel oplocks & file locking",
        "",
        "[System & Process Supervision]",
        " * Docker Runtime: microservices",
        " * systemd: sshd, smbd, zerotier-one",
        " * POSIX Quotas: quota -u <username>",
        "",
        "[User Provisioning Subsystem]",
        " * Script: /home/minow/create_user.sh",
        " * Automates group, ACLs, and .ssh keys"
    ], header_color='#E0F2F1', border_color='#00897B', title_color='#004D40')


    # ==========================================================
    # CLEAN HORIZONTAL CONNECTORS ACROSS TIERS
    # ==========================================================
    def draw_direct_arrow(x1, y, x2, color, label=""):
        arrow_style = ArrowStyle("->", head_length=5, head_width=3.2)
        patch = patches.FancyArrowPatch((x1, y), (x2, y),
                                        arrowstyle=arrow_style,
                                        color=color, linewidth=2.0, zorder=6)
        ax.add_patch(patch)
        if label:
            mid_x = (x1 + x2) / 2
            ax.text(mid_x, y, label, ha='center', va='center',
                    fontsize=8.2, fontweight='bold', color=color, zorder=7,
                    bbox=dict(boxstyle="round,pad=0.25", facecolor="#FFFFFF", edgecolor=color, linewidth=1.2, alpha=1.0))

    # Tier 1 Connectors (ZeroTier Mesh Bridge, y=86.5)
    draw_direct_arrow(28, 86.5, 35, c_accent_zt)
    draw_direct_arrow(65, 86.5, 72, c_accent_zt)

    # Tier 2 Connectors (SSH Stream, y=61.0)
    draw_direct_arrow(28, 61.0, 35, c_accent_ssh, "Port 22")
    draw_direct_arrow(65, 61.0, 72, c_accent_ssh, "TCP/22")

    # Tier 3 Connectors (Samba SMB Stream, y=22.0)
    draw_direct_arrow(28, 22.0, 35, c_accent_smb, "Port 445")
    draw_direct_arrow(65, 22.0, 72, c_accent_smb, "TCP/445")

    # Save to outputs
    plt.tight_layout()
    svg_path = 'docs/images/smartssh_architecture_diagram.svg'
    png_path = 'docs/images/smartssh_architecture_diagram.jpg'
    
    plt.savefig(svg_path, format='svg', bbox_inches='tight', pad_inches=0.05)
    plt.savefig(png_path, format='jpg', dpi=300, bbox_inches='tight', pad_inches=0.05)
    plt.close()
    print("Successfully generated clean engineering schematic to:", svg_path, "and", png_path)

if __name__ == '__main__':
    create_architecture_diagram()
