#!/bin/bash
#skrypt, który łączy wykrywanie systemu, sprawdzanie obecności bibliotek X11 oraz interaktywną ich instalację w razie potrzeby
#Zrobione w całości przez Gemini

# --- 1. Wykrywanie systemu i konfiguracja zmiennych ---
if [ -f /etc/debian_version ]; then
    OS_TYPE="Debian-based"
    PKG_MANAGER="apt"
    CHECK_CMD="dpkg -s"
    # Nazwy pakietów dla Debiana/Ubuntu
    XLIB_PKG="libx11-6"
    XPM_PKG="libxpm4"
elif [ -f /etc/redhat-release ]; then
    OS_TYPE="RedHat-based"
    PKG_MANAGER="dnf" # dnf jest standardem w nowszych systemach, zastąpił yum
    CHECK_CMD="rpm -q"
    # Nazwy pakietów dla RHEL/CentOS/Fedora
    XLIB_PKG="libX11"
    XPM_PKG="libXpm"
else
    echo "Błąd: Nieobsługiwany system operacyjny."
    exit 1
fi

echo "System: $OS_TYPE"
echo "Menedżer pakietów: $PKG_MANAGER"
echo "------------------------------------------"

# --- 2. Funkcja sprawdzająca i instalująca ---
manage_package() {
    local pkg=$1
    echo -n "Sprawdzanie $pkg... "

    if $CHECK_CMD "$pkg" &> /dev/null; then
        echo "[ZAINSTALOWANY]"
    else
        echo "[BRAK]"
        read -p "Czy chcesz zainstalować pakiet $pkg? [y/N]: " choice
        case "$choice" in
            [yY][eE][sS]|[yY])
                echo "Próba instalacji $pkg..."
                sudo $PKG_MANAGER install -y "$pkg"
                ;;
            *)
                echo "Pominięto instalację $pkg."
                ;;
        esac
    fi
}

# --- 3. Wykonanie dla konkretnych bibliotek ---
manage_package "$XLIB_PKG"
manage_package "$XPM_PKG"

echo "------------------------------------------"
echo "Zakończono sprawdzanie bibliotek."
