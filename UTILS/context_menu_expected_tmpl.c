/// @file
/// @brief Simple demo of `ssh_context_menu_expected` user defined function.
//         =================================================================
/// @date 2026-04-21 (last modification)
//  Created by borkowsk on 16.04.26.

#include <stdio.h>
#include "symshell.h"


long long ssh_context_menu_expected(unsigned x, unsigned y, struct ssh_basic_win_place_context* other_data)
{
/**
 * @internal
 *    PL: Funkcja może być blokująca lub nieblokująca (np. odpalać osobny wątek). Podstawową implementację dostarcza
 *        biblioteka SYMSHELL, ale zdefiniowanie własnej przez użytkownika biblioteki blokuje linkowanie wersji domyślnej.
 *    EN: A function can be blocking or non-blocking (e.g., launching a separate thread). The basic implementation
 *        is provided by the SYMSHELL library, but defining your own one blocks linking of the default version.
 */
    fprintf(stderr,"Dummy `ssh_context_menu_expected(%d,%d)` is called...\n",x,y);
    if(other_data!=NULL)
    {
        fprintf(stderr,"Dummy `ssh_basic_win_place_context`:\nD:\t%lld\nW:\t%lld\nX:\t%u\nY:\t%u\n",
                other_data->ScrIdentifier,
                other_data->WinIdentifier,
                other_data->X,
                other_data->Y
                );
        fflush(stderr);
    }

    return -1; /* -1 to OBSŁUGA ZANIECHANA! Poślij dane domyślnej obsłudze `\b`/
        * -1 means RESIGNATION FROM SERVICE! Send data to the default handler `\b` */
}

/* *******************************************************************/
/*                   SYMSHELLLIGHT  version 2026                     */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
