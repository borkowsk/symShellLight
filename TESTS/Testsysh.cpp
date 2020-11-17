///////////////////////////////////////////////////////////////////////////////////////////
//                          Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
// Bardzo prosty test bez funkcji odtwarzania ekranu. Nastepuje odtwarzanie z bitmapy
// ale po zmianie rozmiaru okna ekran bedzie czarny!
///////////////////////////////////////////////////////////////////////////////////////////
#include "../symshell.h"
#include <stdio.h>

int main(int argc,const char* argv[])
{
    buffering_setup(1);/* Pamieta w bitmapie zawartosc ekranu */
    shell_setup("VERY SIMPLE TEST",argc,argv);

    init_plot(16,16,16,16);//16x16 pixeli + 16x16 znakow

    for(int i=0;i<16;i++)
        for(int j=0;j<16;j++)
        {
            plot(j,i,i*16+j);
            char c=i*16+j;
            printbw(16+j*char_width('W'),16+i*char_height('W'),"%c",c);
        }

    flush_plot();	/* Bitmapa gotowa */

    while(get_char()!=EOF);	/* Tu czeka i ignoruje ewentualne zdarzenia */

    close_plot();/* Po zabawie */
    printf("Do widzenia!\n");
    return 0;
}

/* For close_plot() */
int WB_error_enter_before_clean=0;

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
