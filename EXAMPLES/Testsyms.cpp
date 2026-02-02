//-/////////////////////////////////////////////////////////////////////////////////////////
/// @page page_e5_replot SYMSHELL TEST WITH Replot
/// @brief Program SYMSHELL'A z dużą funkcją odrysowywania
///
/// @section intro_sec_e5 Opis przykładu "Użycie replot()"
/// Zawiera funkcje `replot()` odrysowująca główny wzorek oraz symulacje symulacji ;-)
/// w postaci rysowania rosnących kołek przerywanego akcjami użytkownika.
/// @include Testsyms.cpp
///
/// @file
/// @brief Przykładowy program SYMSHELL'A z funkcją "replot"
/// @date 2026-02-02 (last update)
//-/////////////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdlib>
#include <cstdio>

/// Repaint all screen - primitive but effective.
void replot()
{
    unsigned int i,j;
    unsigned char pom;
    int old=mouse_activity(0); //Possible mouse deactivation during redrawing
    clear_screen();

    // Redrawing the "bit-picture" content
    for(i=0;i<256U; i++)
        for(j=0;j<256U; j++)
        {
            pom=(unsigned char)( i&j );
            plot(j,i,pom);
        }

    //Drawing the rest of the scene figures
    printbw(0,screen_height()-char_height('X'),"%s","ST:");
    printc(char_width('X')*3,screen_height()-char_height('X'),1,140,"TEST ONLY");
    fill_circle(screen_width(),0,10,240);
    fill_circle(0,screen_height(),10,240);
    line(screen_width(),0,0,screen_height(),251);
    circle(screen_width()/2,0,25,200);
    fill_circle(128,128,30,140);
    printbw(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");

    flush_plot(); //End of drawing
    mouse_activity(old); //Possible reactivation of the mouse
}

///  GENERAL MAIN FUNCTION.
//   **********************
int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0; //Variables for reading mouse data
    int cont=1; //continue processing flag
    int std=0;

    //APPLICATION INITIATION
    mouse_activity(1);
    set_background(128);
    buffering_setup(0); //Animation can be turned off as it is not necessary here.
    shell_setup("SYMSHELL TEST WITH Replot",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");

    if(!init_plot(256,256,0,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont) //PĘTLA GŁÓWNA
    {
        int tab;

        if(input_ready()) //Are there any input events to be processed?
        {
            tab=get_char(); //There's an entrance. It reads. It shouldn't stop here because availability was checked earlier.
            switch(tab)
            {
            case '\0': /* do nothing */ break;
            case '@':					//Manual spell to fire replot()
            case '\r':replot();break;	//Systematic code to fire of replot()
            case '\b':get_mouse_event(&xpos,&ypos,&click); //Handling a mouse event
                if(click&0x1)
                    set_char('X');
                break;
            case 's':std=!std;break;	//Echo on console on/off
            case 'q':					//Manual code at application termination
            case EOF:					//System application termination (Close in MSWin, Ctrl-C in X11 or closing the window)
                cont=0;
                break;
            default:
                break;
            }

            i=0; //Resetting the circle radius
            if(std) //Implementing command echoing on the console
            {
                printf("stdout<<%c\n",tab);
                fflush(stdout);
            }

            printc(screen_width()-char_width('X'),
                   screen_height()-char_height('X'),
                   34,255,"%c",tab);
        }

        //Let's keep doing our thing!!!
        fill_circle(xpos,ypos,(i%256+2)/2,i%256);
        i++;
        flush_plot();
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
