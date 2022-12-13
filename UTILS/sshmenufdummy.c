/** DUMMY IMPLEMENTATION FOR SYMSHELL MENU AND RELATED FEATURES
(wymagane dla prawidłowego linkowania pod X11, gdzie teraz nie ma menu)
************************************************************************ */

#include "../sshmenuf.h"

/// Ustala tekst nazwy okna w jego belce
/// \param WindowName
/// \return TRUE - success!
/// \note Wygląda że stało się redundantne z nowszą funkcją set_title()
int ssh_set_window_name(const char* WindowName)
{
	return 0;
}

/// Daje uchwyt do głównego menu
/// \return HANDLE
ssh_menu_handle ssh_main_menu()
{
	return (ssh_menu_handle)1;
}

/// Daje uchwyt do podmenu wg. pozycji
/// \param hMenu
/// \param Position
/// \return HANDLE
ssh_menu_handle ssh_sub_menu(ssh_menu_handle hMenu, unsigned Position)
{
	return (ssh_menu_handle)2;
}

/// Odnajduje pozycje itemu w jakims menu
/// \param hMenu
/// \param ItemName
/// \return item position
unsigned ssh_get_item_position(ssh_menu_handle hMenu, const char* ItemName)
{
	return 3;//3?
}

/// Dodaje item do menu
/// \param hMenu
/// \param ItemName
/// \param Message
/// \param Flags
/// \return ???
int ssh_menu_add_item(	ssh_menu_handle hMenu,
						const char* ItemName,
						unsigned Message,
						unsigned Flags)
{
	return 4;
}

/// Usuwa item z menu
/// \param hMenu
/// \param ItemCommandOrPosition
/// \param asPosition
/// \return ???
int ssh_menu_remove_item(
						ssh_menu_handle hMenu,
						unsigned ItemCommandOrPosition,
						unsigned asPosition)
{
	return 1;
}

/// Ustawia lub usuwa marker przy itemie
/// \param hMenu
/// \param Check
/// \param ItemCommandOrPosition
/// \param asPosition
/// \return
int ssh_menu_mark_item(	ssh_menu_handle hMenu,
						unsigned Check,
						unsigned ItemCommandOrPosition,
						unsigned asPosition)
{
	return 1;
}

/// Zapewnia, że menu będzie wyglądać zgodnie z poprzednimi poleceniami
/// \param hMenu
/// \return
int ssh_realize_menu(ssh_menu_handle hMenu)
{
	return 1;
}

/* *******************************************************************/
/*              SYMSHELLLIGHT  version 2022-12-13                    */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
