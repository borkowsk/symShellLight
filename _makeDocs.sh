#!/bin/sh
#(whereis rofi | grep "/rofi") && "OK, doxywizard available"
#(whereis doxygen | grep "/doxygen") && echo "OK, doxygen installed"
#(whereis doxywizard | grep "/doxywizard") && echo "OK, doxywizard installed"

#!/bin/sh

# Funkcja sprawdzająca dostępność komponentu
check_dependency() {
    if ! whereis "$1" | grep -q "/$1"; then
        echo "BŁĄD: Komponent '$1' nie jest zainstalowany. Przerywam."
        exit 1
    fi
    echo "OK, $1 zainstalowany"
}

# 1. Sprawdzenie zależności
check_dependency "doxygen"
check_dependency "doxywizard"
check_dependency "rofi"

# 2. Zapytanie użytkownika o język
#echo "Wybierz język / Choose language:"
#echo "1) Polski"
#echo "2) English"
#read -p "Wybór (1/2): " choice

# 2. Wybór języka za pomocą rofi
# Tworzymy listę opcji oddzielonych nową linią
OPTIONS="Polski\nEnglish"

# Wyświetlamy menu rofi i przechwytujemy wybór
choice=$(echo "$OPTIONS" | rofi -dmenu -p "Język/language:" -i \
    -theme-str 'window { width: 300px; border: 2px; border-radius: 15px; border-color: #444444; }' \
    -theme-str 'listview { lines: 2; scrollbar: false; }' \
    -theme-str 'element { border-radius: 10px; }')

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

# 3. Uruchomienie Doxygen
if [ -f "$FILE" ]; then
    echo "\nRUNNING Doxygen ($FILE):\n"
    doxywizard "$FILE"
else
    echo "BŁĄD: Plik $FILE nie istnieje w bieżącym katalogu!"
    exit 1
fi

#run Doxygen directly
#doxygen  $FILE
