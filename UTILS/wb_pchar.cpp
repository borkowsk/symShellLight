///	Implementation of more "sophisticated" methods for \class wb_pchar
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZE�ENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#include "wb_ptr.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// Wyprowadza formatowane dane na zawartość obiektu wb_pchar.
/// \note Nie sprawdza rozmiaru!!! Trzeba wcześniej zaalokować bezpiecznie.
/// \param format : jak dla vsprintf
/// \param ...
/// \return
wb_pchar& wb_pchar::prn(const char* format,...)
{
   va_list marker;
   va_start( marker, format );     /* Initialize variable arguments. */
   vsprintf(get_ptr_val(),format,marker);
   va_end( marker );              /* Reset variable arguments.      */	
   return *this;
}

/// Dopisuje do starej zawartości obiektu wb_pchar.
/// \note Nie sprawdza rozmiaru!!! Trzeba wcześniej zaalokować bezpiecznie.
/// \param format  : jak dla vsprintf
/// \param ...
/// \return
wb_pchar& wb_pchar::add(const char* format,...)
{
   va_list marker;
   va_start( marker, format );     /* Initialize variable arguments. */
   size_t pos=::strlen(get());
   char* ptr=get_ptr_val();
   ptr+=pos;
   vsprintf(ptr,format,marker);
   va_end( marker );              /* Reset variable arguments.      */	
   return *this;
}

/// \NOTE Used only internally
static char* _find(char* where,const char* forfind,bool fullwords)
{
	do{
		char* poz=::strstr(where,forfind);//Szuka

		if(poz==NULL) 
			return NULL;//W ogóle nie znalazł

		if(!fullwords)
		{
			return poz;	//Znalazł połozony dowolnie
		}
		else			//Sprawdza, czy całe słowo
		{
			size_t findlen=::strlen(forfind);		                        assert(findlen>0);
			char* pozza=poz+findlen;			//Pozycja za znaleziona fragmentem
			if(*pozza=='\0' || isspace(*pozza) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/",*pozza)!=NULL  )//Cza za jest "odstep"
				if(poz==where || isspace(*(poz-1)) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/",*(poz-1))!=NULL )//Czy przed jest "odstep".
				{
					return poz;
				}

			where=poz+1; // Jeśli doszło tu to szukamy dalej
		}
	
	}while(1);
}

/// Wstawia łańcuch tekstowy do bufora na określonej pozycji
/// \note bufor jest sztafetowany! TODO CHECK?
///
/// \param bufor
/// \param pos
/// \param whatins
/// \return 'true' jeśli wykonał akcję, a 'false' jeśli nie znalazł
bool insert(wb_pchar& bufor,unsigned pos,const char* whatins)
{
    if(pos>::strlen(bufor.get()))
        return false;

    wb_pchar pom=bufor;//sztafeta!!?
    bufor.alloc(::strlen(pom.get())+::strlen(whatins)+1);
    char* posptr=pom.get_ptr_val()+pos;
    char  point=*posptr;
    *posptr='\0';//ciach
    posptr++;
    bufor.prn("%s%s%c%s",pom.get(),whatins,point,posptr);

    return true;
}

/// Zamienia wszystkie łańcuchy 'forrep' zawarte w obiekcie wb_pchar
/// na łańcuchy 'whatins'. \note Bufor jest sztafetowany!!!
/// \param bufor
/// \param forrep
/// \param whatins
/// \param fullwords
/// \param startpos
/// \return 'true' jeśli wykonał akcję, a 'false' jeśli nie znalazł
bool replace(wb_pchar& bufor,const char* forrep,const char* whatins,bool fullwords,unsigned startpos)//,bool case_sens=1 ??? Brak funkcji w rodzaju stristr
{
	wb_pchar pom=bufor;//sztafeta!!!

	char* poz=NULL;
	poz=_find(pom.get_ptr_val()+startpos,forrep,fullwords);

	//cerr<<"REPLACE("<<pom<<" , "<<forrep<<" , "<<whatins<<")"<<(poz?"OK":"NO")<<endl;
	
	if(poz==NULL) 
		return false;//nie ma tego do zamiany

	while(poz!=NULL)
	{
		bufor.alloc(::strlen(pom.get())+::strlen(whatins)-::strlen(forrep)+1);
		*poz='\0';
		poz+=::strlen(forrep);
		bufor.prn("%s%s%s",pom.get(),whatins,poz);

		poz=bufor.get_ptr_val()+::strlen(pom.get())+::strlen(whatins);
		poz=_find(poz,forrep,fullwords);
		if(poz)
			pom=bufor;
	}
	
	return true;
}


} //namespace wbrtm
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
