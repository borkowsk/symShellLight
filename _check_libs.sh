#!/bin/bash
#Uniwersalny skrypt Bash, który rozpoznaje system i sprawdza obecność bibliotek Xlib oraz Xpm
#Napisany w całości przez Gemini.

# Funkcja sprawdzająca status pakietu
check_package() {
    local pkg=$1
    if [ -f /etc/debian_version ]; then
        dpkg -s "$pkg" &> /dev/null
        return $?
    elif [ -f /etc/redhat-release ]; then
        rpm -q "$pkg" &> /dev/null
        return $?
    fi
    return 1
}

# Definicja nazw pakietów dla różnych systemów
if [ -f /etc/debian_version ]; then
    OS_TYPE="Debian-based"
    # Pakiety deweloperskie dla Debiana
    XLIB_PKG="libx11-dev"
    XPM_PKG="libxpm-dev"
elif [ -f /etc/redhat-release ]; then
    OS_TYPE="RedHat-based"
    # Pakiety deweloperskie dla RedHat/Fedora
    XLIB_PKG="libX11-devel"
    XPM_PKG="libXpm-devel"
else
    echo "Nieobsługiwany system operacyjny."
    exit 1
fi

echo "Wykryto system: $OS_TYPE"
echo "--------------------------"

# Sprawdzanie Xlib
if check_package "$XLIB_PKG"; then
    echo "[OK] Xlib ($XLIB_PKG) jest zainstalowana."
else
    echo "[FAIL] Xlib ($XLIB_PKG) NIE jest zainstalowana."
fi

# Sprawdzanie Xpm
if check_package "$XPM_PKG"; then
    echo "[OK] Xpm ($XPM_PKG) jest zainstalowana."
else
    echo "[FAIL] Xpm ($XPM_PKG) NIE jest zainstalowana."
fi
