/// @file
/// @brief Zmienne konfiguracyjne modułu SVG — deklaracje
//-------------------------------------------------------
/// @date 2026 (created)
// Created by borkowsk on 26.04.26.
//
#ifndef SYMSHELLLIGHT_SYMSHSVGDEF_H
#define SYMSHELLLIGHT_SYMSHSVGDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/// @name Zmienne sterujące zapisem do pliku SVG.
/// @details Można je zdefiniować we własnym programie, bo są w osobnym module pomocniczym biblioteki ("symshsvhdef.c").
///          Zwłaszcza `GrSVG_MAXIMAL_LENGTH_RATIO` warto powiększyć, gdy masowo używana jest funkcja `plot`.
///          Gdy jednak staje się to potrzebne, to trzeba już zdefiniować wszystkie cztery zmienne!
///
///          Zmienne te sterują przetwarzaniem komend graficznych symshell-a na plik SVG. Gdy grafiki są niezbyt
///          skomplikowane, ustawienie nie mają wielkiego znaczenia. Trzeba jednak pamiętać, że bufor zapamiętujący
///          komendy ma skończoną długość maksymalną i gdy zostanie ona przekroczona, to jedynie cześć zaprojektowanego
///          rysunku będzie trafiać do pliku SVG. Ratunkiem jest wywoływanie `clear_screen` lub `invalidate_screen` tak
///          często jak to tylko ma sens.
/// @{

/// @brief Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez zadeklarowaną liczbę pikseli ekranu).
extern double      GrSVG_INITIAL_LENGTH_RATIO; // = 0.005;

/// @brief Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez zadeklarowaną liczbę pikseli ekranu).
extern double      GrSVG_MAXIMAL_LENGTH_RATIO; // = 0.999;

/// @brief Co ile czasu skrypt w pliku SVG wymusza przeładowanie.
/// Gdy 0 to w ogóle nie ma skryptu i automatycznego ładowania.
extern unsigned    GrSVG_ReloadInterval; // = 1000;

/// @brief Ścieżka, gdzie ma wrzucać zrzuty tymczasowe.
extern const char* GrSVG_TmpOutputDirectory; // = "./";
/// @}

/// @brief Rozszerzenie nazwy pliku wyjściowego.
/// Jednocześnie określa format pliku wyjściowego.
/// Jak extension nieznane, to wyrzuca strumień obiektowy '.str'
extern const char* GrSVG_FileOutputByExtension; // = "svg";

/// @name Zmienne do symulowania myszy.
/// @details Myszy w SVG domyślnie nie ma, ale inny moduł może ją symulować przez linkowanie do tych zmiennych globalnych.
/// @{
extern int         GrMouseX;      ///< Default = -1; @brief Pozycja X symulowanej myszy.
extern int         GrMouseY;      ///< Default = -1; @brief Pozycja Y symulowanej myszy.
extern int         GrMouseC;      ///< Default = -1; @brief Klik symulowanej myszy (0,1,2).
extern int         GrMouseActive; ///< Default = false; @brief Ustala czy w ogóle sprawdzać te zmienne myszowe, czy nie.
/// @}

/// W module SVG nie ma też klawiatury, ale inny moduł może ją symulować przez linkowanie do tej zmiennej globalnej.
/// No i dla pewności za pierwszym razem zwraca "REPLOT", bo tak ma X11 i na tym bazują niektóre proste aplikacje.
extern int         GrCharMessage; ///< Default = '\r';

#ifdef __cplusplus
};
#endif


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
#endif //SYMSHELLLIGHT_SYMSHSVGDEF_H
