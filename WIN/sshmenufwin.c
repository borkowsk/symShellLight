/* IMPLEMENTATION FOR SYMSHELL MENU AND RELEATED FEATURES */
/**********************************************************/
#include "INCLUDE/platform.h"

#if defined( __WIN32__ ) 
#include <windows.h> //bo Menu itp
#include <assert.h>
#include "../../sshmenuf.h"

#ifdef __cplusplus
extern "C" 
#endif

extern HWND	WB_Hwnd;//W symshwin.c

ssh_menu_handle ssh_main_menu()
{
return	GetMenu(WB_Hwnd);
}

ssh_menu_handle ssh_sub_menu(
					ssh_menu_handle hMenu,				
					unsigned    Position)
{
return GetSubMenu(hMenu,Position);
}

unsigned ssh_get_item_position(
					 ssh_menu_handle hMenu,
					 const char* ItemName)
{
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
{
/*wchar_t	UniItemName[1024];
//I tak nie dziala - zostaja krzaczki
int ret=MultiByteToWideChar(
	1252,//CP_ACP,			// UINT CodePage,         // code page
	MB_PRECOMPOSED	,		//DWORD dwFlags,         // character-type options
	ItemName,				//LPCSTR lpMultiByteStr, // address of string to map
  strlen(ItemName)+1,		//number of bytes in string
	UniItemName,			//LPWSTR lpWideCharStr,  // address of wide-character buffer
	1024					//int cchWideChar        // size of buffer
);
 */
if(Flags==0)/* Ma byc domyslnie */
	Flags=MF_ENABLED;
//return AppendMenuW(hMenu,Flags,Message,UniItemName);
return AppendMenu(hMenu,Flags,Message,ItemName);
}

int ssh_menu_mark_item(
					ssh_menu_handle hMenu,
					unsigned    Check,
					unsigned    ItemCommandOrPosition,										
					unsigned    asPosition
					   )
{
UINT Flags=0;
if(Check) Flags|=MF_CHECKED;
	else  Flags|=MF_UNCHECKED;
if(asPosition) Flags|=MF_BYPOSITION;
	else	   Flags|=MF_BYCOMMAND;
return  CheckMenuItem(hMenu,ItemCommandOrPosition,Flags)!=0xffffffff;
}

int ssh_realize_menu(ssh_menu_handle hMenu)
{
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


