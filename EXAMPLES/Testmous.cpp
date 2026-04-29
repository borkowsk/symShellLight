/// @file
/// @brief A sample SYMSHELLA program demonstrating mouse support/
///        Przykładowy program SYMSHELLA demonstrujący obsługę myszy.
/// @date 2026-04-29 (last update)
//-/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @page page_e4_mouse SYMSHELL TEST FOR Mouse
/// @brief SYMSHELL'S program that handles mouse clicking/Program SYMSHELL'A obsługujący klikanie myszką.
///
/// @section intro_sec_e4 Description of the "Using the Mouse" example/Opis przykładu "Użycie myszy"
///
/// EN: Demonstrates mouse event handling. Remembers click points and can redraw them after pressing ENTER.
/// Flashes "Click!" in the background.
///
/// PL: Demonstruje obsługę zdarzeń myszowych. Zapamiętuje punkty kliknięć i potrafi je odrysować po naciśnięciu ENTER
/// W tle miga napisem "Click!"
/// @include Testmous.cpp
//-/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#define COLOR 128          ///< The color of the "Click!"
#define WAIT 1000000       ///< After how many turns of the main loop should the text color be changed.
const int max_points=1024; ///< How many maximum click points can it remember?

/// A struct representing clicked points.
struct xy_point
{
    int x,y;

    friend
    ostream& operator << (ostream& o,const xy_point& p)
    { return o<<' '<<p.x<<' '<<p.y; }
    friend
    istream& operator >> (istream& i,xy_point& p)
    { return i>>p.x>>p.y; }
};

wb_dynarray< xy_point >  table(max_points); ///< Clicked points. A simple array with dynamic size, set at creation time.

int click_index=-1; ///< Initial index state before the array

/// On request, it redraws previously clicked points
void replot()
{
    int i;
    for(i=0;i<click_index;i++)
    {
        plot(table[i].x, table[i].y, COLOR);
    }
}

/// Mouse handling procedure example.
void read_mouse()
{
    int xpos,ypos,click; //for use_mouse data

    if(get_mouse_event(&xpos,&ypos,&click)!=-1)   //Reads the last use_mouse event.
    {
        if(click_index<max_points-1)
        {
            click_index++;
            printf(" Point %d ",click_index);
        }
        table[click_index].x=xpos;
        table[click_index].y=ypos;
        plot(xpos, ypos, COLOR - 100);
    }
}

///  GENERAL MAIN FUNCTION.
//   **********************
int main(int argc,const char* argv[])
{
    unsigned step=0;
    unsigned x_size=320;
    unsigned y_size=200;
    mouse_activity(1);
    shell_setup("MOUSE TEST",argc,argv); //Program file name used as window title!
    cout<<"MOUSE OPERATION TEST IN SYMSHELL`S PORTABLE GRAPHIC SHELL:\n q-quit\n";
    cout.flush();

    init_plot(x_size, y_size, 0, 0); //No extra space for text in the margins

    while(1)
    {
        int pom;

        if(input_ready()) //Is there an event to handle?
        {
            pom=get_char();
            //printf("What??? %c  ",pom);
            switch(pom)
            {
            case '\0':
                printf("(N)"); //Any event handled internally or requiring no handling
                break;
            case '\r': replot();break;
            case '\b': //print(0,100,"Mouse");
                read_mouse();
                break;
            case 'D':
                dump_screen("symshell_dump");
                break;
            case EOF:
            case 'q':goto END;
            default: //Unsupported input
                printbw(0,100,"What??? %c  ",pom);
                break;
            }
        }

        switch(step % WAIT)
        {
        case 0:	printc(5,0,128,255,"Click! Click!!!");break;
        case (WAIT/2):printc(5,0,255,28,"Click! Click!!!");break;
        }

        step++;
        flush_plot();
    }

END: //system("PAUSE"); //Instead of WB_error_enter_before_clean=1
    //Terminating this application
    close_plot();
    printf("Do widzenia!!! / Goodbye!!!\n");
    return 0;
}

/// For close_plot() - maybe also linked from the library with default value "0".
int WB_error_enter_before_clean=0;

/* **************************************************************** */
/*                  SYMSHELLLIGHT  version 2026                     */
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
