#!/bin/bash
# ==============================================================================
# SmartSSH - Startup Service Verification & Self-Healing Watchdog
# Checks: smb (smbd), zerotier (zerotier-one), and dnsmasq.
# If any service is inactive, attempts up to 3 restarts via systemctl.
# ==============================================================================

set -u

SERVICES=("smbd" "zerotier-one" "dnsmasq")
MAX_RETRIES=3
RETRY_DELAY=2

echo "[$(date '+%Y-%m-%d %H:%M:%S')] [SmartSSH Watchdog] Starting startup service health check..."

for svc in "${SERVICES[@]}"; do
    echo "[*] Checking status of service: $svc..."
    if systemctl is-active --quiet "$svc"; then
        echo "[+] Service '$svc' is ACTIVE (OK)."
    else
        echo "[-] WARNING: Service '$svc' is NOT active! Initiating up to $MAX_RETRIES restart attempts..."
        recovered=0
        for attempt in $(seq 1 $MAX_RETRIES); do
            echo "[!] Attempt $attempt of $MAX_RETRIES: systemctl restart $svc..."
            systemctl restart "$svc" || true
            sleep "$RETRY_DELAY"
            if systemctl is-active --quiet "$svc"; then
                echo "[+] Service '$svc' RECOVERED successfully on attempt $attempt and is now ACTIVE."
                recovered=1
                break
            fi
        done
        if [ "$recovered" -eq 0 ]; then
            echo "[-] ERROR: Service '$svc' failed to recover after $MAX_RETRIES restart attempts!" >&2
        fi
    fi
done

echo "[$(date '+%Y-%m-%d %H:%M:%S')] [SmartSSH Watchdog] Startup service health check completed."
