#  SymShellLight - basic graphics for Unix/Window simulations
## @date 2026-05-09 (last change)

 [//]: # (date is updated automatically by git preprocessing script) 

Light version of __Simulation Shell__. Mostly basic graphic for _X11_,_MS Windows_ & _SVG_ 
for educational and scientific purposes.

PL: Lekka wersja grafiki __SYMSHELL__'a dla _X11_, _MS Windows_ & _SVG_ 
z niektórymi podstawowymi funkcjonalnościami czasu wykonania. 
Wyodrębnione z pełnej wersji w celach dydaktycznych i naukowych.

>[!NOTE]
>Header files previously located in the repository's root directory have been moved to the INCL(or INCL_EN)
> directory as of January 2026. You must correct this path in your projects!

>[!IMPORTANT]
>PL: Pliki nagłówkowe, umieszczone dotychczas w katalogu głównym repozytorium zostały od stycznia 2026 przeniesione 
>do katalogu INCL (albo INCL_EN). Musicie poprawić tę ścieżkę w waszych projektach!

<img src="./DOC/renovation.gif" width="100" title="Under renovation">

**HISTORICAL OUTLINE/RYS HISTORYCZNY**

How could a programmer accustomed to "linear" simulation programming become adopted to the event-driven interface 
of _X11_ or __MS Windows__ applications, in which control of the main program thread is transferred to the GUI?
A library was needed that would hide the complexity of the event interface and its reliance on the main process thread. 
This was precisely what SymShell was initially intended to accomplish. And then it evolved...

The _Simulation Shell_ library project for C/C++ was started around 1995 to facilitate
the development of simulation programs easily portable between systems and different C/C++
compilers, which were then much more varied than today.
It included basic graphics and portable semigraphics under __unix__, __linux__ (_X11_),
__Windows__ and __DOS__ (under _GO32_) and modules for managing simulation results and their visualization
(Currently available in the [_SymShell2AndRTM_](https://github.com/borkowsk/symShell2andRTM) package).

In one way or another, it still serves me today. It can be also useful to any computer simulation developer who is no
longer satisfied with _Python_ or _Matlab_, but who does not have the time to delve into the intricacies of Qt, or,
even more so, the bare programming interfaces of windowing systems. Additionally, if you're using _Processing_ but find 
the speed of its code execution in the Java virtual machine insufficient, you might be interested in the translator
to C++, which I've been working on for several years. Its graphics translation is also based on the _SymShell_ library.

PL: Jak programista przyzwyczajony do "liniowego" programowania symulacji mógł się oswoić z ze zdarzeniowym interfejsem 
aplikacji _X11_ czy __MS Windows__, w którym sterowanie głównym wątkiem programu oddawane jest GUI?
Potrzebna byłaby biblioteka, która ukrywała by złożoność interfejsu zdarzeniowego i jego zachłanność na wątek główny 
procesu. Temu właśnie miał początkowo służyć SymShell. A potem się rozwinął...

Projekt biblioteki _Simulation Shell_ dla C/C++ został rozpoczęty około roku 1995 w celu
ułatwienia tworzenia programów symulacyjnych łatwych do przenoszenia pomiędzy systemami
i różnymi kompilatorami C/C++, które były wtedy dużo bardziej zróżnicowane niż dzisiaj.
Zawierał podstawową grafikę oraz semigrafikę przenośna pod __unix__, __linux__ (_X11_),
__Windows__ oraz __DOS__ (pod _GO32_) oraz moduły do zarządzania wynikami symulacji i ich wizualizacji
(Aktualnie dostępne w pakiecie [_SymShell2AndRTM_](https://github.com/borkowsk/symShell2andRTM) ).

W taki czy inny sposób służy mi to do dzisiaj i może posłużyć każdemu twórcy symulacji komputerowych, któremu użycie 
_Pythona_ czy _Matlaba_ już nie wystarcza, a jednocześnie nie ma czasu wgryzać się w zawiłości Qt, czy, tym bardziej, 
gołych interfejsów programistycznych systemów okienkowych. Ponadto, jeśli używa Processingu, ale nie wystarcza 
mu szybkość kodu wykonywanego przez maszynę wirtualną JAVA-y, może go zainteresować translator na C++, nad którym 
pracuję od kilku lat. Przekład grafiki w nim także opiera się na bibliotece _SymShell_.

<img src="DOC/Just a linking test_16904.svg"/>

**Quick start/dla niecierpliwych**

The repository contains the library code and sample programs in the _EXAMPLES/_ directory
To use, perform the following operations in the selected directory:

PL: Repozytorium zawiera kod biblioteki i przykładowe programy w katalogu _EXAMPLES/_. Aby z nich skorzystać, 
wykonaj następujące operacje w wybranym katalogu:

```bash
  $ git clone https://github.com/borkowsk/SymShellLight.git
  $ cd SymShellLigt
  $ bash _check_installation.sh
  $ cmake . -DWITHTESTS=false
  $ make
```
You can also compile with tests using "true":

```bash
  $ cmake . -DWITHTESTS=true
  $ make
```

It was tested on __UBUNTU 16.04__ with _gcc_, __UBUNTU 18.04__ with _gcc_ & __UBUNTU 20.04__ with _gcc/g++_ and __POP OS 22.04__.
Current version does not work fully under __Windows__, because some functions are missed or cannot be implemented easily!!!

PL: Zostało przetestowane na __UBUNTU 16.04__ z _gcc_, __UBUNTU 18.04__ z _gcc_, __UBUNTU 20.04__ z _gcc/g++_ oraz __POP OS 22.04__.
Obecna wersja nie działa w pełni w __Windows__, ponieważ niektóre funkcje są pominięte lub nie można ich łatwo zaimplementować!!!

**REQUIREMENTS/WYMAGANIA**

* cmake
* make
* c/c++ (gcc/g++)
* For _unix_ native graphics it needs development version of __X11__ packages including XPM
  ( _libxpm-dev_, so on UBUNTU use: `sudo apt install libxpm-dev` )
* _rofi_ for context menus
* _Doxygen_, _DoxyWizard_ for creating documentation.

**DOC/-kumentacja**

If you can run the _Doxygen_ package in your system, simply run the *_makeDocs.sh* script. Everything
is ready to generate documentation in Polish or English, or Polish/English (depending on the file).
In modern IDEs, the Doxygen documentation should also work well as tooltips. The header files' language will 
be adjusted to match your system's language, but you can also choose your own language by modifying two
lines in the *CMakeFiles.txt* file.

PL: Jeśli możesz uruchomić u siebie pakiet _Doxygen_, to wystarczy, że uruchomisz skrypt *_makeDocs.sh*. Wszystko 
jest przygotowane do wytworzenia dokumentacji w wersji polskiej lub angielskiej, lub polsko/angielskiej (w zależności 
od pliku). W nowoczesnych IDE dokumentacja doxygen-owa powinna też dobrze działać jako podpowiedzi w trakcie pracy.
Język plików nagłówkowych zostanie dopasowany do języka twojego systemu, ale możesz też wybrać sam modyfikując dwie 
linie w pliku *CMakeFiles.txt*.


**Licencing/Licencja**

I've made every effort to ensure this software works properly, but of course, you use it at your own risk. 
You can use it free of charge for educational and research purposes, and if you feel it's appropriate to give back 
in some way, please buy me a large coffee :-)

PL: Dołożyłem wszelkich starań, żeby to oprogramowanie działało, jak należy, ale oczywiście używasz na własną 
    odpowiedzialność. Możesz używać go bezpłatnie w celach edukacyjnych i badawczych, a jeśli uznasz, że wypadałoby 
    się w jakiś sposób odwdzięczyć, to proszę, postaw mi dużą kawę :-)


**COFFEE/KAFKA**

* [PayPal](https://www.paypal.com/paypalme/wborkowsk)
  OR
* [Coffee](https://buycoffee.to/adalbertus)

