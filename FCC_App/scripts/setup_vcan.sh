#!/bin/bash
# =============================================================================
# setup_vcan.sh — מגדיר Virtual CAN interface על WSL / Linux
# מריץ פעם אחת לפני הרצת fcc_app או ISM tests
# =============================================================================

set -e

echo "=== Setting up virtual CAN (vcan0) ==="

# טעינת מודול vcan
sudo modprobe vcan

# יצירת הממשק
sudo ip link add dev vcan0 type vcan 2>/dev/null || echo "vcan0 already exists"

# הפעלה
sudo ip link set up vcan0

echo "vcan0 is UP"
ip link show vcan0
