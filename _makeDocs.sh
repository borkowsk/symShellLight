#!/bin/sh
#      DOXUMENTATION "MANAGER"
# =================================
# @date 2026-04-29 (modification)
#
# Instalation of prerequisities under Ubuntu:
# ===========================================
# sudo apt install rofi
# (whereis rofi | grep "/rofi") && "OK, doxywizard available"
# sudo apt install doxygen
# (whereis doxygen | grep "/doxygen") && echo "OK, doxygen installed"
# sudo apt install doxygen-gui
# (whereis doxywizard | grep "/doxywizard") && echo "OK, doxywizard installed"

# REALNY KOD PONIŻEJ:
# ===================

case "$OSTYPE" in
  solaris*) echo "SOLARIS" ;;
  darwin*)  echo "macOS (Unix)" ;; 
  linux*)   echo "LINUX (Unix)" ;;
  bsd*)     echo "BSD (Unix)" ;;
  msys*)    echo "Git Bash / MINGW (Windows)" ;;
  cygwin*)  echo "Cygwin (Windows)" ;;
  *)        echo "Nieznany system: $OSTYPE" ;;
esac

# Funkcja sprawdzająca dostępność komponentu
check_dependency() {
    if ! whereis "$1" | grep -q "/$1"; then
        echo "ERROR: '$1' is NOT installed! Sorry."
        exit 1
    fi
    #echo "OK, $1 is instaled."
}

# 1. Sprawdzenie zależności
if [[ "$OSTYPE" != "msys"* ]]; then
    #echo "To nie jest Git Bash. Wykonuję operacje dla systemów Unix/Linux..."
    # Tutaj umieść kod dla systemów innych niż Windows (Git Bash)
	check_dependency "doxywizard"
	check_dependency "doxygen"
	check_dependency "rofi"
else
    echo -e "Wykryto Git Bash na Windows.\nUpewnij się że doxywizzard jest zainstalowany\n".
fi

# 2. Wybór języka za pomocą rofi
# Tworzymy listę opcji oddzielonych nową linią
OPTIONS="Polski\nEnglish"

# Wyświetlamy menu rofi i przechwytujemy wybór
choice=$(echo "$OPTIONS" | rofi -dmenu -p "Język/language:" -i \
    -theme-str 'window { width: 300px; border: 2px; border-radius: 15px; border-color: #444444; }' \
    -theme-str 'listview { lines: 2; scrollbar: false; }' \
    -theme-str 'element { border-radius: 10px; }')
	
if [ $? -ne 0 ]; then
    echo "Wywołanie rofi zakończyło się niepowodzeniem."
    # 2. Zapytanie użytkownika o język
	echo "Wybierz język / Choose language:"
	echo "1) Polski"
	echo "2) English"
	read -p "Wybór (1/2): " choice
fi	

case $choice in
    1)
        FILE="Doxyfile"
        echo "Wybrano język polski."
        ;;
    2)
        FILE="DoxyfileEN"
        echo "English language selected."
        ;;
"Polski")
        FILE="Doxyfile"
        echo "Wybrano język polski."
        ;;
"English")
        FILE="DoxyfileEN"
        echo "English language selected."
        ;;        
    *)
        echo "Nieprawidłowy wybór. Przerywam."
        exit 1
        ;;
esac

# 3. Uruchomienie Doxygen-a
if [ -f "$FILE" ]; then
    echo -e "\nSTARTING Doxygen Wizard ($FILE):\n"
    doxywizard "$FILE" &
    exit 0
else
    echo "BŁĄD: Plik $FILE nie istnieje w bieżącym katalogu!"
    exit 1
fi

#run Doxygen directly
#doxygen  $FILE

