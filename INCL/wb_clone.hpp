/** \file wb_clone.hpp
*	\brief  WB CLONING SUPPORT FOR ANY C++ OBJECTS */
/*	        ************************************** */
/** @date 2026-04-18 (last modification */
/**	\author borkowsk
*   \details
*       - Function for cloning strings;
*       - Template function for cloning scalars;
*       - Template class for forced cloning;
*       - For class pointers to char could be handled intuitively - by contents, not by pointer value.
*/
#ifndef WB_CLONE_HPP_INCLUDED_
#define WB_CLONE_HPP_INCLUDED_

#include <string.h>

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// \brief   Kopiuje stały łańcuch znaków na stertę
/// \return  Wynikiem jest kopia albo NULL, jeśli nie może stworzyć kopi.
inline char* clone_str(const char* const p)
{
    if (p != NULL) {
        auto len = ::strlen(p) + 1;
        char *out = new char[len];
        if (out != NULL) //To sprawdzenie już zbędne W WIĘKSZOŚCI SYSTEMÓW.
#ifdef _MSC_VER
        {
            strcpy_s(out,len,p);
        }
#else
        {
            ::strcpy(out, p);
        }
#endif
        return out;
    }
    else return NULL;
}

/// \brief Klonowanie łańcucha znaków zgodne z szablonem funkcyjnym clone().
///        Wersje dla const char *const
inline char* clone(const char *const p)
{
    return clone_str(p);
}

/// \brief Klonowanie łańcucha znaków zgodne z szablonem funkcyjnym clone().
///        Wersja dla char* nie różni się niczym. Chyba zbędna. TODO?
inline char* clone(char* p)
{
    return clone_str(p);
}

/// \brief  Funkcja szablonu klonowania obiektu na stertę,
/// \return  NULL jeśli nie może zaalokować.
template<class T>
#ifndef __BORLANDC__
inline 			//W Borlandzie 4.X powoduje pad :-D (a to ci stara historia! )
#else
static
#endif
T* clone(const T* p)
{
    return (p!=NULL?new T(*p):NULL);
}

///	 \brief CLASS alternative (FIXING TYPE) of cloning template.
template<class T>
class Clone
{
    T* ptr;
public:
    Clone(const T* par){ ptr=new T(*par);}
    operator T* () {return ptr;}
};

///  \brief "char" specialization of CLASS alternative of cloning template.
template<>
class Clone<char>
{
    char* ptr;
public:
    Clone(const char *const par){ ptr=clone_str(par);}
    operator char* () {return ptr;}
};

} //namespace
/* ***************************************************************** */
/*               WB_RTM for SymShell  version 2026                   */
/* ***************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif
