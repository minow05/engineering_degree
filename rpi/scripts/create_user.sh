#!/bin/bash
# SmartSSH - Server-side User Provisioning Script for Raspberry Pi
# Usage: sudo ./create_user.sh <username> [public_key_string] [samba_password]

set -e

if [ "$EUID" -ne 0 ]; then
    echo "[-] ERROR: This script must be run as root (use sudo)." >&2
    exit 1
fi

NEW_USER="$1"
PUB_KEY="$2"
USER_PASS="$3"

if [ -z "$NEW_USER" ]; then
    echo "Usage: sudo $0 <username> [public_key_string] [samba_password]" >&2
    exit 1
fi

echo "[i] Provisioning user '$NEW_USER' on Raspberry Pi..."

# 1. Create Linux user account and home directory if not exists
if id -u "$NEW_USER" >/dev/null 2>&1; then
    echo "[i] User '$NEW_USER' already exists in Linux."
else
    useradd -m -s /bin/bash "$NEW_USER"
    echo "[+] Linux user '$NEW_USER' created with home /home/$NEW_USER."
fi

# Set password if provided
if [ -n "$USER_PASS" ]; then
    echo "$NEW_USER:$USER_PASS" | chpasswd
    echo "[+] Password set for Linux user '$NEW_USER'."
fi

# 2. Strict POSIX data isolation for user's home
chmod 700 "/home/$NEW_USER"
chown "$NEW_USER:$NEW_USER" "/home/$NEW_USER"

# 3. Setup SSH key authentication if public key provided
if [ -n "$PUB_KEY" ]; then
    SSH_DIR="/home/$NEW_USER/.ssh"
    mkdir -p "$SSH_DIR"
    chmod 700 "$SSH_DIR"
    echo "$PUB_KEY" >> "$SSH_DIR/authorized_keys"
    sort -u -o "$SSH_DIR/authorized_keys" "$SSH_DIR/authorized_keys"
    chmod 600 "$SSH_DIR/authorized_keys"
    chown -R "$NEW_USER:$NEW_USER" "$SSH_DIR"
    echo "[+] SSH public key installed for '$NEW_USER'."
fi

# 4. Configure Samba Share and Quotas
SMB_CONF="/etc/samba/smb.conf"
if [ -f "$SMB_CONF" ]; then
    # Add user private share if not already present
    if ! grep -q "^\[$NEW_USER\]" "$SMB_CONF"; then
        cat >> "$SMB_CONF" <<EOF

[$NEW_USER]
   path = /home/$NEW_USER
   browseable = yes
   read only = no
   valid users = $NEW_USER
   force user = $NEW_USER
   create mask = 0777
   directory mask = 0777
EOF
        echo "[+] Added Samba share '[$NEW_USER]' in $SMB_CONF."
    fi

    # Add user to [shared] valid users if not present
    if grep -q "^\[shared\]" "$SMB_CONF"; then
        if ! grep -A 8 "^\[shared\]" "$SMB_CONF" | grep -q "$NEW_USER"; then
            sed -i "/^\[shared\]/,/^\[/ s/valid users = .*/& $NEW_USER/" "$SMB_CONF"
            echo "[+] Added '$NEW_USER' to [shared] Samba share."
        fi
    fi

    # Set Samba password if provided
    if [ -n "$USER_PASS" ]; then
        (echo "$USER_PASS"; echo "$USER_PASS") | smbpasswd -s -a "$NEW_USER"
        smbpasswd -e "$NEW_USER" >/dev/null 2>&1 || true
        echo "[+] Samba password enabled for '$NEW_USER'."
    fi

    # Reload Samba daemon
    systemctl reload smbd 2>/dev/null || systemctl restart smbd 2>/dev/null || true
fi

echo "[+] User '$NEW_USER' successfully provisioned on Raspberry Pi!"
