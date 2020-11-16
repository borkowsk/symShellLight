/*****************************************************************************************
* Implementacja pomocniczych funkcji symshella wsposob juz niezalezny od platformy.
* Napisane PRAWIE NIEOBIEKTOWO ale w C++
* Jest tu: print_width() , rect(), bar3D(), ...itp...
*****************************************************************************************/
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

typedef ssh_color wb_color;

const wb_color default_transparent=wb_color(-1);
const wb_color default_color=default_transparent;
const wb_color default_black=0;
const wb_color default_white=255;
const wb_color default_dark_gray=256+64;
const wb_color default_half_gray=256+128;
const wb_color default_light_gray=256+128+64;

extern int def_frame_width;//=1;//Domyslna grubosc ramki 
extern int def_cross_width;//=5;//Domyslna szerokosc krzyzyka
extern int def_scale_width;//=10;//Domyslna szerokosc skali
extern double def_arrow_size;//=15;   //Domyslna dlugosc grota strzalki
extern double def_arrow_theta;//=M_PI/6.0+M_PI; //Domyslne rozwarcie grota strzalki


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

//KOnfiguracja slupkow 3D. Zwraca poprzednia konfiguracje.
//Jesli parametr ==NULL to przywraca poprzednio zapamietana
const settings_bar3d* bar3d_config(settings_bar3d* st);

//Rysuje slupek 3D
void bar3d(int x,int y,int h,wb_color col1,wb_color col2);
void bar3dRGB(int x,int y,int h,int R,int G,int B,int ShadowDiv);

//Rysuje kwadratowa ramke
void rect(int x1,int y1,int x2,int y2,wb_color frame_c,int width=def_frame_width);

//Rysuje pionowa skale kolorow
void ver_scale(int x1,int y1,int width=def_scale_width,wb_color start=0,wb_color end=255);

//Rysuje pozioma skale kolorow
void hor_scale(int x1,int y1,int high=def_scale_width,wb_color start=0,wb_color end=255);

//Rysuje krzyzyk
void cross(int x,int y,wb_color color,int width=def_cross_width);

//Rysuje dowolna strzalke od punktu x1y1 do x2y2
void arrow(int x1,int y1,int x2,int y2,wb_color color,double size=def_arrow_size,double theta=def_arrow_theta);

//Efektywnie rysuje pozioma strzalke
void vert_arrow(int x1,int x2,int y,wb_color color,double size=def_arrow_size);

//Efektywnie rysuje pionowa strzalke
void hor_arrow(int x,int y1,int y2,wb_color  color,double size=def_arrow_size);

//Drukuje w obszarze nie wiekszym niz max_width. Zwraca width albo 0
//wewnetrzny bufor ma nie wiecej niz 1024 znaki
int print_width(int x,int y,int maxwidth,wb_color col,wb_color bcg,const char* format,...);

//Wyswietlanie pliku HTML poprzez system shell - tak naprawde to wszystkiego co moze wyswietlic przegladarka
extern "C" { int ViewHtml(const char* URL); }//From "symshell" but less or more independent

#endif


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/

