/* IMPLEMENTATION FOR SYMSHELL MENU AND RELEATED FEATURES */
/** \date  2022 - 12 - 30 (last modification)             */
/**********************************************************/
//#include "platform.h"

#if defined(_MSC_VER) || defined(__MSWINDOWS__) 
#include <windows.h> //bo Menu itp
#include <assert.h>
#include "sshmenuf.h"

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
//TYMCZASEM - OSTRZE¯ENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#ifdef __cplusplus
extern "C" 
#endif

extern HWND	WB_Hwnd;//W symshwin.c

/// Ustala tekst nazwy okna w jego belce
/// \param WindowName
/// \return TRUE - success!
/// \note Wygl¹da ¿e sta³o siê redundantne z nowsz¹ funkcj¹ set_title()
int ssh_set_window_name(const char* WindowName)
{
	return SetWindowText(WB_Hwnd, WindowName);
	// SetWindowText return TRUE on success!
}

ssh_menu_handle ssh_main_menu()
{
	return	GetMenu(WB_Hwnd);
}

ssh_menu_handle ssh_sub_menu(
					ssh_menu_handle hMenu,				
					unsigned    Position)
{										assert(hMenu != 0);
	return GetSubMenu((HMENU)hMenu,Position);
}

unsigned ssh_get_item_position(
					 ssh_menu_handle hMenu,
					 const char* ItemName)
{										assert(hMenu != 0);
	size_t i,len=strlen(ItemName);
	char* pom=malloc(len+1);
	int N=GetMenuItemCount(hMenu);
	
	for(i=0;i<N;i++)
	{
		int ret=GetMenuString(hMenu,i,pom,len+1,MF_BYPOSITION);
									        assert(ret!=0);
		if(strcmp(ItemName,pom)==0)
		{
			free(pom);
			return i;
		}
	}

	free(pom);
	return UINT_MAX;
}

int	ssh_menu_add_item(
				ssh_menu_handle hMenu,
				const char* ItemName,
				unsigned    Message,
				unsigned    Flags)
{										assert(hMenu != 0);
	/*wchar_t	UniItemName[1024];
	//I tak nie dziala - zostaja krzaczki
	int ret=MultiByteToWideChar(
		1252,//CP_ACP,		       // UINT CodePage,         // code page
		MB_PRECOMPOSED	,	       // DWORD dwFlags,         // character-type options
		ItemName,		       // LPCSTR lpMultiByteStr, // address of string to map
	  strlen(ItemName)+1,		       // number of bytes in string
		UniItemName,			// LPWSTR lpWideCharStr,  // address of wide-character buffer
		1024				// int cchWideChar        // size of buffer
	);
	 */
	if(Flags==0)/* Ma byc domyslnie */
		Flags=MF_ENABLED;
	//return AppendMenuW(hMenu,Flags,Message,UniItemName);
	return AppendMenu(hMenu,Flags,Message,ItemName);
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
{										assert(hMenu != 0);
	UINT Flags = 0;
	if (asPosition)
		Flags |= MF_BYPOSITION;
	else
		Flags |= MF_BYCOMMAND;
	return RemoveMenu(hMenu, ItemCommandOrPosition, Flags) != 0xffffffff;
}

int ssh_menu_mark_item(
				ssh_menu_handle hMenu,
				unsigned    Check,
				unsigned    ItemCommandOrPosition,										
				unsigned    asPosition
			)
{										assert(hMenu != 0);
	UINT Flags=0;
	if(Check) Flags|=MF_CHECKED;
		else  Flags|=MF_UNCHECKED;
	if(asPosition) Flags|=MF_BYPOSITION;
		else	   Flags|=MF_BYCOMMAND;
	return  CheckMenuItem(hMenu,ItemCommandOrPosition,Flags)!=0xffffffff;
}

/// Ustawia lub usuwa marker przy itemie
int ssh_menu_mark_item2(	ssh_menu_handle hMenu,
				unsigned Check,
				unsigned ItemCommandOrPosition,
				unsigned asPosition
			)
{										assert(hMenu != 0);
	UINT Flags = 0;
	if (Check)
		Flags |= MF_CHECKED;
	else
		Flags |= MF_UNCHECKED;
	if (asPosition)
		Flags |= MF_BYPOSITION;
	else
		Flags |= MF_BYCOMMAND;
	return CheckMenuItem(hMenu, ItemCommandOrPosition, Flags) != 0xffffffff;
}

int ssh_realize_menu(ssh_menu_handle hMenu)
{										assert(hMenu != 0);
	return DrawMenuBar(WB_Hwnd ); 
}

#else
#error "THIS SYMSHELL MENU IMPLEMENTATION IS NOT FOR THIS PLATFORM"
#endif

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/


