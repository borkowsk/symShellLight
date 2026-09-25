/// @file
/// @brief A function that launches the default file viewer—most often a web browser.
/// @date 2026-09-25 (modified)
//*//////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "wb_ptr.hpp"
#include "sshutils.hpp" //To musi być, bo jest tam deklaracja zapowiadająca dla "ViewHtml" jako funkcji typu C.

using wbrtm::wb_pchar;

#if defined( _MSC_VER )

#include <Windows.h> // General MS Windows header

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @internal
/// If this particular mechanism starts to cause problems
/// with modern browsers, there is one function that is a "spiritual successor" to this
/// approach and is as easy to use as `WinExe`c:
///
/// ```C
/// //Opens a URL in the default browser without going through cmd.exe
/// ShellExecute(NULL, "open", URL, NULL, NULL, SW_SHOWMAXIMIZED);
/// ```
///
#else
/// @internal
/// Jeśli ten konkretny mechanizm zacząłby sprawiać problemy
/// z nowoczesnymi przeglądarkami, istnieje jedna funkcja, która jest "duchowym następcą" tego 
/// podejścia i jest równie prosta w użyciu co `WinExec`:
/// 
/// ```C
///  //Otwiera URL w domyślnej przeglądarce bez pośrednictwa cmd.exe
///  ShellExecute(NULL, "open", URL, NULL, NULL, SW_SHOWMAXIMIZED);
/// ```
///
#endif

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
    std::cerr << "Command \"" << bufor.get() << "\" returned with code " << ret << (ret>31?" SUCCESS":" ERROR") << std::endl;
    return ret;
}

#else

#include <cstdlib>

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @internal
/// `xdg-open` should work on most Linux and related systems.
/// 👉 http://askubuntu.com/questions/8252/how-to-launch-default-web-browser-from-the-terminal
#else
/// @internal
/// `xdg-open` powinno działać na większości systemów linux-owych i pokrewnych.
/// 👉 http://askubuntu.com/questions/8252/how-to-launch-default-web-browser-from-the-terminal
#endif
int ViewHtml(const char* URL)
{
    wb_pchar bufor(strlen(URL)+80);
    bufor.prn("xdg-open \"%s\" &",URL);
    int ret=system(bufor.get());
    std::cerr<<"Command \""<<bufor.get()<<"\" returned with code "<<ret<<std::endl;
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
