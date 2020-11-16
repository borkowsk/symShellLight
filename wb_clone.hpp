/*****************************************************************************************/
/* 	WB CLONING SUPPORT FOR ANY C++ OBJECTS  	                                         */
/*****************************************************************************************/
/* 
Function for cloning strings;
Template function for cloning scalars;
Template class for forced cloning;
In class pointers to char could be handled intuityvelly - by
	contens, not by pointer value.
*/
#ifndef _WB_CLONE_HPP_
#define _WB_CLONE_HPP_

#ifndef __cplusplus
#error C++ required
#endif

#include <string.h>

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

inline
char* clone_str(const char *const p)
// Kopiuje lancuch znakow na sterte
// Zwraca NULL jesli nie moze
{
char* out=NULL;
if(p!=NULL)
	out=new char[::strlen(p)+1];
if(out!=NULL)	
	::strcpy(out,p);
return out;
}

//Rozne wersje dla char*
inline
char* clone(char* p)
{
return clone_str(p);
}

inline
char* clone(const char *const p)
{
return clone_str(p);
}



template<class T>
#ifndef __BORLANDC__
inline 			//W Borlandzie 4.X powoduje wywalke
#endif
T* clone(const T* p)
// Kopiuje obiekt na sterte,
// WYRAZENIE :) Zwraca NULL jesli nie moze!!!
{
return (p!=NULL?new T(*p):NULL);
}

//	CLASS VERSION - FIXING TYPE
//-------------------------------
template<class T>
class Clone
{
T* ptr;
public:
Clone(const T* par){ ptr=new T(*par);}
operator T* () {return ptr;}
};

template<>		//New syntax
class Clone<char>
{
char* ptr;
public:
Clone(const char* par){ ptr=clone_str(par);}
operator char* () {return ptr;}
};



} //namespace

/********************************************************************/
/*			          WBRTM  light version 2009                     */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif
