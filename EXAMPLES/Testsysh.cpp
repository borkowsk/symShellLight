//-/////////////////////////////////////////////////////////////////////////////////////////
/// @page page_e1_simplest VERY SIMPLE TEST
/// @brief Najprostszy program SYMSHELL'A
///
/// @section intro_sec_e1 Opis przykładu "Kolory i litery"
/// Bardzo prosty test bez funkcji odtwarzania ekranu. Następuje odtwarzanie z bitmapy
/// , ale po zmianie rozmiaru okna ekran będzie czarny!
/// @include Testsysh.cpp
///
/// @file
/// @brief Kolory i litery
/// @date 2026-02-02 (last update)
//-/////////////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdio>

///  SIMPLE MAIN FUNCTION.
//   *********************
int main(int argc,const char* argv[])
{
    buffering_setup(1); //It has to remember the screen/window content in a bitmap
    shell_setup("VERY SIMPLE TEST",argc,argv);

    init_plot(16,16,16,16); //Window for 16x16 pixels + 16x16 characters

    for(int i=0;i<16;i++)
        for(int j=0;j<16;j++)
        {
            plot(j,i,i*16+j);
            char c=i*16+j;
            printbw(16+j*char_width('W'),16+i*char_height('W'),"%c",c);
        }

    flush_plot(); //Picture is ready

    while(get_char()!=EOF); //It just waits here, ignoring any possible events.

    close_plot(); //The fun's over! We're closing the window.
    printf("Do widzenia!\n");
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

