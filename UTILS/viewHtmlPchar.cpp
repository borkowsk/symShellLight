// Call HTML viewer
//*//////////////////////////////////////////////
#include <iostream>
//#include "compatyb.hpp"
#include "wb_ptr.hpp"
#include "sshutils.hpp"

using wbrtm::wb_pchar;

#if defined( _Windows )
// Windows Header
#include <Windows.h>

int ViewHtml(const char* URL)
{
	wb_pchar bufor(strlen(URL)+80);
	bufor.prn("start /max %s",URL);
	int ret=WinExec(bufor.get(),SW_SHOWMINIMIZED);//Jak dla Windows 98

	if(ret==ERROR_FILE_NOT_FOUND  ||  ret==ERROR_PATH_NOT_FOUND )
	{
		bufor.prn("cmd /C start /max %s",URL);
		ret=WinExec(bufor.get(),SW_SHOWMINIMIZED);//Jak dla Windows NT
	}

	return ret;
}
#else
#include <cstdlib>

int ViewHtml(const char* URL)
{  // http://askubuntu.com/questions/8252/how-to-launch-default-web-browser-from-the-terminal
    wb_pchar bufor(strlen(URL)+80);
	bufor.prn("xdg-open \"%s\" &",URL);
    int ret=system(bufor.get());
    std::cerr<<"Command "<<bufor.get()<<" returned with code "<<ret<<std::endl;
    return 1;
}

#endif
/* *******************************************************************/
/*                  WBRTM  version 2022-10-27                        */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:    https://github.com/borkowsk                        */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
