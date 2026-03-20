#!/bin/bash
#skrypt, który łączy wykrywanie systemu, sprawdzanie obecności bibliotek X11 oraz interaktywną ich instalację w razie potrzeby
#Zrobione w całości przez Gemini

# --- 1. Wykrywanie systemu i konfiguracja nazw deweloperskich ---
if [ -f /etc/debian_version ]; then
    OS_TYPE="Debian-based"
    PKG_MANAGER="apt"
    CHECK_CMD="dpkg -s"
    # Pakiety deweloperskie dla Debiana
    XLIB_PKG="libx11-dev"
    XPM_PKG="libxpm-dev"
elif [ -f /etc/redhat-release ]; then
    OS_TYPE="RedHat-based"
    PKG_MANAGER="dnf"
    CHECK_CMD="rpm -q"
    # Pakiety deweloperskie dla RedHat/Fedora
    XLIB_PKG="libX11-devel"
    XPM_PKG="libXpm-devel"
else
    echo "Błąd: Nieobsługiwany system operacyjny."
    exit 1
fi

echo "Wykryto system: $OS_TYPE"
echo "Tryb: Sprawdzanie pakietów deweloperskich (-dev/-devel)"
echo "------------------------------------------"

# --- 2. Funkcja sprawdzająca i instalująca ---
manage_package() {
    local pkg=$1
    echo -n "Sprawdzanie $pkg... "

    if $CHECK_CMD "$pkg" &> /dev/null; then
        echo "[ZAINSTALOWANY]"
    else
        echo "[BRAK]"
        read -p "Czy chcesz zainstalować pakiet deweloperski $pkg? [y/N]: " choice
        case "$choice" in
            [yY][eE][sS]|[yY])
                echo "Aktualizacja list pakietów i instalacja $pkg..."
                # Dodano 'sudo apt update' dla Debiana, by uniknąć błędów 404
                [[ "$PKG_MANAGER" == "apt" ]] && sudo apt update
                sudo $PKG_MANAGER install -y "$pkg"
                ;;
            *)
                echo "Pominięto instalację $pkg."
                ;;
        esac
    fi
}

# --- 3. Wykonanie dla bibliotek deweloperskich ---
manage_package "$XLIB_PKG"
manage_package "$XPM_PKG"

echo "------------------------------------------"
echo "Zakończono sprawdzanie."
