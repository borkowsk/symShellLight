# SymShellLight

EN: Light version of __Simulation Shell__. Mostly basic graphix for _X11_,_MS Windows_ & _SVG_ for educational purpose

PL: Lekka wersja grafiki __SYMSHELL__'a dla _X11_, _MS Windows_ & _SVG_ z niektórymi podstawowymi funkcjonalnościami czasu wykonania. Wyodrebnione w celach dydaktycznych

**HISTORY**

PL: Projekt biblioteki Simulation Shell dla C/C ++ został rozpoczęty ponad 20 lat temu (około roku 1995) w celu ułatwienia tworzenia programów symulacyjnych łatwych do przenoszenia pomiędzy systemami i różnymi kompilatorami C/C++, które były wtedy dużo bardziej zróżnicowane niż dzisiaj. Zawierał podstawową grafikę oraz semigrafikę przenośna pod unix, linux (X11), windows oraz DOS (pod GO32) oraz moduły do zarządzania wynikami symulacji i ich wizualizacji.

EN: The Simulation Shell library project for C/C ++ was started over 20 years ago (around 1995) to facilitate the development of simulation programs easily portable between systems and different C/C++ compilers, which were then much more varied than today. It included basic graphics and portable semigraphics under unix, linux (X11), windows and DOS (under GO32) and modules for managing simulation results and their visualization.

**Quick start**

The repository contains the library code and sample programs in the _TESTS/_ directory
To use, perform the following operations in the selected directory:

```console
  $ git clone https://github.com/borkowsk/SymShellLight.git
  $ cd SymShellLigt 
  $ cmake . -DX11=true OR -DWINDOWS=true
  $ make
```
You can also compile tests:

```console
  $ cmake . -DWITHTESTS=true
  $ make
```

It was tested on UBUNTU 16.04 with gcc, UBUNTU 18.04 with gcc & MS Windows 7 with MSVC 2012


**Licencing**

You can use this software freely for educational and research purposes, but if you feel that it would be appropriate to repay somehow, please finance me a big coffee :-)

https://www.paypal.com/paypalme/wborkowsk
