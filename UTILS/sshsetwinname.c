/// @file
/// @brief Dummy implementation of the obsolete function ssh_set_window_name
/// @date 2026-09-25 (modified)

#include "sshmenuf.h"
#include "maybe_unused.h"

// Pełno tutaj nieużywanych parametrów.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedParameter"

// Ustala tekst nazwy okna w jego belce
// \param WindowName - nowa nazwa.
// \return TRUE - success!
// \note Wygląda, że stało się redundantne z nowszą funkcją set_title()
//MAYBE_UNUSED
int ssh_set_window_name(const char* WindowName)
{
    return 0;
}

#pragma clang diagnostic pop
/* *******************************************************************/
/*                 SYMSHELLLIGHT  version 2026                       */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/

