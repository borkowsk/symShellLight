///////////////////////////////////////////////////////////////////////////////////////////
///	                          Przykładowy program SYMSHELLA
///----------------------------------------------------------------------------------------
/// Demonstruje obsługę zdarzeń myszowych.
/// Zapamiętuje punkty kliknięć i potrafi odrysować po naciśnięciu ENTER
/// W tle miga napisem "Klikaj!"
///////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>

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
#include "wb_ptr.hpp"

using namespace std;
using namespace wbrtm;

#define KOLOR 128
#define WAIT 20000
const int max_points=1024;//Ile maksymalnie punktów może zapamiętywać?

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

int click_index=-1;//Początkowy stan indeksu

void replot() // Odrysowanie klikniętych punktów
{
    int i;
    for(i=0;i<click_index;i++)
    {
        plot(table[i].x,table[i].y,KOLOR);
    }
}

void read_mouse() //Przykładowa procedura obsługi myszy
{
    int xpos,ypos,click;/* dane z myszy */

    if(get_mouse_event(&xpos,&ypos,&click)!=-1)    /* Odczytuje ostatnie zdarzenie myszy */
    {
        if(click_index<max_points-1)
        {
            click_index++;
            printf(" Punkt %d ",click_index);
        }
        table[click_index].x=xpos;
        table[click_index].y=ypos;
        plot(xpos,ypos,KOLOR-100);
    }
}

/*  +- OGÓLNA FUNKCJA MAIN  */
/****************************/
int main(int argc,const char* argv[])
{
    unsigned step=0;
    unsigned xsize=320;
    unsigned ysize=200;
    mouse_activity(1);
    shell_setup(argv[0],argc,argv);//Nazwa pliku z programem jako tytuł okna!
    cout<<"TEST OBSŁUGI MYSZY W PRZENOŚNEJ POWŁOCE GRAFICZNEJ SYMSHELL'A:\n";
    cout<<" KOLORÓW= 256 q-quit\n";
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
                printbw(0,100,"What??? %c  ",pom);break;
            }
        }

        switch(step % WAIT)
        {
        case 0:	printc(0,0,128,255,"Klikaj!");break;
        case (WAIT/2):printc(0,0,255,28,"Klikaj!");break;
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
int WB_error_enter_before_clean=0;/* Zamyka okno nie czekając, albo czekając (1) na ENTER. */

/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
