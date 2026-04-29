/// @file
/// @brief Call HTML viewer
/// @date 2026-04-29 (modified)
//*////////////////////////////

#include <iostream>
//#include "compatyb.hpp"
#include "wb_ptr.hpp"
#include "sshutils.hpp"

using wbrtm::wb_pchar;

#if defined( _MSC_VER )

#include <Windows.h> // General MS Windows header

/// @internal
/// Jeśli mimo braku chęci do zmian w kodzie, ten konkretny mechanizm zacząłby sprawiać problemy 
/// z nowoczesnymi przeglądarkami, istnieje jedna funkcja, która jest "duchowym następcą" tego 
/// podejścia i jest równie prosta w użyciu co WinExec:
/// 
/// ```C
///  //Otwiera URL w domyślnej przeglądarce bez pośrednictwa cmd.exe
///  ShellExecute(NULL, "open", URL, NULL, NULL, SW_SHOWMAXIMIZED);
/// ```
///

int ViewHtml(const char* URL)
{
	wb_pchar bufor(strlen(URL)+80);
	bufor.prn("start /max %s",URL);
	int ret=WinExec(bufor.get(), SW_SHOWNORMAL); //Jak dla Windows 98

	if(ret==ERROR_FILE_NOT_FOUND  ||  ret==ERROR_PATH_NOT_FOUND )
	{
		bufor.prn("cmd /C start /max \"\" \"%s\"",URL);
		ret=WinExec(bufor.get(), SW_SHOWNORMAL); //Jak dla Windows NT
	}
	std::cerr << "Command " << bufor.get() << " returned with code " << ret << (ret>31?" SUCCESS":" ERROR") << std::endl;
	return ret;
}

#else

#include <cstdlib>

/// @internal
/// `xdg-open` powinno działać na większości systemów linuxowych i pokrewnych.
/// 

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
/*                   WBRTM  version 2026                             */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:    https://github.com/borkowsk                        */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
