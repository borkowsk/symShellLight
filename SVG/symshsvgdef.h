/// @file
/// @brief Zmienne konfiguracyjne modułu SVG — deklaracje.
///        EN: SVG module configuration variables - declarations.
/// @date 2026-04-27 (modified)
//---------------------------------------------------------------
// Created by borkowsk on 26.04.26.
//
#ifndef SYMSHELLLIGHT_SYMSHSVGDEF_H
#define SYMSHELLLIGHT_SYMSHSVGDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @name Variables controlling SVG file output.
/// @details These can be defined in your own program as they reside in a separate auxiliary module of the library ("symshsvhdef.c").
///          In particular, increasing `GrSVG_MAXIMAL_LENGTH_RATIO` is recommended when the `plot` function is used extensively.
///          However, if modification becomes necessary, all five variables must be defined!
///
///          These variables control the conversion of symshell graphical commands into an SVG file. For simple
///          graphics, these settings are of little significance. However, keep in mind that the command buffer
///          has a finite maximum length; if exceeded, only a portion of the designed drawing will be written
///          to the SVG file. To prevent this, call `clear_screen` or `invalidate_screen` as frequently as
///          is practical.
#else
/// @name Zmienne sterujące zapisem do pliku SVG.
/// @details Można je zdefiniować we własnym programie, bo są w osobnym module pomocniczym biblioteki ("symshsvhdef.c").
///          Zwłaszcza `GrSVG_MAXIMAL_LENGTH_RATIO` warto powiększyć, gdy masowo używana jest funkcja `plot`.
///          Gdy jednak staje się to potrzebne, to trzeba już zdefiniować wszystkie pięć zmiennych!
///
///          Zmienne te sterują przetwarzaniem komend graficznych symshell-a na plik SVG. Gdy grafiki są niezbyt
///          skomplikowane, ustawienie nie mają wielkiego znaczenia. Trzeba jednak pamiętać, że bufor zapamiętujący
///          komendy ma skończoną długość maksymalną i gdy zostanie ona przekroczona, to jedynie cześć zaprojektowanego
///          rysunku będzie trafiać do pliku SVG. Ratunkiem jest wywoływanie `clear_screen` lub `invalidate_screen` tak
///          często jak to tylko ma sens.
#endif
/// @{

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief What length do we initialize the graphics operation tables with (multiplied by the declared number of screen pixels).
#else
/// @brief Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez zadeklarowaną liczbę pikseli ekranu).
#endif
extern double      GrSVG_INITIAL_LENGTH_RATIO; // = 0.005;

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief What length do we initialize the graphics operation tables with (multiplied by the declared number of screen pixels).
#else
/// @brief Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez zadeklarowaną liczbę pikseli ekranu).
#endif
extern double      GrSVG_MAXIMAL_LENGTH_RATIO; // = 0.999;

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief How often does the script in the SVG file force a reload?
/// If 0, there is no script and no automatic reloading.
#else
/// @brief Co ile czasu skrypt w pliku SVG wymusza przeładowanie.
/// Gdy 0 to w ogóle nie ma skryptu i automatycznego ładowania.
#endif
extern unsigned    GrSVG_ReloadInterval; // = 1000;

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief
#else
/// @brief Ścieżka, gdzie ma wrzucać zrzuty tymczasowe.
#endif
extern const char* GrSVG_TmpOutputDirectory; // = "./";

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Output filename extension.
/// Also specifies the output file format.
/// If the extension is unknown, throws the '.str' object stream
#else
/// @brief Rozszerzenie nazwy pliku wyjściowego.
/// Jednocześnie określa format pliku wyjściowego.
/// Jak extension nieznane, to wyrzuca strumień obiektowy '.str'
#endif
extern const char* GrSVG_FileOutputByExtension; // = "svg" or "txt" for debugging;

/// @}

#ifdef USE_ENGLISH_IF_POSSIBLE
/** @name Variables controlling mouse simulation.
 * @details There is no mouse in SVG by default, but another module can simulate one by linking to these global variables.
 *
 * @var GrMouseX
 * @brief X-coordinate of the simulated mouse (Default = -1).
 * @var GrMouseY
 * @brief Y-coordinate of the simulated mouse (Default = -1).
 * @var GrMouseC
 * @brief Simulated mouse click state: 0, 1, or 2 (Default = -1).
 * @var GrMouseActive
 * @brief Determines whether to check these mouse variables or not (Default = false).
 */
#else
/** @name Zmienne do symulowania myszy.
 * @details Myszy w SVG domyślnie nie ma, ale inny moduł może ją symulować przez linkowanie do tych zmiennych globalnych.
*
 * @var GrMouseX
 * @brief Pozycja X symulowanej myszy (Default = -1).
 * @var GrMouseY
 * @brief Pozycja Y symulowanej myszy (Default = -1).
 * @var GrMouseC
 * @brief Klik symulowanej myszy: 0, 1 lub 2 (Default = -1).
 * @var GrMouseActive
 * @brief Ustala, czy w ogóle sprawdzać zmienne myszowe (Default = false).
 */
#endif
/// @{
extern int GrMouseX;
extern int GrMouseY;
extern int GrMouseC;
extern int GrMouseActive;
/// @}

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief There's no keyboard in the SVG module, but another module can simulate one by linking to this global variable.
/// And just to be sure, it returns "REPLOT" the first time, because that's what X11 does and some simple applications rely on it.
#else
/// @brief W module SVG nie ma też klawiatury, ale inny moduł może ją symulować przez linkowanie do tej zmiennej globalnej.
/// No i dla pewności za pierwszym razem zwraca "REPLOT", bo tak ma X11 i na tym bazują niektóre proste aplikacje.
#endif
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
