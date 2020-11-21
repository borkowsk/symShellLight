//
//	Implementation of more "sophisticated" methods for wb_pchar
//
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZE¯ENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#include "wb_ptr.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

wb_pchar& wb_pchar::prn(const char* format,...)
{
   va_list marker;
   va_start( marker, format );     /* Initialize variable arguments. */
   vsprintf(get_ptr_val(),format,marker);
   va_end( marker );              /* Reset variable arguments.      */	
   return *this;
}

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

//Used internaly
char* find(char* where,const char* forfind,bool fullwords)
{
	do{
		char* poz=::strstr(where,forfind);//Szuka

		if(poz==NULL) 
			return NULL;//Wogole nie znalazl

		if(!fullwords)
		{
			return poz;	//Znalazl polozony dowolnie
		}
		else			//Sprawdza czy cale slowo
		{
			size_t findlen=::strlen(forfind);		assert(findlen>0);
			char* pozza=poz+findlen;			//Pozycja za znaleziona fragmentem
			if(*pozza=='\0' || isspace(*pozza) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/",*pozza)!=NULL  )//Cza za jest "odstep"
				if(poz==where || isspace(*(poz-1)) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/",*(poz-1))!=NULL )//Czy przed jest "odstep".
				{
					return poz;
				}

			where=poz+1;
			//Jesli doszlo tu to szukamy dalej
		}
	
	}while(1);
}

//Wstawia lancuch do bufora na okreslonej pozycji - bufor jest sztafetowany!!!
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

bool replace(wb_pchar& bufor,const char* forrep,const char* whatins,bool fullwords,unsigned startpos)//,bool case_sens=1 ??? Brak funkcji w rodzaju stristr
{
	wb_pchar pom=bufor;//sztafeta!!!

	char* poz=NULL;
	poz=find(pom.get_ptr_val()+startpos,forrep,fullwords);

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
		poz=find(poz,forrep,fullwords);
		if(poz)
			pom=bufor;
	}
	
	return true;
}


} //namespace

/********************************************************************/
/*			          WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
