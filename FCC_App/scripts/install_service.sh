#!/bin/bash
# =============================================================================
# install_service.sh — מתקין את fcc_app כ-systemd service קבוע על ה-Jetson
# כך שהוא רץ תמיד ברקע (עולה עם ה-boot, קם אוטומטית אם קורס) בלי תלות
# בחיבור PuTTY/SSH פעיל ובלי צורך ב-screen -S fcc.
#
# הרצה חד-פעמית, מתוך תיקיית FCC_App על ה-Jetson:
#   cd FCC_App/scripts
#   sudo ./install_service.sh
#
# לצפייה בלוג החי אחר כך (מחליף את הצפייה ב-screen):
#   journalctl -u fcc_app -f
# =============================================================================

set -e

FCC_APP_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SERVICE_SRC="$FCC_APP_DIR/scripts/fcc_app.service"
SERVICE_DST="/etc/systemd/system/fcc_app.service"

if [ ! -f "$FCC_APP_DIR/build/fcc_app" ]; then
    echo "ERROR: $FCC_APP_DIR/build/fcc_app not found — build it first (cmake .. && make -j\$(nproc) inside build/)."
    exit 1
fi

echo "=== Installing fcc_app.service (FCC_APP_DIR=$FCC_APP_DIR) ==="
sed "s|__FCC_APP_DIR__|$FCC_APP_DIR|g" "$SERVICE_SRC" > "$SERVICE_DST"

systemctl daemon-reload
systemctl enable fcc_app
systemctl restart fcc_app

echo ""
echo "Done. Status:"
systemctl status fcc_app --no-pager
echo ""
echo "Live log:   journalctl -u fcc_app -f"
echo "Stop:       sudo systemctl stop fcc_app"
echo "Disable:    sudo systemctl disable fcc_app"
