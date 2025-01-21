#  SymShellLight - basic graphics for Unix/Window simulations
## @date 2024-10-01 (last change)

 [//]: # (date is updated automatically by git preprocessing script) 

Light version of __Simulation Shell__. Mostly basic graphic for _X11_,_MS Windows_ & _SVG_ for educational purpose

PL: Lekka wersja grafiki __SYMSHELL__'a dla _X11_, _MS Windows_ & _SVG_ z niektórymi podstawowymi funkcjonalnościami czasu wykonania. 
Wyodrębnione z pełnej wersji w celach dydaktycznych i eksportowych.


**HISTORY**

The Simulation Shell library project for C/C++ was started over 20 years ago (around 1995) to facilitate the development of simulation programs easily portable between systems and different C/C++ compilers, which were then much more varied than today. It included basic graphics and portable semigraphics under unix, linux (X11), windows and DOS (under GO32) and modules for managing simulation results and their visualization.

PL: Projekt biblioteki Simulation Shell dla C/C++ został rozpoczęty ponad 20 lat temu (około roku 1995) w celu ułatwienia tworzenia programów symulacyjnych łatwych do przenoszenia pomiędzy systemami i różnymi kompilatorami C/C++, które były wtedy dużo bardziej zróżnicowane niż dzisiaj. Zawierał podstawową grafikę oraz semigrafikę przenośna pod unix, linux (X11), windows oraz DOS (pod GO32) oraz moduły do zarządzania wynikami symulacji i ich wizualizacji.

<img src="DOC/Just a linking test_16904.svg"/>

**Quick start**

The repository contains the library code and sample programs in the _TESTS/_ directory
To use, perform the following operations in the selected directory:

```bash
  $ git clone https://github.com/borkowsk/SymShellLight.git
  $ cd SymShellLigt
  $ bash _check.sh
  $ cmake . -DX11=true OR -DWINDOWS=true -DWITHTESTS=false
  $ make
```
You can also compile tests:

```bash
  $ cmake . -DWITHTESTS=true
  $ make
```

It was tested on __UBUNTU 16.04__ with _gcc_, __UBUNTU 18.04__ with _gcc_ & __UBUNTU 20.04__ with _gcc/g++_ and __POP OS 22.04__.
Current version does not work fully under __Windows__, because some functions are missed or cannot be implemented easily!!! 

**REQUIREMENTS**

* Development version of __X11__ packages

* _libxpm-dev_ (on UBUNTU: `sudo apt install libxpm-dev` )

**Licencing**

You can use this software freely for educational and research purposes, but if you feel that it would be appropriate to repay somehow, please finance me a big coffee :-)

**COFFEE**

* https://www.buymeacoffee.com/wborkowsk
* https://www.paypal.com/paypalme/wborkowsk

last big modification: 2022-10-27
