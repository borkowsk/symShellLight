## IKONA APLIKACJI
## @date 2026-04-13 (created)

**Aplikacje symulacyjne SymShella teoretycznie mają własną, czarno białą ikonę. Dawniej to działało, ale od którejś wersji gnome przestało. Ta ikona nigdy i nigdzie nie jest widoczna i aplikacja ma jakąś standardową albo w ogóle jakby pustą.**

Jest to klasyczny problem w nowoczesnych środowiskach graficznych Linuxa (szczególnie GNOME). Dawniej ikony ustawiane bezpośrednio przez API X11 (tzw. WM\_HINTS) były respektowane przez menedżery okien. Dzisiaj GNOME polega niemal wyłącznie na systemie **Desktop Entry** i standardzie **StartupWMClass**.

Jeśli stara aplikacja "straciła" ikonę, prawdopodobnie system nie wie, jak powiązać uruchomiony proces z plikiem graficznym na dysku. Oto jak możesz to naprawić, wykorzystując Fluttera jako "zarządcę" lub po prostu naprawiając strukturę aplikacji:

### **1\. Rozwiązanie "Naprawcze" (StartupWMClass)**

Nawet jeśli Twoja aplikacja w C++ ma wewnątrz zakodowaną ikonę, GNOME jej nie wyświetli, jeśli nie znajdzie pasującego pliku .desktop.

Musisz stworzyć plik /usr/share/applications/symulator.desktop (lub w folderze lokalnym):

```Ini, TOML  
\[Desktop Entry\]  
Name=Mój Symulator  
Exec=/sciezka/do/binarki\_cpp  
Icon=/sciezka/do/ikony.png  
Type=Application  
StartupWMClass=moja-binarka-cpp
```

**Kluczowy moment:** Musisz upewnić się, że Twoja aplikacja w C++ ustawia klasę okna (WM\_CLASS) na taką samą wartość, jaka jest w StartupWMClass. W czystym X11 robi się to tak:

```C++  
XClassHint \*class\_hint \= XAllocClassHint();  
class\_hint-\>res\_name \= (char\*)"moja-binarka-cpp";  
class\_hint-\>res\_class \= (char\*)"MojaBinarkaCpp";  
XSetClassHint(display, window, class\_hint);  
XFree(class\_hint);
```

Bez tego GNOME widzi "nieznane okno" i przypisuje mu domyślną ikonę (lub pusty kwadrat).

---

### **2\. Rozwiązanie "Flutterowe" (Eleganckie)**

Skoro i tak planujesz użyć Fluttera do menu, możesz sprawić, by to **Flutter był głównym punktem wejścia (entry point)** aplikacji.

1. Użytkownik klika ikonę "Symulator" (która w rzeczywistości uruchamia Twoją apkę we Flutterze).  
2. Flutter ma ustawioną ładną ikonę w swoim projekcie (robisz to jednym kliknięciem w narzędziu flutter\_launcher\_icons).  
3. Flutter po starcie ukrywa swoje okno, dodaje ikonkę do traya i uruchamia proces C++ w tle (Process.start).

W ten sposób w systemie "głównym" procesem z perspektywy UI jest Flutter, który ma poprawną, nowoczesną ikonę, a okno X11 z symulacją jest traktowane jako okno pomocnicze.

---

### **3\. Dlaczego stara ikona "zniknęła"?**

GNOME i Wayland odchodzą od pobierania ikon z protokołu X11, ponieważ:

* **Skalowanie:** Ikony w X11 były często małymi bitmapami (np. 32x32). Na ekranach 4K wyglądają tragicznie.  
* **Bezpieczeństwo i porządek:** System chce mieć centralną bazę ikon w /usr/share/icons, a nie ufać binarkom, że "coś tam sobie narysują" w rogu okna.

### **Co możesz więcej zrobić?**

Jeśli chcesz, aby Twoje menu we Flutterze "przejęło" reprezentowanie aplikacji:

1. Wstaw do folderu assets Fluttera ikonę w formacie PNG (np. 512x512).  
2. Użyj wspomnianego wcześniej tray\_manager, aby ta ikona była zawsze widoczna w zasobniku (tam, gdzie GNOME niczego nie blokuje).  
3. W pliku .desktop, który przygotujesz dla użytkowników, wskaż ikonę Fluttera.

Dzięki temu Twoja aplikacja wreszcie przestanie wyglądać jak "puste okno" i zyska profesjonalny wygląd na pasku zadań.


### **4\. Katalog lokalny **

Jeśli chodzi o lokalny katalog dla użytkownika (bez uprawnień roota), to w systemach Linux zgodnych ze standardem **XDG**, pliki .desktop (które w Twoim przypadku pełnią rolę "opisu aplikacji", choć mają składnię INI, a nie TOML) należy umieszczać tutaj:

### ---

### **Ścieżka lokalna:**

\~/.local/share/applications/  

Jeśli ten katalog nie istnieje, możesz go utworzyć komendą:  

```
mkdir \-p \~/.local/share/applications
```

### **Jak to się ma do TOML?**

Warto tutaj wyjaśnić pewne nieporozumienie terminologiczne:

* **Pliki .desktop**: To standard w Linuxie (GNOME, KDE, XFCE). Mimo że wyglądają podobnie do formatu TOML (mają sekcje w nawiasach kwadratowych jak \[Desktop Entry\]), to technicznie jest to format **Desktop Entry Specification**. Systemy Linux szukają właśnie tych plików, aby wyświetlić ikonę w menu czy na pasku zadań.  
* **TOML**: Jest używany przez nowsze narzędzia (np. Rust, niektóre konfiguratory), ale sam menedżer okien (GNOME) go nie odczyta jako definicji skrótu do aplikacji.

### ---

### **Jak powinien wyglądać ten plik, aby naprawić Twój problem z ikoną?**

Stwórz plik o nazwie np. symulator.desktop w powyższym katalogu i wypełnij go tak:

```Ini, TOML
\[Desktop Entry\]  
Type=Application  
Name=Symulator C++  
Comment=Mój zaawansowany symulator  
\# Ścieżka do Twojej binarki C++ lub wrappera we Flutterze  
Exec=/home/uzytkownik/bin/moj\_symulator  
\# Ścieżka do ikony (najlepiej PNG 256x256 lub SVG)  
Icon=/home/uzytkownik/obrazy/ikona\_symulatora.png  
Terminal=false  
\# KLUCZOWE: Musi być identyczne z WM\_CLASS ustawionym w kodzie C++ X11  
StartupWMClass=symulator\_bin
```

### **Co to daje?**

1. **Ikona w Menu**: Aplikacja pojawi się w wyszukiwarce systemowej z Twoją grafiką.  
2. **Grupowanie okien**: Gdy uruchomisz program, GNOME "zobaczy" StartupWMClass, dopasuje go do pliku .desktop i podświetli właściwą ikonę na pasku zadań (zamiast pokazywać znak zapytania lub puste okno).  
3. **Brak potrzeby roota**: Instalujesz to tylko dla swojego użytkownika, nie psując nic w systemie.

### **Jak sprawdzić czy działa?**

Po zapisaniu pliku w \~/.local/share/applications/, system powinien go wykryć automatycznie w ciągu kilku sekund. Możesz też wymusić odświeżenie bazy komendą:  
update-desktop-database \~/.local/share/applications  
Czy chciałbyś, aby Twój program we Flutterze generował taki plik automatycznie przy pierwszym uruchomieniu, żeby użytkownik nie musiał robić tego ręcznie?


