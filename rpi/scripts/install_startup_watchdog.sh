#!/bin/bash
set -e

if [ "$EUID" -ne 0 ]; then
    echo "[-] ERROR: This script must be run as root (use sudo)." >&2
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "[i] Installing SmartSSH Startup Watchdog Service..."

# 1. Install watchdog script to /usr/local/bin
cp "$SCRIPT_DIR/check_services_startup.sh" /usr/local/bin/check_services_startup.sh
chmod 755 /usr/local/bin/check_services_startup.sh
echo "[+] Copied check_services_startup.sh to /usr/local/bin/."

# Also ensure it is in /home/shared if /home/shared exists
if [ -d "/home/shared" ]; then
    cp "$SCRIPT_DIR/check_services_startup.sh" /home/shared/check_services_startup.sh
    chmod 755 /home/shared/check_services_startup.sh
fi

# 2. Install systemd service unit
SYSTEMD_CONF="/etc/systemd/system/smartssh-startup-healthcheck.service"
if [ -f "$SCRIPT_DIR/../configs/smartssh-startup-healthcheck.service" ]; then
    cp "$SCRIPT_DIR/../configs/smartssh-startup-healthcheck.service" "$SYSTEMD_CONF"
else
    cat > "$SYSTEMD_CONF" <<'EOF'
[Unit]
Description=SmartSSH Startup Services Healthcheck and Self-Healing Watchdog
After=network-online.target multi-user.target smbd.service zerotier-one.service dnsmasq.service
Wants=network-online.target

[Service]
Type=oneshot
RemainAfterExit=yes
ExecStartPre=/bin/sleep 5
ExecStart=/usr/local/bin/check_services_startup.sh
StandardOutput=journal+console
StandardError=journal+console

[Install]
WantedBy=multi-user.target
EOF
fi

chmod 644 "$SYSTEMD_CONF"
echo "[+] Installed systemd unit to $SYSTEMD_CONF."

# 3. Reload systemd daemon and enable service
systemctl daemon-reload
systemctl enable smartssh-startup-healthcheck.service
echo "[+] Enabled smartssh-startup-healthcheck.service."

# 4. Test run the script
echo "[*] Running diagnostic check now..."
/usr/local/bin/check_services_startup.sh

echo "[+] Startup Watchdog successfully installed and verified!"
