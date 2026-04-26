//
// Created by borkowsk on 26.04.26.
//

#include "../SVG/symshsvgdef.h"


// Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu).
double      GrSVG_INITIAL_LENGTH_RATIO = 0.05;

// Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu).
double      GrSVG_MAXIMAL_LENGTH_RATIO = 1.999;

// Co ile czasu skrypt w pliku SVG wymusza przeładowanie. Jak 0 to w ogóle nie ma skryptu.
unsigned    GrSVG_ReloadInterval = 3000;

// Ścieżka, gdzie ma wrzucać zrzuty tymczasowe.
const char* GrSVG_TmpOutputDirectory = "./";
