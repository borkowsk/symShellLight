#include <cctype>
#include <cstdlib>

#include "wb_ptr.hpp"

using namespace std;

//Funkcja interpretuj¹ca string jako wartoœæ RGB
//Dopuszczalne formaty to: xFFFFFF  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255) oraz dziesiêtny oczywiœcie

unsigned strtorgb(const char *s, char **endptr)
{
#ifdef _MSC_VER
   char* next_token = NULL;
#endif
   while(isspace(*s)) s++;   //isblank() Usun biale       ?

   if(s[0]=='0' && tolower(s[1])=='x' )
		return  strtoul(s,endptr,16);
   else
   if(tolower(s[0])=='x')
		return  strtoul(s+1,endptr,16);
   else
   if(tolower(s[0])=='o')
		return  strtoul(s+1,endptr,8);
   else
   if(tolower(s[0])=='b')
		return  strtoul(s+1,endptr,2);
   else
   if(tolower(s[0])=='r' &&  tolower(s[1])=='g' && tolower(s[2])=='b' && tolower(s[3])=='(')
   {
		wbrtm::wb_pchar pom(s+4);
		char* endptr2=NULL;
#ifdef _MSC_VER
		char* token = strtok_s(pom.get_ptr_val(), "," , &next_token);
#else
		char* token=strtok(pom.get_ptr_val(),",");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		unsigned val=strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

#ifdef _MSC_VER
		token = strtok_s(NULL, ",", &next_token);
#else
		token=strtok(NULL,",");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

#ifdef _MSC_VER
		token = strtok_s(NULL, ")", &next_token);
#else
		token = strtok(NULL, ")");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		return val;
   }
   else
   if(tolower(s[0])=='a' && tolower(s[1])=='r' &&  tolower(s[2])=='g' && tolower(s[3])=='b' && tolower(s[4])=='(')
   {
		wbrtm::wb_pchar pom(s+5);
		char* endptr2=NULL;

#ifdef _MSC_VER
		char* token = strtok_s(pom.get_ptr_val(), ",", &next_token);
#else
		char* token = strtok(pom.get_ptr_val(), ",");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		unsigned val=strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

#ifdef _MSC_VER
		token = strtok_s(NULL, ",", &next_token);
#else
		token = strtok(NULL, ",");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

#ifdef _MSC_VER
		token = strtok_s(NULL, ",", &next_token);
#else
		token = strtok(NULL, ",");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

#ifdef _MSC_VER
		token = strtok_s(NULL, ")", &next_token);
#else
		token = strtok(NULL, ")");
#endif
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtoul(token,&endptr2,10);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		return val;
   }
   else
   {
		return strtoul(s,endptr,10);
   }
}
