/** @file
 * @brief Implementacja funkcji interpretującej string jako wartość RGB.                             */
/*        ****************************************************************************************** */
/// @date 2026-04-21 (last modification)

#include <cctype>
#include <cstdlib>

#include "sshutils.hpp"
#include "wb_ptr.hpp"

using namespace std;

//Funkcja interpretująca string jako wartość RGB
//Dopuszczalne formaty to: xFFFFFF  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255)
//oraz dziesiętny oczywiście
unsigned strtorgb(const char *s, char **end_ptr)
{
#ifdef _MSC_VER
   char* next_token = NULL;
#endif
   while(isspace(*s)) s++;   //isblank() Usun białe       ?

   if(s[0]=='0' && tolower(s[1])=='x' )
        return  strtoul(s, end_ptr, 16);
   else
   if(tolower(s[0])=='x')
        return  strtoul(s+1, end_ptr, 16);
   else
   if(tolower(s[0])=='o')
        return  strtoul(s+1, end_ptr, 8);
   else
   if(tolower(s[0])=='b')
        return  strtoul(s+1, end_ptr, 2);
   else
   if(tolower(s[0])=='r' &&  tolower(s[1])=='g' && tolower(s[2])=='b' && tolower(s[3])=='(')
   {
        wbrtm::wb_pchar pom(s+4);
        char* end_ptr2=NULL;
#ifdef _MSC_VER
        char* token = strtok_s(pom.get_ptr_val(), "," , &next_token);
#else
        char* token=strtok(pom.get_ptr_val(),",");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        unsigned val=strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

#ifdef _MSC_VER
        token = strtok_s(NULL, ",", &next_token);
#else
        token=strtok(NULL,",");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        val=256*val+strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

#ifdef _MSC_VER
        token = strtok_s(NULL, ")", &next_token);
#else
        token = strtok(NULL, ")");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        val=256*val+strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

        return val;
   }
   else
   if(tolower(s[0])=='a' && tolower(s[1])=='r' &&  tolower(s[2])=='g' && tolower(s[3])=='b' && tolower(s[4])=='(')
   {
        wbrtm::wb_pchar pom(s+5);
        char* end_ptr2=NULL;

#ifdef _MSC_VER
        char* token = strtok_s(pom.get_ptr_val(), ",", &next_token);
#else
        char* token = strtok(pom.get_ptr_val(), ",");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        unsigned val=strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

#ifdef _MSC_VER
        token = strtok_s(NULL, ",", &next_token);
#else
        token = strtok(NULL, ",");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        val=256*val+strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

#ifdef _MSC_VER
        token = strtok_s(NULL, ",", &next_token);
#else
        token = strtok(NULL, ",");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        val=256*val+strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

#ifdef _MSC_VER
        token = strtok_s(NULL, ")", &next_token);
#else
        token = strtok(NULL, ")");
#endif
        if(token==NULL) { *end_ptr=(char*)s;return 0; }
        val=256*val+strtoul(token, &end_ptr2, 10);
        if(end_ptr2 != NULL && *end_ptr2 != '\0') { *end_ptr=(char*)s;return 0; }

        return val;
   }
   else
   {
        return strtoul(s, end_ptr, 10);
   }
}

/* *******************************************************************/
/*                   SYMSHELLLIGHT  version 2026                     */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/

