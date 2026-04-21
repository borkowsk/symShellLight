/** @file
 *  @brief Atrapowa implementacja menu SYMSHELL i powiązanych funkcji. */
/*         =========================================================== */
/** @date 2026-04-21 (ostatnia modyfikacja)
 *  @details Rodzaj "zaślepki", która pozwala na likowanie i w bezpieczny
 *           sposób nic nie robi. Wymagane dla prawidłowego linkowania
 *           pod X11 i SVG, gdzie teraz nie ma implementacji menu.
 * ******************************************************************** */

#include "sshmenuf.h"
#include "maybe_unused.h"

// Pełno tutaj nieużywanych parametrów.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedParameter"

// Ustala tekst nazwy okna w jego belce
// \param WindowName - nowa nazwa.
// \return TRUE - success!
// \note Wygląda, że stało się redundantne z nowszą funkcją set_title()
MAYBE_UNUSED
int ssh_set_window_name(const char* WindowName)
{
    return 0;
}

// Normalnie tworzy/koduje uchwyt do głównego menu, który powinien być raczej niezerowy.
// Zatem zero wskazuje, że nie należy tego używać.
// \return Cokolwiek zwraca, to raczej 0 jest wartością niepoprawną!
MAYBE_UNUSED
ssh_menu_handle ssh_main_menu()
{
    return (ssh_menu_handle)0;
}

// Daje uchwyt do podmenu wg. pozycji
MAYBE_UNUSED
ssh_menu_handle ssh_sub_menu(ssh_menu_handle hMenu, unsigned Position)
{
    return (ssh_menu_handle)2;
}

// Odnajduje pozycje itemu w jakimś menu
MAYBE_UNUSED
unsigned ssh_get_item_position(ssh_menu_handle hMenu, const char* ItemName)
{
    return 3;// dlaczego 3?
}

// Dodaje item do menu
MAYBE_UNUSED
int ssh_menu_add_item(	ssh_menu_handle hMenu,
                        const char* ItemName,
                        unsigned Message,
                        unsigned Flags)
{
    return 4;
}

// Usuwa item z menu
MAYBE_UNUSED
int ssh_menu_remove_item(
                        ssh_menu_handle hMenu,
                        unsigned ItemCommandOrPosition,
                        unsigned asPosition)
{
    return 1;
}

// Ustawia lub usuwa marker przy item-ie
MAYBE_UNUSED
int ssh_menu_mark_item(	ssh_menu_handle hMenu,
                        unsigned Check,
                        unsigned ItemCommandOrPosition,
                        unsigned asPosition)
{
    return 1;
}

// Zapewnia, że menu będzie wyglądać zgodnie z poprzednimi poleceniami
MAYBE_UNUSED
int ssh_realize_menu(ssh_menu_handle hMenu)
{
    return 1;
}

char*			ssh_menu_options=""; //"-fg black -bg gray";?

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

