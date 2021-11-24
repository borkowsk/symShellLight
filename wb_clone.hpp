/**
*	WB CLONING SUPPORT FOR ANY C++ OBJECTS
*****************************************************************************************
*
* Function for cloning strings;
* Template function for cloning scalars;
* Template class for forced cloning;
* In class pointers to char could be handled intuitively - by contens, not by pointer value.
*/
#ifndef _WB_CLONE_HPP_
#define _WB_CLONE_HPP_

#ifndef __cplusplus
#error C++ required
#endif

#include <string.h>

///\namespace WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// Kopiuje stały łańcuch znaków na stertę
/// Zwraca NULL jeśli nie może
inline
char* clone_str(const char *const p)
{
char* out=NULL;
if(p!=NULL)
	out=new char[::strlen(p)+1];
if(out!=NULL)	
	::strcpy(out,p);
return out;
}

///Klonowanie łańcucha znaków zgodne z szablonem funkcyjnym clone()
///Wersje dla const char *const
inline
char* clone(const char *const p)
{
    return clone_str(p);
}

///Wersja dla char* nie różni się niczym. Chyba zbędna. TODO?
inline
char* clone(char* p)
{
    return clone_str(p);
}

/// Funkcja szablonu klonowania obiektu na stertę,
/// Zwraca NULL jeśli nie może zaalokować.
template<class T>
#ifndef __BORLANDC__
inline 			//W Borlandzie 4.X powoduje wywałkę :-D - a to ci stara historia!
#endif
T* clone(const T* p)
{
    return (p!=NULL?new T(*p):NULL);
}

///	CLASS VERSION (FIXING TYPE)
/// of cloning template
template<class T>
class Clone
{
    T* ptr;
public:
    Clone(const T* par){ ptr=new T(*par);}
    operator T* () {return ptr;}
};

template<>		//New syntax?
class Clone<char>
{
    char* ptr;
public:
    Clone(const char *const par){ ptr=clone_str(par);}
    operator char* () {return ptr;}
};

} //namespace
/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-24                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif
