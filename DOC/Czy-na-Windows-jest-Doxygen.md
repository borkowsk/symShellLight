# DOXYGEN pod MS Windows
## @date 2026-04-29 (created)

Doxygen jest w pełni dostępny na system Windows i jest jednym z najpopularniejszych narzędzi do generowania dokumentacji na tej platformie.

Możesz go zainstalować na kilka sposobów:

### 1. Oficjalny instalator (Zalecane)

Na stronie _[doxygen.nl](https://www.doxygen.nl/download.html)_ znajdziesz gotowe pliki binarne dla Windows:

* **Instalator (.exe):** Standardowy plik instalacyjny, który konfiguruje ścieżki systemowe.  
* **Wersja przenośna (.zip):** Archiwum zawierające pliki wykonywalne, które wystarczy wypakować (nie wymaga instalacji).

### 2. Menedżery pakietów

Jeśli korzystasz z terminala, możesz zainstalować Doxygen za pomocą popularnych menedżerów:

* **Winget:** winget install doxygen.doxygen  
* **Chocolatey:** choco install doxygen


### 3. Dodatkowe narzędzia warto mieć na Windows:

* **Doxywizard:** Graficzny interfejs (GUI), który jest dołączony do instalatora Windows. Znacznie ułatwia tworzenie i edycję pliku konfiguracyjnego (Doxyfile) bez konieczności ręcznej edycji tekstu.  
* **Graphviz:** Warto zainstalować go osobno. Doxygen wykorzystuje narzędzie dot z pakietu Graphviz do generowania diagramów klas, dziedziczenia i zależności. Bez niego dokumentacja będzie zawierać jedynie tekst i proste tabele.  
* **HTML Help Workshop:** Jeśli planujesz generować dokumentację w formacie .chm (stary, ale wciąż używany format pomocy Windows).

