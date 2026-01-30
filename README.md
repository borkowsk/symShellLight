#  SymShellLight - basic graphics for Unix/Window simulations
## @date 2026-01-30 (last change)

 [//]: # (date is updated automatically by git preprocessing script) 

Light version of __Simulation Shell__. Mostly basic graphic for _X11_,_MS Windows_ & _SVG_ 
for educational and scientific purposes.

PL: Lekka wersja grafiki __SYMSHELL__'a dla _X11_, _MS Windows_ & _SVG_ 
z niektórymi podstawowymi funkcjonalnościami czasu wykonania. 
Wyodrębnione z pełnej wersji w celach dydaktycznych i naukowych.

>[!NOTE]
>Header files previously located in the repository's root directory have been moved to the INCL directory as of January 2026.
>You must correct this path in your projects!

>[!IMPORTANT]
>PL: Pliki nagłówkowe, umieszczone dotychczas w katalogu głównym repozytorium zostały od stycznia 2026 przeniesione 
>do katalogu INCL. Musicie poprawić tę ścieżkę w waszych projektach!

**HISTORY**

The _Simulation Shell_ library project for C/C++ was started around 1995 to facilitate 
the development of simulation programs easily portable between systems and different C/C++ 
compilers, which were then much more varied than today. 
It included basic graphics and portable semigraphics under unix, linux (X11), 
Windows and DOS (under GO32) and modules for managing simulation results and their visualization.

PL: Projekt biblioteki _Simulation Shell_ dla C/C++ został rozpoczęty około roku 1995 w celu 
ułatwienia tworzenia programów symulacyjnych łatwych do przenoszenia pomiędzy systemami 
i różnymi kompilatorami C/C++, które były wtedy dużo bardziej zróżnicowane niż dzisiaj. 
Zawierał podstawową grafikę oraz semigrafikę przenośna pod unix, linux (X11), 
Windows oraz DOS (pod GO32) oraz moduły do zarządzania wynikami symulacji i ich wizualizacji.

<img src="DOC/Just a linking test_16904.svg"/>

**Quick start**

The repository contains the library code and sample programs in the _EXAMPLES/_ directory
To use, perform the following operations in the selected directory:

```bash
  $ git clone https://github.com/borkowsk/SymShellLight.git
  $ cd SymShellLigt
  $ bash _check.sh
  $ cmake . -DWITHTESTS=false
  $ make
```
You can also compile with tests:

```bash
  $ cmake . -DWITHTESTS=true
  $ make
```

It was tested on __UBUNTU 16.04__ with _gcc_, __UBUNTU 18.04__ with _gcc_ & __UBUNTU 20.04__ with _gcc/g++_ and __POP OS 22.04__.
Current version does not work fully under __Windows__, because some functions are missed or cannot be implemented easily!!! 

**REQUIREMENTS**

* cmake
* make
* c/c++ (gcc/g++)
* For unix native graphixs it needs development version of __X11__ packages including XPM
  ( _libxpm-dev_, so on UBUNTU use: `sudo apt install libxpm-dev` )
* Also _doxygen_ adn _doxywizard_ for creating documentation.

**Licencing**

You can use this software freely for educational and research purposes, but if you feel that it would be appropriate to repay somehow, please finance me a big coffee :-)

**COFFEE**

* https://www.paypal.com/paypalme/wborkowsk

