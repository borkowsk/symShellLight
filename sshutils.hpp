/**
* Implementacja pomocniczych funkcji symshell'a w sposób już niezależny od platformy.
* Napisane PRAWIE NIEOBIEKTOWO ale w C++
* Jest tu: print_width() , rect(), bar3D(), ...itp...
******************************************************************************************* */
#ifndef __SSHUTILS_HPP__
#define __SSHUTILS_HPP__

#ifndef __cplusplus
#error C++ required
#endif

#include <math.h>
#include "symshell.h"

#ifndef M_PI		/* np MVC++ nie definiuje */
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#endif

/// W funkcjach modułu 'sshutils' używany jest kolor indeksowany
typedef ssh_color wb_color;

/// Zestaw identyfikatorów kolorów 'sshutils'
const wb_color default_transparent=wb_color(-1);
const wb_color default_color=default_transparent;
const wb_color default_black=0;
const wb_color default_white=255;
const wb_color default_dark_gray=256+64;
const wb_color default_half_gray=256+128;
const wb_color default_light_gray=256+128+64;

/// Ustawienia grubości i rozmiarów elementów 'sshutils'
extern int def_frame_width;//=1;    //Domyślna grubość ramki
extern int def_cross_width;//=5;    //Domyślna szerokość krzyżyka
extern int def_scale_width;//=10;   //Domyślna szerokość skali
extern double def_arrow_size;//=15; //Domyślna długość grota strzałki
extern double def_arrow_theta;//=M_PI/6.0+M_PI; //Domyślne rozwarcie grota strzałki


struct settings_bar3d
{
settings_bar3d(int ia=10,int ib=10,int ic=6,
			   wb_color wcol=default_white,
			   wb_color bcol=default_black):
		a(ia),b(ib),c(ic),wire(wcol),back(bcol){}
int a;
int b;
int c;
wb_color wire; //Kolor ramek
wb_color back; //Informacja o kolorze tla
};

/// Konfiguracja słupków 3D. Zwraca poprzednią konfiguracje.
/// Jeśli parametr == NULL to przywraca poprzednio zapamiętaną
const settings_bar3d* bar3d_config(settings_bar3d* st);

/// Rysuje słupek 3D w kolorach indeksowanych
void bar3d(int x,int y,int h,wb_color col1,wb_color col2);

/// Rysuje słupek 3D w kolorze RBG z cieniem
void bar3dRGB(int x,int y,int h,int R,int G,int B,int ShadowDiv);

/// Rysuje kwadratowa ramkę o zadanej grubości
void rect(int x1,int y1,int x2,int y2,wb_color frame_c,int width=def_frame_width);

/// Rysuje pionową skalę kolorów
void ver_scale(int x1,int y1,int width=def_scale_width,wb_color start=0,wb_color end=255);

/// Rysuje poziomą skalę kolorów
void hor_scale(int x1,int y1,int high=def_scale_width,wb_color start=0,wb_color end=255);

/// Rysuje krzyżyk
void cross(int x,int y,wb_color color,int width=def_cross_width);

/// Rysuje dowolnie skierowaną strzałkę od punktu x1y1 do x2y2
void arrow(int x1,int y1,int x2,int y2,wb_color color,double size=def_arrow_size,double theta=def_arrow_theta);

/// Wydajnie rysuje poziomą strzałkę
void vert_arrow(int x1,int x2,int y,wb_color color,double size=def_arrow_size);

/// Efektywnie rysuje pionową strzałkę
void hor_arrow(int x,int y1,int y2,wb_color  color,double size=def_arrow_size);

/// Drukuje tekst w obszarze nie szerszym niz max_width. Zwraca width albo 0
/// wewnętrzny bufor ma nie więcej niż 1024 znaki
int print_width(int x,int y,int max_width,wb_color col,wb_color bcg,const char* format ...);

/// Wyświetlanie pliku HTML poprzez system shell systemowy.
/// Tak naprawdę można użyć do wszystkich typów plików
/// jakie może wyświetlić przeglądarka
extern "C" { int ViewHtml(const char* URL); }

/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-24                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif


