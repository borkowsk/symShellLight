/* DUMMY IMPLEMENTATION FOR SYMSHELL MENU AND RELEATED FEATURES */
/****************************************************************/
#include "../sshmenuf.h"

int ssh_set_window_name(const char* WindowName)
//Ustala tekst nazwy okna w jego belce
{
	return 1;
	// SetWindowText return TRUE on success!
}

ssh_menu_handle ssh_main_menu()
// Daje uchwyt do głównego menu
{
	return (ssh_menu_handle)1;
}

ssh_menu_handle ssh_sub_menu(ssh_menu_handle hMenu, unsigned Position)
// Daje uchwyt do podmenu wg. pozycji
{
	return (ssh_menu_handle)2;
}

unsigned ssh_get_item_position(ssh_menu_handle hMenu, const char* ItemName)
// Odnajduje pozycje itemu w jakims menu
{
	return 3;
}

int ssh_menu_add_item(	ssh_menu_handle hMenu,
						const char* ItemName,
						unsigned Message,
						unsigned Flags)
// Dodaje item do menu
{
	return 4;
}

int ssh_menu_remove_item(
						ssh_menu_handle hMenu,
						unsigned ItemCommandOrPosition,
						unsigned asPosition)
// Usuwa item z menu
{
	return 1;
}

int ssh_menu_mark_item(	ssh_menu_handle hMenu,
						unsigned Check,
						unsigned ItemCommandOrPosition,
						unsigned asPosition)
// Ustawia lub usuwa marker przy itemie
{
	return 1;
}

int ssh_realize_menu(ssh_menu_handle hMenu)
// Zapewnia ze menu bedzie wygladac zgodnie z poprzednimi poleceniami
{
	return 1;
}


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                               (Don't change or remove this note) */
/********************************************************************/
