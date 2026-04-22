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

echo -e $COLOR2"Detected system kind: $OS_TYPE"$COLOR1
echo "------------------------------------"

# Sprawdzanie Xlib
if check_package "$XLIB_PKG"; then
    echo -e $COLOR2"[OK] Xlib ($XLIB_PKG) is instaled."$NORMCO
else
    echo -e $COLERR"[FAIL] Xlib ($XLIB_PKG) is NOT instaled!"$NORMCO
    exit 2
fi

# Sprawdzanie Xpm
if check_package "$XPM_PKG"; then
    echo  -e $COLOR2"[OK] Xpm ($XPM_PKG) is instaled."$NORMCO
else
    echo -e $COLERR"[FAIL] Xpm ($XPM_PKG) is NOT instaled!."$NORMCO
    exit 3
fi

