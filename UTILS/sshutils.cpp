/*****************************************************************************************
* Implementacja pomocniczych funkcji Symshell'a w sposób juz niezależny od platformy.
* Napisane NIEOBIEKTOWO, ale w C++
* Jest tu: print_width() , rect(), bar3D(), cross(), *_arrow() ...itp...
*****************************************************************************************/
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <cmath>
#include <ctype.h>
//#include "INCLUDE/wbminmax.hpp"

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#include "symshell.h"
#include "sshutils.hpp"

using namespace std;

/// Pomocnicza funkcja kwadratowa dla elegancji kodu
inline int sqr(int x)
{
    return x*x;
}

/// Funkcja ta drukuje w obszarze nie większym niz 'maxwidth' z justowaniem
/// Sekwencje określające justowanie muszą być na początku i są to:
/// %@L - left , %@R - right oraz %@C - center.
/// Wewnętrzny bufor ma nie więcej niz 1024 znaki.
/// \param x : pozioma współrzędna punktu startu
/// \param y : pionowa współrzędna punktu startu
/// \param maxwidth : maksymalna dostępna szerokość
/// \param col : kolor tekstu
/// \param bcg : kolor tła
/// \param format : jak zwykle w print, ale może mieć na początku sekwencje określający justowanie
/// \param ... : zmienne parametry wynikające z formatu
/// \return Zwraca użyte 'width' albo 0
int print_width(int x,int y,int maxwidth,wb_color col,wb_color bcg,const char* format ...)
{
    const size_t BUFSIZE=1024;
    char bufor[BUFSIZE];
    char  yust='L';
    int   width=0;//Skalkulowana szerokość tekstu

if(format[0]=='%' && format[1]=='@')//Został podany sposób justowania
	{
	yust=toupper(format[2]);
	format+=3;
	}
if(strchr(format,'%')!=nullptr)//Są jakieś znaki formatujące
	{
	va_list list;
    va_start(list,format);//??? undeclared identifier?
	if(vsprintf(bufor,format,list)>=BUFSIZE)
		{
		errno=ENOMEM;
		perror("print_width internal bufor exceed");
		}
	}
	else
	{
	strcpy(bufor,format);	
	}

while( (width=string_width(bufor)) >maxwidth ) // Gdy za mało miejsca
		{
		int size=strlen(bufor);
		if(size==1) break;
		bufor[size-2]='*';
		bufor[size-1]='\0';
		}
//rect(x,y,x+maxwidth,y+string_height(bufor),128);//DEBUG
if(width<=maxwidth)
	{
	switch(yust){
	case 'L':printc(x,y,col,bcg,"%s",bufor);break;
	case 'R':printc((x+maxwidth)-width,y,col,bcg,"%s",bufor);break;
	case 'C':printc(x+maxwidth/2-width/2-1,y,col,bcg,"%s",bufor);break;
	default:
		;	}
	return width;
	}
	else
	return 0;//Nie ma miejsca nawet na gwiazdkę
}

/// Domyślna grubość ramki
int def_frame_width=1;

/// Rysuje kwadratowa ramkę o zadanej grubości
/// \param x1
/// \param y1
/// \param x2
/// \param y2
/// \param frame_c
/// \param width
void rect(int x1,int y1,int x2,int y2,wb_color frame_c,int width)
{
    if(width<=1)
    {
	line(x1,y1,x2,y1,frame_c);//--->
	line(x2,y1,x2,y2,frame_c);//vvv
	line(x1,y2,x2,y2,frame_c);//<---
	line(x1,y1,x1,y2,frame_c);//^^^
    }
    else
    {
        assert("NOT TESTED CODE in rect()"==nullptr);
    fill_rect(x1,y1,x2,y1+width,frame_c);//--->
    fill_rect(x2,y1,x2+width,y2,frame_c);//vvv
    fill_rect(x1,y2,x2,y2+width,frame_c);//<---
    fill_rect(x1,y1,x1+width,y2,frame_c);//^^^
    }
}

/// Stałe parametry konfiguracji słupka: s-aktualny old-poprzedni
static settings_bar3d s,old;

/// Konfiguracja słupków 3D. Zwraca poprzednią konfiguracje.
/// Jeśli parametr == NULL to przywraca poprzednio zapamiętaną w 'old'
/// \param st
/// \return
const settings_bar3d* bar3d_config(settings_bar3d* st)
{
if(st!=nullptr)
	{
	old=s;//Zapamiętuje poprzednia
	s=*st;
	}
	else
	s=old;//Przywraca poprzednia
return &old;
}

/// Rysuje słupek 3D w kolorach indeksowanych
/// \param x  : pozioma współrzędna lewego dolnego rogu frontu
/// \param y  : pionowa współrzędna lewego dolnego rogu frontu
/// \param h  : wysokość frontu
/// \param col1 : indeks koloru przodu
/// \param col2 : indeks koloru boku
void bar3d(int x,int y,int h,wb_color col1,wb_color col2)
{
	ssh_point romb[7];
	wb_color wire_col=s.wire;
	if(wire_col==col1)
		wire_col=(wire_col+s.back)/2;
	if(wire_col==col2)
		wire_col=(wire_col+s.back)/2;
										   /*       6 -----  5   */
	romb[1].x= x;                          /*     /        / |   */
	romb[1].y= y - h;                      /*    1 ------ 2  |   */
	romb[2].x= x + s.a;                    /*    |        |  |   */
	romb[2].y= romb[1].y;                  /*    |        |  |   */
	romb[3].x= romb[2].x;                  /*    |        |  |   */
	romb[3].y= y;                          /*    |        |  4   */
	romb[4].x= x + s.a + s.b;              /*    |        | /  c */
	romb[4].y= y - s.c;                    /*  x,y ------ 3      */
	romb[5].x= romb[4].x;                  /*         a      b   */
	romb[5].y= y - h - s.c;
	romb[6].x= x + s.b;
	romb[6].y= romb[5].y;

	fill_rect(x,y-h,x+s.a,y,col1);               //front

	fill_poly(0,0,romb+1,6,col2);              //bok i gora

	fill_rect(romb[1].x,romb[1].y,romb[2].x+1,romb[2].y+1,wire_col);//górny poziom
	fill_rect(x,y,romb[3].x+1,romb[3].y+1,wire_col);       //dolny poziom

	line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,wire_col); //blik?

	//plot(romb[5].x,romb[5].y,wire_col-1);
	line(romb[1].x,romb[1].y,romb[6].x,romb[6].y,wire_col);//lewy ukos
	line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,wire_col);//prawy ukos
	line(romb[3].x,romb[3].y,romb[4].x,romb[4].y,wire_col);//dolny ukos
	line(romb[4].x,romb[4].y,romb[5].x,romb[5].y,wire_col);//tyl bok
	line(romb[5].x,romb[5].y,romb[6].x,romb[6].y,wire_col);//tyl bok

	fill_rect(x,y-h,x+1,y+1,wire_col);       //lewy pion

} /* end of bar3d */

/// Rysuje słupek 3D w kolorze RBG z cieniem
/// \param x  : pozioma współrzędna lewego dolnego rogu frontu
/// \param y  : pionowa współrzędna lewego dolnego rogu frontu
/// \param h  : wysokość frontu
/// \param R  : składowa koloru 'red'
/// \param G  : składowa koloru 'green'
/// \param B  : składowa koloru 'blue'
/// \param Shad : dzielnik do RGB żeby uzyskać cień na bocznej ścianie
void bar3dRGB(int x,int y,int h,int R,int G,int B,int Shad)
{
	ssh_point romb[7];
	wb_color wire_col=s.wire;

										   /*       6 -----  5   */
	romb[1].x= x;                          /*     /        / |   */
	romb[1].y= y - h;                      /*    1 ------ 2  |   */
	romb[2].x= x + s.a;                    /*    |        |  |   */
	romb[2].y= romb[1].y;                  /*    |        |  |   */
	romb[3].x= romb[2].x;                  /*    |        |  |   */
	romb[3].y= y;                          /*    |        |  4   */
	romb[4].x= x + s.a + s.b;              /*    |        | /  c */
	romb[4].y= y - s.c;                    /*  x,y ------ 3      */
	romb[5].x= romb[4].x;                  /*         a      b   */
	romb[5].y= y - h - s.c;
	romb[6].x= x + s.b;
	romb[6].y= romb[5].y;

	set_brush_rgb(R,G,B);
	fill_rect_d(x,y-h,x+s.a,y);               //front

	set_brush_rgb(R/Shad,G/Shad,B/Shad);
	fill_poly_d(0,0,romb+1,6);              //bok i gora

	fill_rect(romb[1].x,romb[1].y,romb[2].x+1,romb[2].y+1,wire_col);//górny poziom
	fill_rect(x,y,romb[3].x+1,romb[3].y+1,wire_col);       //dolny poziom

	line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,wire_col); //blik?

	//plot(romb[5].x,romb[5].y,wire_col-1);
	line(romb[1].x,romb[1].y,romb[6].x,romb[6].y,wire_col);//lewy ukos
	line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,wire_col);//prawy ukos
	line(romb[3].x,romb[3].y,romb[4].x,romb[4].y,wire_col);//dolny ukos
	line(romb[4].x,romb[4].y,romb[5].x,romb[5].y,wire_col);//tyl bok
	line(romb[5].x,romb[5].y,romb[6].x,romb[6].y,wire_col);//tyl bok

	fill_rect(x,y-h,x+1,y+1,wire_col);       //lewy pion
}/* end of bar3dRGB */

/// Domyślna szerokość krzyżyka
int def_cross_width=5;

/// Rysuje krzyżyk
/// \param x
/// \param y
/// \param color
/// \param width
void cross(int x,int y,wb_color color,int width)
{
	line(x-width,y,x+width,y,color);
	line(x,y-width,x,y+width,color);
}

/// Rysuje pionową skalę kolorów
/// \param x1
/// \param y1
/// \param width
/// \param start
/// \param end
void ver_scale(int x1,int y1,int width,wb_color start,wb_color end)
{
	for(wb_color i=start;i<=end;i++)
	{
		line(x1,y1+i,x1+width,y1+i,i);
	}
}

/// Stałe parametry strzałek - '*size' grota i '*theta'-jego rozwarcie
double def_arrow_size=15,def_arrow_theta=M_PI/6.0+M_PI;//3.6651914291881

/// Rysuje dowolnie skierowaną strzałkę od punktu x1y1 do x2y2
/// \param x1
/// \param y1
/// \param x2
/// \param y2
/// \param color : indeks koloru strzałki
/// \param size  : długość strzałki
/// \param theta : kierunek strzałki
void arrow(int x1,int y1,int x2,int y2,wb_color color,double size,double theta)
{	
	//METODA LICZENIA Z OBRACANIA OSI STRZAŁKI
	double A=(size>=1?size:size*sqrt((float)(sqr(x1-x2)+sqr(y1-y2))));
	double poY=double(y2-y1);
	double poX=double(x2-x1);
	if(poY==0 && poX==0)
	{
		//Rzadki błąd, ale DOMAIN ERROR!
		cross((int)x1,int(y1),color,(int)(def_arrow_size/2));
		circle((int)(x1+def_arrow_size/sqrt(2.0)),(int)(y1-def_arrow_size/sqrt(2.0)+1),(ssh_natural)(def_arrow_size),color);
		return;	
	}
	
	double alfa=atan2(poY,poX);             assert(fabs(alfa)<=M_PI);
	double xo1=A*cos(theta+alfa);
	double yo1=A*sin(theta+alfa);
	double xo2=A*cos(alfa-theta);
	double yo2=A*sin(alfa-theta);
	//cross(x2,y2,color);DEBUG

	line(int(x2+xo1),int(y2+yo1),x2,y2,color);
	line(int(x2+xo2),int(y2+yo2),x2,y2,color);
	line(x1,y1,x2,y2,color);
}
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


