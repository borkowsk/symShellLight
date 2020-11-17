///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
//Demonstruje skalowanie okna niezalezne od aplikacyjnej strony programu
//i robione w funkcjach symshella. Petla oblsugi zdarzen bez pracy w tle
///////////////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <cstdio>
#include <cstdlib>

int x,y,vx,vy;

/* For close_plot() */
int WB_error_enter_before_clean=0;/* Nie czeka z zamykaniem okna */

void replot()
{
    unsigned int i,j;
    unsigned char pom;
    int old=mouse_activity(0);
    clear_screen();
    for(i=0;i<16U; i++)
        for(j=0;j<16U; j++)
        {
            pom=(unsigned char)(16*i+j);
            plot(j,i,pom);
        }
    printbw(0,16,"%s","PALETA");
    flush_plot();
    mouse_activity(old);
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0;//Myszowate
    int cont=1;//flaga kontynuacji
    int std=0;

    mouse_activity(0);/*Mysz niepotrzebna */
    set_background(128);
    buffering_setup(0);/* Wylaczona animacja - tu niepotrzebna */
    shell_setup("SYMSHELL - PALETA",argc,argv);
    printf("COLORS= 256 q-quit s-swich stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");
    /*
RANDOMIZE();
*/
    if(!init_plot(16,16,1,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont)
    {
        int inpt;
        // Czekaj na wejscie
        inpt=get_char();
        switch(inpt)
        {
        case '@':
        case '\r':replot();break;
        case '\b':get_mouse_event(&xpos,&ypos,&click);break;
        case 's':std=!std;break;
        case 'q':
        case EOF:
            cont=0;
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

    close_plot();
    printf("Do widzenia!!!\n");
    return 0;
}
/********************************************************************/
/*              SYMSHELLLIGHT  version 2020-11-16                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
