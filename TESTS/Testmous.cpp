///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
//Demonstruje obsluge zdarzen myszowych. Zapamientuje punkty klikniec i potrafi odrysowac
//W tle miga napisem "CZEKAM"
//POD Dev-Cpp potrzebne s¹ dwie bibloteki:
//".../Dev-Cpp/lib/libgdi32.a"
//"...Dev-Cpp/lib/libcomdlg32.a"
///////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "../symshell.h"
#include "../wb_ptr.hpp"
using namespace std;
using namespace wbrtm;

#define KOLOR 128
#define WAIT 20000
const int max_points=1024;//Ile maksymalnie punktow zapamientywac

struct point
{
int x,y;
friend
ostream& operator << (ostream& o,const point& p)
	{return o<<' '<<p.x<<' '<<p.y;}
friend
istream& operator >> (istream& i,point& p)
	{return i>>p.x>>p.y;}
};

wb_dynarray< point >  table(max_points);

int index=-1;//Poczatkowy stan indeksu

void replot()
{
int i;
for(i=0;i<index;i++)
	{
	plot(table[i].x,table[i].y,KOLOR);
	}
}

void read_mouse()
//Przykladowa procedura obslugi myszy
{
int xpos,ypos,click;
/* Odczytuje ostatnie zdazenie myszy */
if(get_mouse_event(&xpos,&ypos,&click)!=-1)
	{
	if(index<max_points-1)
	{
		index++;
		printf(" Punkt %d ",index);
    }
	table[index].x=xpos;
	table[index].y=ypos;
	plot(xpos,ypos,KOLOR-100);
	}
}

/*  +- OGOLNA FUNKCJA MAIN */
/************************/
main(int argc,char* argv[])
{
unsigned step=0;
unsigned xsize=320;
unsigned ysize=200;
mouse_activity(1);
shell_setup(argv[0],argc,argv);//Nazwa pliku z programem jako tytul okna!
cout<<"TEST OBSLUGI MYSZY W PRZENOSNEJ POWLOCE GRAFICZNEJ:\n";
cout<<" COLOROW= 256 q-quit\n";
cout.flush();
init_plot(xsize,ysize,0,0);

while(1)
{ //printf
int pom;

if(input_ready())
  {
  pom=get_char();//printf("What??? %c  ",pom);
  switch(pom)
  {
  case '\r': replot();break;
  case '\b': //print(0,100,"Mouse");
             read_mouse();break;
  case EOF:
  case 'q':goto END;
  default:
	print(0,100,"What??? %c  ",pom);break;
  }
}

switch(step%WAIT)
{
case 0:	printc(0,0,128,255,"Czekam!");break;
case (WAIT/2):printc(0,0,255,28,"Czekam!");break;
}

step++;
flush_plot();
}

END:
;
//system("PAUSE");
close_plot();
return 0;
}

/* For close_plot() */
int WB_error_enter_before_clean=0;/* Zamyka okno nie czekajac */
/********************************************************************/
/*		TEST CODE FOR WBRTM  light version 2009                     */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
