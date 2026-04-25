/** @file   sshutils.hpp
 *  @brief  Implementacja pomocniczych funkcji symshell-a w sposób już niezależny od platformy.  */
/*         ===================================================================================== */
/** @date 2026-04-26 (last modification)
*   @details
*          Napisane PRAWIE NIEOBIEKTOWO ale w C++
*          Jest tu: print_width() , puste rect(), bar3D(), arrow() ...itp...
*   @author borkowsk                                                                             */
/* ********************************************************************************************* */
#ifndef SYMSHELL_UTILS_HPP_INCLUDED_
#define SYMSHELL_UTILS_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include <cmath>
#include "symshell.h"
#include "maybe_unused.h"

#ifndef M_PI
/** @name M_PIx
 * @brief Własne definicje stałych związanych z liczbą Pi.
 * @details Definiowane, gdy nie są dostarczone przez kompilator. Np. starsze MVC++ nie definiowało.
 * @{
 */
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
/** @} */
#endif


/**
 * @defgroup SymShellUtils Różne dodatkowe narzędzia do grafiki
 * \brief    Zdefiniowane kolory i dodatkowe kształty.
 */
/// @{

/// Pomocnicza funkcja kwadratowa dla skrócenia kodu tu i tam.
template<class NUM>
inline NUM sqr(const NUM& x)
{ return x*x; }

/// \brief Obliczanie odległości Euklidesa. Często potrzebne w takich programach.
/// @note NIEINTUICYJNY UKŁAD PARAMETRÓW!
MAYBE_UNUSED
double distance(double X1,double X2,double Y1,double Y2);

/// \brief Alias dla typu `ssh_color`.
/// \note W funkcjach rysujących modułu 'sshutils' używany jest kolor indeksowany.
typedef ssh_color wb_color;

/**
 * @name Zestaw identyfikatorów kolorów 'sshutils'.
 * @details Wszystko to są wybrane indeksy zestawu 256 kolorów i 256 odcieni szarości.
 * @{
 */
const wb_color default_transparent=wb_color(-1);     ///< Domyślny kolor do oznaczania transparentności.
const wb_color default_black=0;                      ///< Domyślny kolor indeksowany czarny.
const wb_color default_white=255;                    ///< Domyślny kolor indeksowany biały.
MAYBE_UNUSED
const wb_color default_color=default_transparent;    ///< Domyślny kolor indeksowany.
MAYBE_UNUSED
const wb_color default_dark_gray=256+64;             ///< Domyślny kolor indeksowany ciemnoszary.
MAYBE_UNUSED
const wb_color default_half_gray=256+128;            ///< Domyślny kolor indeksowany średnio szary.
MAYBE_UNUSED
const wb_color default_light_gray=256+128+64;        ///< Domyślny kolor indeksowany jasnoszary.
/** @} */

/// \brief Funkcja interpretująca string jako wartość RGB.
/// \param s powinno zawierać tekst z definicją koloru RBG.
/// \param end_ptr pozwala sprawdzić, czy nie było błędu.
/// \return kolor RBG zakodowany w postaci liczby 32-bitowej. TODO — powinno zwracać ssh_rgba, ale na razie nie używamy
/// \details Dopuszczalne formaty to: xffffff  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255)
MAYBE_UNUSED
unsigned strtorgb(const char *s, char **end_ptr);

/**
 * @name Ustawienia grubości i rozmiarów elementów 'sshutils'.
 * @details Wartości konfiguracyjne dla ramek, krzyżyków strzałek itp...
 * @{
 */

extern int def_frame_width; /* =1;*/                  ///< Domyślna grubość ramki.
extern int def_cross_width; /* =5;*/                  ///< Domyślna szerokość krzyżyka.
extern int def_scale_width; /* =10;*/                 ///< Domyślna szerokość skali.
extern double def_arrow_size; /* =15;*/               ///< Domyślna długość grota strzałki.
extern double def_arrow_theta; /*=M_PI/6.0+M_PI;*/    ///< Domyślne rozwarcie grota strzałki.

/// \brief Struktura konfiguracji słupków 3D.
struct settings_bar3d
{
    ///Konstruktor struktury konfiguracji słupków 3D.
    settings_bar3d(
               int ia=10,
               int ib=10,
               int ic=6,
               wb_color wcol=default_white,
               wb_color bcol=default_black
               ):
        a(ia),b(ib),c(ic),wire(wcol),back(bcol)
        {}
    int         a; ///< Długość odcinka a słupka.
    int         b; ///< Długość odcinka b słupka.
    int         c; ///< Długość odcinka c słupka.
    wb_color wire; ///< Kolor ramek.
    wb_color back; ///< Informacja o kolorze tla.
};

/// \brief  Funkcja konfiguracji słupków 3D.
/// \return Zwraca poprzednią konfigurację albo NULL, jeśli przywraca poprzednio zapamiętaną.
MAYBE_UNUSED
const settings_bar3d* bar3d_config(settings_bar3d* st);

/** @} */

/// \brief  Rysuje słupek 3D w kolorach indeksowanych.
/// \param x to pozioma współrzędna lewego dolnego rogu frontu.
/// \param y to pionowa współrzędna lewego dolnego rogu frontu.
/// \param h to wysokość frontu.
/// \param col1 to indeks koloru przodu.
/// \param col2 to indeks koloru boku.
MAYBE_UNUSED
void bar3d(int x,int y,int h,wb_color col1,wb_color col2);

/// \brief  Rysuje słupek 3D w kolorze RBG z cieniem.
/// \param x to pozioma współrzędna lewego dolnego rogu frontu.
/// \param y to pionowa współrzędna lewego dolnego rogu frontu.
/// \param h to wysokość frontu.
/// \param R to składowa koloru 'red'.
/// \param G to składowa koloru 'green'.
/// \param B to składowa koloru 'blue'.
/// \param ShadowDiv to dzielnik do RGB, żeby uzyskać cień na bocznej ścianie.
MAYBE_UNUSED
void bar3dRGB(int x,int y,int h,int R,int G,int B,int ShadowDiv);

/// \brief  Rysuje kwadratową ramkę o zadanej grubości.
MAYBE_UNUSED
void rect(int x1,int y1,int x2,int y2,wb_color frame_c,int width=def_frame_width);

/// \brief  Rysuje pionową skalę kolorów.
/// \param x1,y1 to współrzędne startowe (lewy górny róg).
/// \param width to szerokość paska.
/// \param start to kolor początkowy.
/// \param end   to kolor końcowy.
MAYBE_UNUSED
void ver_scale(int x1,int y1,int width=def_scale_width,wb_color start=0,wb_color end=255);

/// \brief  Rysuje poziomą skalę kolorów.
MAYBE_UNUSED
void hor_scale(int x1,int y1,int high=def_scale_width,wb_color start=0,wb_color end=255);

/// \brief  Rysuje krzyżyk.
MAYBE_UNUSED
void cross(int x,int y,wb_color color,int line_width=def_cross_width);

/// \brief  Rysuje dowolnie skierowaną strzałkę od punktu x1y1 do x2y2.
/// \param x1,y1 to współrzędne początku strzałki.
/// \param x2,y2 to współrzędne końca strzałki.
/// \param color to indeks koloru strzałki.
/// \param size  to długość grotu.
/// \param theta to kierunek strzałki (kat w radianach).
MAYBE_UNUSED
void arrow(int x1,int y1,int x2,int y2,wb_color color,double size=def_arrow_size,double theta=def_arrow_theta);

/// \brief  Efektywnie rysuje poziomą strzałkę.
MAYBE_UNUSED
void vert_arrow(int x1,int x2,int y,wb_color color,double size=def_arrow_size);

/// \brief  Efektywnie rysuje pionową strzałkę.
MAYBE_UNUSED
void hor_arrow(int x,int y1,int y2,wb_color  color,double size=def_arrow_size);

/// \brief   Drukuje tekst w obszarze nie szerszym niż `max_width`.\
/// \param x, y to współrzędne punktu startowego tekstu.
/// \param max_width to maksymalna długość w pikselach.
/// \param col, bcg to color tekstu i tła.
/// \param format format tekstu jak dla `printf`.
/// \param ... to zmienne parametry wynikające z formatu.
/// \return  Zwraca width albo 0.
/// \details wewnętrzny bufor ma nie więcej niż 1024 znaki.
MAYBE_UNUSED
int print_width(int x,int y,int max_width,wb_color col,wb_color bcg,const char* format ...);

extern "C" {
/// \brief Wyświetlanie pliku HTML poprzez systemowy shell.
/// \details Tak naprawdę można użyć do wszystkich typów plików
///          , jakie może wyświetlić przeglądarka.
/// \note Pochodzi z biblioteki WB_RTM, a w "sshutils.h" jest zadeklarowane dla wygody.
/// \param URL - pełny URL, ale czasem ujdzie i nazwa pliku :-D ...
/// \return powinien zwrócić kod wykonania programu "dziecka".
MAYBE_UNUSED
int ViewHtml(const char* url);
// TODO int view_html(const char* url);
}

///@name Proste wsparcie dla interfejsu wielojęzycznego.
///@details Wybór łańcuchów tekstowych z jednego z dwóch albo trzech języków narodowych.
/// @{
extern unsigned lang_selector; ///< @brief Zmienna użytkownika dla zestawu funkcji `lang`.

MAYBE_UNUSED inline /// Wybór jednego z dwóch łańcuchów tekstowych na podstawie zmiennej `lang_selector`.
const char* lang(const char* def,const char* alt)
{ if(lang_selector!=0) return alt;else return def;}

MAYBE_UNUSED inline /// Wybór jednego z trzech łańcuchów tekstowych na podstawie zmiennej `lang_selector`.
const char* lang(const char* def,const char* alt1,const char* alt2)
{ if(lang_selector==2) return alt2;if(lang_selector==1)return alt1; else return def;}
/// @}

/// @}

/* ******************************************************************/
/*                 SYMSHELLLIGHT  version 2026                      */
/* ******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif


