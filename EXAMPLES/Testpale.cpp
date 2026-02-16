//-/////////////////////////////////////////////////////////////////////////////////////////
/// @page page_e2_paleta SYMSHELL - RESKALOWALNA PALETA
/// @brief Program SYMSHELL'A z automatycznym reskalowaniem i funkcją odrysowywania
///
/// @section intro_sec_e2 Opis przykładu "Reskalowalna paleta"
/// Demonstruje skalowanie okna niezależne od aplikacyjnej strony programu
/// i robione w funkcjach symshell'a. Pętla obsługi zdarzeń bez pracy w tle
/// , czyli oczekuje na zdarzenia.
/// @include Testpale.cpp
///
/// @file
/// @brief Przykładowy program SYMSHELL'A z reskalowaniem
/// @date 2026-02-16 (last update)
//-/////////////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdio>
#include <cstdlib>

///On demand, it redraws the entire image - primitive but effective.
void replot()
{
    unsigned int i,j;
    ssh_color pom;
    int old=mouse_activity(0);
    clear_screen();

    for(i=0;i<16U; i++)
        for(j=0;j<16U; j++)
        {
            pom=(ssh_color)(16*i+j);
            plot(j,i,pom);
        }

    printbw(0,16,"%s","PALETA");
    flush_plot();
    mouse_activity(old);
}

///  GENERAL MAIN FUNCTION.
//   **********************
int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0; //Variables for reading use_mouse data
    int cont=1; //continue processing flag
    int std=0;

    mouse_activity(0); //We don't need the use_mouse now, so we deactivate it
    set_background(128);
    buffering_setup(0); //We don't need the animation, so we deactivate it
    shell_setup("SYMSHELL - RESKALOWALNA PALETA ",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");

    if(!init_plot(16,16,6,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont) //MAIN LOOP
    {
        int inpt;
        inpt=get_char(); // Wait for any input!
        switch(inpt)
        {
        case '\0':
                printf("(N)"); //Any event handled internally or requiring no handling
            break;
        case '@':
        case '\r':replot();break;
        case '\b':get_mouse_event(&xpos,&ypos,&click);break;
        case 's':std=!std;break;
        case 'q':
        case EOF:
            cont=0;
            break;
        default:
            break;
        }

        if(std)
        {
            printf("stdout<<%c\n",inpt);
            fflush(stdout);
        }

        printc(16,16, 34,255,"%c",inpt);
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

