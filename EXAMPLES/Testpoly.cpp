//-/////////////////////////////////////////////////////////////////////////////////////////
/// @page page_e3_bars_and_polygons SYMSHELL TEST FOR 3D bars and polygons
/// @brief Program SYMSHELL'A rysujący słupki 3D i poligony na klikanie myszką
///
/// @section intro_sec_e3 Opis przykładu "3D BARS & POLYGONS"
/// Demonstracja użycia poligonów i pseudo 3D słupków. Klikanie myszą powoduje
/// cykliczne dodawanie wieloboków do listy. Funkcja replot() odrysowuje słupki
/// i poligony.
/// Pętla obsługi podobna do tej w 'testsyms.cpp', ale oczekuje na zdarzenia
/// i nie umożliwia pracy w tle (nie używa input_ready() )
/// @include Testpoly.cpp
///
/// @file
/// @brief Przykładowy program SYMSHELLA demonstrujący rysowanie słupków i wielokątów.
/// @date 2026-02-16 (last update)
//-/////////////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdio>
#include <cstdlib>

// BAR3D == column
// ---------------
const int a=10;
const int b=10;
const int c=6;
const int WHITE=255;
const int YELLO=254;

void column(int x, int y, int h, unsigned char col1, unsigned char col2)
{
    ssh_point romb[7];
                                           /*      6 -----  5    */
    romb[1].x= x;                          /*     /        / |   */
    romb[1].y= y - h;                      /*    1 ------ 2  |   */
    romb[2].x= x + a;                      /*    |        |  |   */
    romb[2].y= romb[1].y;                  /*    |        |  |   */
    romb[3].x= romb[2].x;                  /*    |        |  |   */
    romb[3].y= y;                          /*    |        |  4   */
    romb[4].x= x + a + b;                  /*    |        | /  c */
    romb[4].y= y - c;                      /*  x,y ------ 3      */
    romb[5].x= romb[4].x;                  /*         a      b   */
    romb[5].y= y - h - c;
    romb[6].x= x + b;
    romb[6].y= romb[5].y;

    fill_rect(x,y-h,x+a,y,col1);               /*front*/

    fill_poly(0,0,romb+1,6,col2);        /*bok i góra*/

    line(romb[6].x,romb[6].y,romb[5].x,romb[5].y,YELLO); /*góra tył*/

    line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,YELLO); /*góra prawa*/

    line(romb[1].x,romb[1].y,romb[6].x,romb[6].y,WHITE); /*góra lewa*/

    line(x,y,romb[1].x,romb[1].y,WHITE);       /*lewy pion*/

    line(romb[1].x,romb[1].y,romb[2].x,romb[2].y,WHITE); /* górny poziom */

    line(x,y,romb[3].x,romb[3].y,WHITE);       /*dolny poziom*/

    //plot(romb[5].x,romb[5].y,WHITE-1);

    line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,WHITE);

} /* end  column() */



/// Struct for clicked points.
struct polydata{
		ssh_point p;
        unsigned char color;
      };

/// Size of clicked point list.
const int numi=100;

/// Clicked point list.
polydata list[numi];

/// Redrawing fixed columns and pentagons at click locations.
void replot()
{
    static const ssh_point points[]={{-10,-10},{13,-13},{13,13},{-10,10},{-15,0}};
    static const int pnumber=sizeof(points)/sizeof(points[0]);
    unsigned int i;
    int old=mouse_activity(0);
    clear_screen();

    for(i=0;i<numi;i++)
    {
        if(list[i].p.x==0) break;
        //fill_circle(list[i].p.x, list[i].p.y,30,list[i].color);
        fill_poly(list[i].p.x, list[i].p.y,points,pnumber,list[i].color);
    }

    for(i=0;i<16;i++)
    {
        column(i * 12, 255, i * 10 + 10, i * 16, i * 16 - 8);
    }

    printbw(0,screen_height()-char_height('X'),"%s","ST:");
    printc(char_width('X')*3,screen_height()-char_height('X'),255,140,"POLYGON & BAR3D TEST");

    printbw(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");
    flush_plot();
    mouse_activity(old);
}

///  GENERAL MAIN FUNCTION.
//   **********************
int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0; //for use_mouse data
    int cont=1;//flaga kontynuacji
    int std=0;

    mouse_activity(1);
    set_background(128);
    buffering_setup(1); // WARNING! Animation enabled.
    shell_setup("3D BARS & POLYGONS SYMSHELL TEST ",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");

    /* RANDOMIZE();*/

    if(!init_plot(256,256,0,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont) //PĘTLA GŁÓWNA
    {
        delay_ms(10); //We limit the frequency of this almost empty loop!
        // Here it waits for events, so theoretically there is no
        // background work.
        // However, some implementations may have active waiting, or they may return
        // a lot of events that don't need handling, and in this example each event triggers
        // a flush_plot, which is expensive.
        int znak=get_char(); //printf("%c [%i]\n",znak,znak);

        switch(znak)
        {
        case '\0':
            //printf("(N)"); //Any event handled internally or requiring no handling
            break;
        case '@':
        case '\r':replot();break;
        case '\b':get_mouse_event(&xpos,&ypos,&click);
            list[i%numi].p.x=xpos;list[i%numi].p.y=ypos;
            list[i%numi].color=i%256;
            i++;
            replot();
            break;
        case 's':std=!std;break;
        case 'q':
        case EOF:
            cont=0;
            break;
        default: //Unsupported input
            printbw(0,100,"What??? %c [%i]  ",znak,znak);
            break;
        }

        if(std)
        {
            printf("stdout<<%c\n",znak);
            fflush(stdout);
        }

        printc(screen_width()-char_width('X'),
               screen_height()-char_height('X'),
               34,255,"%c",znak);

        flush_plot(); /// When there is an animation, flush plot costs CPU time,
                      /// even if there were no changes!
    }

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

