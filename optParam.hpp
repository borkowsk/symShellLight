#ifndef OPTIONAL_PARAMETERS_HPP
#define  OPTIONAL_PARAMETERS_HPP
// KLASY potrzebne do obs�ugi parametr�w wywo�ania programu
// Obs�ugiwane s� typy standardowe numeryczne oraz
// inne typy maj�ce implementacje << >> na strumienie i
// operator por�wnania <=
// Obs�ugiwany jest te� typ const char* ale na poziomie wska�nik�w, czyli
// wska�nik do zawarto�� lini komend mo�e zosta� przypisany na zmienn�
// typu const char*
// NIESTETY nie mo�na u�ywa� typy string bo nie ma on obs�ugi strumieni!

//#include "platform.hpp"

#ifndef OLD_FASHION_CPP
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#else
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstrea.h>
#endif

#include "wb_ptr.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

using namespace std;

class OptionalParameterBase
{
public:
    //Interface sprawdzania parametr�w obiektami hierarchi OptionalParametr
    virtual int CheckStr(const char* argv,char sep='=')
    { return 0;} //0:Nie moja sprawa 1:moje i dobre -1:Moje,ale zly format

    //Interface drukowania lini HELPu
virtual void HelpPrn(ostream& o)
    {o<<"UPSSss..."<<endl;}
    //For writing state to stream. buff should be enought long
virtual const char* getName() { return "#"; }
virtual const char* getVal(char* buff=NULL)  { return "...";}

//Metoda do obslugi listy parametr�w - musi przeanalizowae� paramtry wywo�ania
//trzeba j� w main() gdzie� w miar� wcze�niej wywo�a�
static
    int parse_options(const int argc,const char* argv[],OptionalParameterBase* Parameters[],int Len);
//Zapis parametrów do pliku
static
    void report(ostream& Out,OptionalParameterBase* Parameters[],int  Len,const char* SeparatorTab="\t=\t",const char* SeparatorLine="\n");
static
    void table(ostream& Out,OptionalParameterBase* Parameters[],int  Len,const char* SeparatorTab="\t",const char* Head="$Parameters",const char* ValHd="values");
};

//Do rozdzielania parametr�w w tablicy i w helpie
class ParameterLabel:public OptionalParameterBase
{
	wb_pchar	Lead;
	wb_pchar	Info;

   //Interface drukowania linii HELPu
	virtual void HelpPrn(ostream& o)
	{o<<Lead.get()<<' '<<Info.get()<<endl;}

  public:
	ParameterLabel(const char* iInfo,const char* iLead="\n#"):
			Lead(clone_str(iLead)),Info(clone_str(iInfo)){}
	~ParameterLabel(){}
};

//Szablon klasy opcjonalnego parametru
template<class T>
class OptionalParameter:public OptionalParameterBase
{
  protected:
	wb_pchar	Name;  //Identyfikator (mnemonik) parametru
	wb_pchar	Info;  //Tekst informacyjny dla u�ytkownika
        T& 		Value; //Referencja to zmiennej kt�ra b�dzie modyfikowana
        T 		LBound;//Najmniejsza dozwolona warto��
        T 		HBound;//Najwi�ksza dozwolona warto��. Dla string�w/tekst�w mo�e mie� inne znaczenie

	void HelpPrn(ostream& o)     //"Wymagane" przez klas� bazow�
	{ o<<Name.get()<<": "<<Info.get()<<" Range: <"<<LBound<<','<<HBound<<">; Default: "<<Value<<endl; }

  public:
	OptionalParameter(T& iV,const T& iLB,const T& iHB,const char* iName,const char* iInfo):
		  Value(iV),LBound(iLB),HBound(iHB),Name(clone_str(iName)),Info(clone_str(iInfo)){}

	~OptionalParameter(){}

	int CheckStr(const char* argv,char sep='='); //Pr�ba przetworzenia konkretnego parametru

	virtual T convert(const char* str);  //Musz� by� r�ne implementacje w zale�no�ci od typu T

	virtual bool check(const T& _val);    //Sprawdzenie domy�lne sprawdza czy w zakresie
				//ale mo�e by� zmienione dla danego typu T (np. wb_pchar czy char*)
};


template<class T>
class OptEnumParametr:public OptionalParameter<T>
{
 protected:
	unsigned     NofEn;//Ile nazw dla typu zdefiniowano
	const char** EnNames;//Jakie to nazwy
	const T*     EnVals;

	void HelpPrn(ostream& o);     //Podstawienie dostarczonej przez klas� bazow�

 public:
	  OptEnumParametr(T& iV,const T& iLB,const T& iHB,
	                  const char* iName,const char* iInfo,
					  unsigned NofNames,const char** EnumNames,const T* EnumValues=NULL):
					  OptionalParameter<T>(iV,iLB,iHB,iName,iInfo),
					  NofEn(NofNames),EnNames(EnumNames),EnVals(EnumValues){}
	  ~OptEnumParametr(){}

	T convert(const char* str);  //Musi by� implementacja zale�na od typu EnNames i EnVals
};


//  FUNKCJE SPRAWDZENIA POPRAWNO�CI DANYCH
////////////////////////////////////////////////////////////////////////////////
template<class T>
bool OptionalParameter<T>::check(const T& _val)
{
	return (LBound<=_val) && (_val<=HBound);
}

template<> inline
bool OptionalParameter<string>::check(const string& val)
{
	return val.c_str()!=NULL &&  *val.c_str()!='\0';
}

template<> inline
bool OptionalParameter<wb_pchar>::check(const wb_pchar& val)
{
	return val.get()!=NULL && *val.get()!='\0';
}

template<> inline
bool OptionalParameter<char*>::check(char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}

template<> inline
bool OptionalParameter<const char*>::check(const char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}


// Funkcje konwersji
////////////////////////////////////////////////////////////////////////////////
template<> inline
char* OptionalParameter<char*>::convert(const char* str)
{
	return clone_str(str);//Bez zwalniania pamieci - ale to przeciez parametr wywolania
}

template<> inline
wb_pchar OptionalParameter<wb_pchar>::convert(const char* str)
{
	return wb_pchar(str);//Zrobi pewnie kopie tego co dostanie (kawa�ka parametru wywolania)
}

template<> inline
string OptionalParameter<string>::convert(const char* str)
{
	return string(str);
}

template<> inline
const char* OptionalParameter<const char*>::convert(const char* str)
{
	return str;//to przeciez kawalek parametru wywolania - nie mo�e sie zmienic
}

template<> inline
double OptionalParameter<double>::convert(const char* str)
{
	return atof(str);
}

template<> inline
float OptionalParameter<float>::convert(const char* str)
{
	return (float)atof(str);// conversion from 'double' to 'float', possible loss of data
}

template<> inline
long OptionalParameter<long>::convert(const char* str)
{
	return atol(str);
}

template<> inline
long long OptionalParameter<long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (long long)atof(str);
#else
	return atoll(str);
#endif
}

template<> inline
unsigned long long OptionalParameter<unsigned long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (unsigned long long)atof(str);
#else
	return atoll(str);
#endif
}

template<> inline
unsigned OptionalParameter<unsigned>::convert(const char* str)
{
	return atol(str);
}

template<> inline
int OptionalParameter<int>::convert(const char* str)
{
	return atoi(str);
}

template<> inline
bool OptionalParameter<bool>::convert(const char* str)
{
	return toupper(*str)=='Y' || toupper(*str)=='T' || (*str)=='1';
}

template<class T> inline  //Podstawienie konwersji dostarczonej przez klas� bazow�
T OptEnumParametr<T>::convert(const char* str)
{
    if(('A'<=str[0] && str[0]<='Z')
            || ('a'<=str[0] && str[0]<='z')
            || (str[0] == '_') )//Jest identyfikator
    {
        for(unsigned i=0;i<NofEn;i++)
            if(std::strcmp(str,EnNames[i])==0) //Jest?
            {
                if(EnVals) return EnVals[i];
                else return T(this->LBound+i);
            }
        return T(-9999);
    }
    else //Probujemy liczbowo
    {
        int pom=atol(str);
        return T(pom);
    }
}

//*  WERSJA OG�LNA ZG�ASZAJ�CA RACZEJ AWARIE
template<class T> inline
T OptionalParameter<T>::convert(const char* str)
{
  /*	istrstream Strm(str);
	T Val;
	Strm>>Val;//Wiele typ�w i tak nie ma, np. r�ne enum
	*/
	return T(-9999);//Zazwyczaj -9999 nie bedzie poprawne.
	//Zw�aszcza �e podstawowe typy i tak s� obs�u�one oddzielnie
	//wi�c sprawa dotyczy enum i jaki� pomys��w na uzywanie klas u�ytkownika
	//Na razie nic bardziej og�lnego nie wymy�li�em
}


//*????
//typedef OptionalParameter<class T> OptPar<class T>;  ??? HOW TO DECLARE THAT?


//	IMPLEMENTACJE GŁÓWNYCH METOD
////////////////////////////////////////////////////////////////////////////////
//Zapis parametrów do pliku
inline
void OptionalParameterBase::report(ostream& Out,OptionalParameterBase* Parameters[],int  Len,const char* SeparatorTab,const char* SeparatorLine)
{
  //Out<<Len;
  Out<<endl;
  for(int j=0;j<Len;j++)
  {
    char buff[1024];
    Out<<Parameters[j]->getName();
    Out<<SeparatorTab;
    Out<<Parameters[j]->getVal(buff);
    Out<<SeparatorLine;
  }
}

inline
void OptionalParameterBase::table(ostream& Out,OptionalParameterBase* Parameters[],int  Len,const char* SepTab,const char* Head,const char* ValHd)
{
  //Out<<Len;
   Out<<Head<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#') //POMIJA LABELKI
         Out<<pom<<SepTab;
   }

   Out<<endl<<ValHd<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#')//POMIJA LABELKI
         {
             char buff[1024];
             Out<<Parameters[j]->getVal(buff)<<SepTab;
         }
   }
}

/// \brief OptionalParameterBase::parse_options
/// \param argc
/// \param argv
/// \param Parameters
/// \param Len
/// \return
inline
int OptionalParameterBase::parse_options(const int argc,const char* argv[],
                                         OptionalParameterBase* Parameters[],int  Len)
{
    for(int i=1;i<argc;i++)
    {
        if( *argv[i]=='-' )
            continue;// Opcja X lub symshella
        if(std::strcmp(argv[i],"HELP")==0) //Bez kwalifikacji std:: mo�e by� k�opot w zakresie dzia�ania definicji friend'�w wb_pchar
        {
            cout<<endl<<"*** NAMES OF PARAMETERS:"<<endl<<flush;
            for(int j=0;j<Len;j++) Parameters[j]->HelpPrn(cout);
            cout<<"* JUST USE LIST OF SPACE SEPARATED PAIRS PARAMETER=VALUE"<<endl;
            cout<<"<<press the return/enter key>>"<<endl;
            cin.get();
            return 1;
        }
        for(int j=0;j<Len;j++)
        {
            int ret=Parameters[j]->CheckStr(argv[i]);
            if(ret==1) goto CONTINUE; //Odnaleziono
            if(ret==-1)
            {
                cerr<<"* SORRY (Press ENTER)*"<<endl;
                cin.ignore(0xffffffff,'\n');
                return -1;
            }
        }
        cerr<<"Unknown parameter "<<argv[i]<<endl;
        return -1;
CONTINUE:;
    }
    return 0;
}

template<class T> inline
int OptionalParameter<T>::CheckStr(const char* argv,char sep/*='='*/)
{
    const char* pom=NULL;
    if((pom=std::strstr(argv,Name.get()))!=NULL)
    {
        pom+=std::strlen(Name.get());
        if(*pom!=sep)
        {
            cerr<<argv<<" is malformed parameter value for "<<Name.get()<<endl;
            return -1;
        }
        T temp=convert(++pom);//Musi by� taka funkcja po�rednicz�ca zwracaj�ca warto�� niemodyfikowaln�
        if(check(temp))
        {
            Value=temp;
            cout<<"* Value "<<Name.get()<<" was changed into '"<<Value<<'\''<<endl;
            return 1;//Moja poprawna warto��
        }
        else
        {
            cerr<<"* Value "<<Name.get()<<" cannot be changed into '"<<Value<<'\''<<endl;
            cerr<<"** Proper values should be beetween "<<LBound<<" and "<<HBound<<endl;
            cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
            return -1;//Moja, ale niepoprawna warto��
        }
    }
    return 0;//Nie MOJA sprawa
}

template<> inline
int OptionalParameter<string>::CheckStr(const char* argv,char sep/*='='*/)
{
    const char* pom=NULL;
    if((pom=std::strstr(argv,Name.get()))!=NULL) //Bez kwalifikacji std:: mo�e by� k�opot w zakresie dzia�ania definicji friend'�w wb_pchar
    {
        pom+=std::strlen(Name.get());
        if(*pom!=sep)
        {
            cerr<<argv<<" is malformed parameter value for "<<Name.get()<<endl;
            return -1;
        }
        string temp=convert(++pom);//Musi by� taka funkcja po�rednicz�ca zwracaj�ca warto�� niemodyfikowaln�
        if(check(temp))
        {
            Value=temp;
            cout<<"* Value "<<Name.get()<<" was changed into '"<<Value.c_str()<<'\''<<endl;
            return 1;//Moja poprawna warto��
        }
        else
        {
            cerr<<"* Value "<<Name.get()<<" cannot be changed into \""<<Value.c_str()<<'"'<<endl;
            cerr<<"** Proper value may looks like \""<<LBound.c_str()<<"\" and \""<<HBound.c_str()<<"\""<<endl;
            cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
            return -1;//Moja, ale niepoprawna warto��
        }
    }
    return 0;//Nie MOJA sprawa
}

// NIETYPOWE METODY DRUKOWANIA HELPU DO PARAMETR�W TEKSTOWYCH
////////////////////////////////////////////////////////////////////////////////
template<> inline
void OptionalParameter<string>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.c_str()<<"\" or \""<<HBound.c_str()<<"\"; Default: \""<<Value.c_str()<<'"'<<endl;
	}

template<> inline
void OptionalParameter<wb_pchar>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.get()<<"\" or \""<<HBound.get()<<"\"; Default: \""<<Value.get()<<'"'<<endl;
	}

template<> inline
void OptionalParameter<char*>::HelpPrn(ostream& o)
	{
	  o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound<<"\" or \""<<HBound<<"\"; Default: \""<<Value<<'"'<<endl;
	}

template<> inline
void OptionalParameter<bool>::HelpPrn(ostream& o)     //"Wymagane" przez klas� bazow�
	{ o<<Name.get()<<": "<<Info.get()<<"; allowed are: 0,1,Yes,No,Tak,Nie; Default: "<<Value<<endl; }

template<class T> inline
void OptEnumParametr<T>::HelpPrn(ostream& o)
{ //GCC chciało tu wszędzie this-> ??? TODO
    o<<this->Name.get()<<": "<<this->Info.get()<<"; allowed are: ";
    for(unsigned i=0;i<this->NofEn;i++) //Tablica definiuje nazwy od LBound do HBound!
    {
        o<<this->EnNames[i];
        if(this->EnVals)
            o<<"="<<this->EnVals[i];
        o<<' ';
    }
    o<<"or integers range: <"<<this->LBound<<','<<this->HBound<<"> Default: "
    <<this->EnNames[this->Value-this->LBound]<<"="<<this->Value<<endl;
}

}//NAMESPACE WBRTM

/********************************************************************/
/*              SYMSHELLLIGHT  version 2020-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif
