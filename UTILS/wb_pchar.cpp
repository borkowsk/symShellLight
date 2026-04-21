/** @file
 * @brief Implementation of more "sophisticated" methods for class "wb_pchar".                       */
/*        ****************************************************************************************** */
/// @date 2026-04-21 (last modification)

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#include "wb_ptr.hpp"
#include "maybe_unused.h"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

// Wyprowadza formatowane dane na zawartość obiektu wb_pchar.
// \note Nie sprawdza rozmiaru!
//       Trzeba wcześniej zaalokować bezpiecznie.
// \param format jak dla `vsprintf`
MAYBE_UNUSED
wb_pchar& wb_pchar::prn(const char* format,...)
{
   va_list marker;
   va_start( marker, format );     /* Initialize variable arguments. */
   vsprintf(get_ptr_val(),format,marker);
   va_end( marker );              /* Reset variable arguments.      */	
   return *this;
}

// Dopisuje do starej zawartości obiektu wb_pchar.
// \note Nie sprawdza rozmiaru!!!
//       Trzeba wcześniej zaalokować bezpiecznie.
// \param format jak dla vsprintf
MAYBE_UNUSED
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

// \NOTE Used only internally
MAYBE_UNUSED
static char* _find(char* where, const char* for_find, bool full_words)
{
    do{
        char* poz=::strstr(where, for_find);//Szuka

        if(poz==NULL) 
            return NULL; //W ogóle nie znalazł

        if(!full_words)
        {
            return poz;	//Znalazł położony dowolnie
        }
        else			//Sprawdza, czy całe słowo
        {
            size_t find_len=::strlen(for_find);													assert(find_len > 0);
            char* loc_beh= poz + find_len;			//Pozycja za znaleziona fragmentem
            if(*loc_beh == '\0' || isspace(*loc_beh) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/", *loc_beh) != NULL ) //Czy za jest "odstęp"?
                if(poz==where || isspace(*(poz-1)) || ::strchr(".,;:?!@#$%^&*()-+={}[]|\\'<>/",*(poz-1))!=NULL ) //Czy przed jest "odstęp"?
                {
                    return poz;
                }

            where=poz+1; // Jeśli doszło tu to szukamy dalej
        }
    
    }while(1);
}

// Wstawia łańcuch tekstowy do bufora na określonej pozycji
// \note bufor jest sztafetowany!
// \return 'true' jeśli wykonał akcję, a 'false' jeśli nie znalazł
MAYBE_UNUSED //TODO CHECK?
bool insert(wb_pchar& bufor, unsigned where, const char* what_ins)
{
    if(where > ::strlen(bufor.get()))
        return false;

    wb_pchar pom=bufor;//sztafeta!!?
    bufor.alloc(::strlen(pom.get()) + ::strlen(what_ins) + 1);
    char* location= pom.get_ptr_val() + where;
    char  point=*location;
    *location='\0';//ciach
    location++;
    bufor.prn("%s%s%c%s", pom.get(), what_ins, point, location);

    return true;
}

// Zamienia wszystkie łańcuchy 'for_rep' zawarte w obiekcie `wb_pchar` na łańcuchy 'what_ins'.
// \note Bufor jest sztafetowany!!!
// \return 'true' jeśli wykonał akcję, a 'false' jeśli nie znalazł
MAYBE_UNUSED
bool replace(wb_pchar& bufor, const char* for_rep, const char* what_ins, bool full_words, unsigned where_start)
{
    wb_pchar pom=bufor;//sztafeta!!!

    char* poz=NULL;
    poz=_find(pom.get_ptr_val() + where_start, for_rep, full_words);

    //cerr<<"REPLACE("<<pom<<" , "<<for_rep<<" , "<<what_ins<<")"<<(poz?"OK":"NO")<<endl;
    
    if(poz==NULL) 
        return false;//nie ma tego do zamiany

    while(poz!=NULL)
    {
        bufor.alloc(::strlen(pom.get()) + ::strlen(what_ins) - ::strlen(for_rep) + 1);
        *poz='\0';
        poz+=::strlen(for_rep);
        bufor.prn("%s%s%s", pom.get(), what_ins, poz);

        poz=bufor.get_ptr_val()+::strlen(pom.get())+::strlen(what_ins);
        poz=_find(poz, for_rep, full_words);
        if(poz)
            pom=bufor;
    }
    
    return true;
}

} //namespace wbrtm

/* *******************************************************************/
/*                   SYMSHELLLIGHT  version 2026                     */
/* *******************************************************************/
/*             THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/

