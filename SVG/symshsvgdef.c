/// @file
/// @brief Zmienne konfiguracyjne modułu SVG — definicje
///        EN: SVG module configuration variables — definitions.
/// @date 2026-04-27 (modified)
//  ------------------------------------------------------------
// Created by borkowsk on 26.04.26.
//

#include "symshsvgdef.h"

// Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu).
double      GrSVG_INITIAL_LENGTH_RATIO = 0.005;

// Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu).
double      GrSVG_MAXIMAL_LENGTH_RATIO = 1.999;

// Co ile czasu skrypt w pliku SVG wymusza przeładowanie. Jak 0 to w ogóle nie ma skryptu.
unsigned    GrSVG_ReloadInterval = 1000;

// Ścieżka, gdzie ma wrzucać zrzuty tymczasowe.
const char* GrSVG_TmpOutputDirectory = "./";

// Rozszerzenie nazwy pliku wyjściowego. Jednocześnie określa format pliku wyjściowego.
// Jak extension nieznane, to wyrzuca strumień obiektowy '.str'.
const char* GrSVG_FileOutputByExtension = "svg";


/* *******************************************************************/
/*               SYMSHELLLIGHT version 2026                          */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
