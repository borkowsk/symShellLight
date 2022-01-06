///\file optParam.hpp
#ifndef OPTIONAL_PARAMETERS_HPP
#define  OPTIONAL_PARAMETERS_HPP
/// \brief KLASY potrzebne do obsługi parametrów wywołania programu
/// \details
/// Obsługiwane są typy standardowe numeryczne oraz
/// inne typy mające implementacje << >> na strumienie i
/// operator porównania <=
/// Obsługiwany jest też typ "const char*" , ale na poziomie wskaźników, czyli
/// wskaźnik do zawartości linii komend może zostać przypisany na zmienną
/// typu "const char*"
/// NIESTETY nie można używać typu 'string' bo nie ma on obsługi strumieni!
/// Chyba żeby już była? TODO CHECK IT!

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

using namespace std;

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// Baza klas dla wszystkich typów parametrów
/// \details Definiuje interfejs parametru i funkcje statyczne do obsługi całej listy możliwych parametrów
class OptionalParameterBase
{
public:
    /// Funkcja interfejsu sprawdzania parametrów obiektami hierarchii OptionalParametr
    virtual
    int CheckStr(const char* argv,char sep='=')
    { return 0;} //0: Nie moja sprawa; 1: moje i dobre; -1: Moje, ale zly format

    /// Funkcja interfejsu  drukowania linii HELP'u
virtual
    void HelpPrn(ostream& o) {o<<"UPSSss..."<<endl;}

    /// Funkcja interfejsu pobierania nazwy parametru
virtual
    const char* getName() { return "#"; }

    /// funkcja interfejsu pobierająca wartość parametru
virtual
    const char* getVal(char* buff=NULL)  { return "...";}

/// Metoda statyczna do obsługi listy parametrów.
/// \details Musi przeanalizować parametry wywołania. Trzeba wywołać ją w main() gdzieś w miarę wcześnie
static
    int parse_options(const int argc,
                      const char* argv[],
                      OptionalParameterBase* Parameters[],
                      int Len
                      );

/// Metoda statyczna zapisu parametrów do pliku jako raport
static
    void report(ostream& Out,
                OptionalParameterBase* Parameters[],
                int  Len,
                const char* SeparatorTab="\t=\t",
                const char* SeparatorLine="\n"
                        );

/// Metoda statyczna zapisu parametrów do pliku jako tabela
static
    void table(ostream& Out,
               OptionalParameterBase* Parameters[],
               int  Len,
               const char* SeparatorTab="\t",
               const char* Head="$Parameters",
               const char* ValHd="values"
                       );
};

// Klasy potomne
// /////////////////

/// Klasa do rozdzielania parametrów w tablicy i w helpie
class ParameterLabel:public OptionalParameterBase
{
protected:
	wb_pchar	Lead;
	wb_pchar	Info;

    /// Implementacja metody drukowania linii HELP'u wymaganej przez klasę bazową
	virtual void HelpPrn(ostream& o)
	{o<<Lead.get()<<' '<<Info.get()<<endl;}

  public:
    /// Konstruktor
	ParameterLabel(const char* iInfo,const char* iLead="\n#"):
			Lead(clone_str(iLead)),Info(clone_str(iInfo))
            {}
    /// Destruktor
	~ParameterLabel(){}
};

/// Szablon klasy opcjonalnego parametru
template<class T>
class OptionalParameter:public OptionalParameterBase
{
  protected:
	wb_pchar	Name;  ///<Identyfikator (mnemonik) parametru
	wb_pchar	Info;  ///<Tekst informacyjny dla użytkownika
        T& 		Value; ///<Referencja to zmiennej, która będzie modyfikowana
        T 		LBound;///<Najmniejsza dozwolona wartość
        T 		HBound;///<Największa dozwolona wartość. Dla stringów/tekstów może mieć inne znaczenie

    /// \brief Drukowanie helpu do tego parametru \details Metoda wymagane przez klasę bazową
	void HelpPrn(ostream& o)
	{ o<<Name.get()<<": "<<Info.get()<<" Range: <"<<LBound<<','<<HBound<<">; Default: "<<Value<<endl; }

  public:
    /// Konstruktor
	OptionalParameter(T& iV,const T& iLB,const T& iHB,const char* iName,const char* iInfo):
		  Value(iV),LBound(iLB),HBound(iHB),Name(clone_str(iName)),Info(clone_str(iInfo)){}

    /// Destruktor
	~OptionalParameter(){}

    /// Metoda przetworzenia konkretnego parametru \return ???
	int CheckStr(const char* argv,char sep='=');

    /// \brief Metoda konwersji. \details Muszą być dostarczone różne implementacje w zależności od typu T
	virtual T convert(const char* str);

    /// Sprawdzenie domyślne wartości
    /// \details Sprawdza czy wartość w zakresie, ale może być zmienione dla danego typu T
    /// (np. wb_pchar czy char* wymagają zupełnie innego sprawdzenia)
	virtual bool check(const T& _val);
};

/// Szablon klasy opcjonalnego parametru będącego WYLICZENIEM
template<class T>
class OptEnumParametr:public OptionalParameter<T>
{
 protected:
	unsigned     NofEn;    ///<Ile nazw dla typu zdefiniowano
	const char** EnNames;  ///<Jakie to nazwy
	const T*     EnVals;   ///< Domyślna wartość ???

	void HelpPrn(ostream& o);     //Podstawienie dostarczonej przez klasę bazową?

 public:
      /// Konstruktor
	  OptEnumParametr(T& iV,const T& iLB,const T& iHB,
	                  const char* iName,const char* iInfo,
					  unsigned NofNames,const char** EnumNames,const T* EnumValues=NULL):
					  OptionalParameter<T>(iV,iLB,iHB,iName,iInfo),
					  NofEn(NofNames),EnNames(EnumNames),EnVals(EnumValues)
                      {}

      /// Destruktor
	  ~OptEnumParametr(){}

    /// \brief Metoda konwersji. \details Musi być implementacja zależna od typu EnNames i EnVals
	T convert(const char* str);
};

// /////////////////////////////
// IMPLEMENTACJE on line
// /////////////////////////////

//  Funkcje sprawdzania poprawności
// ////////////////////////////////////

/// Implementacja ogólna sprawdzania poprawności
template<class T>
bool OptionalParameter<T>::check(const T& _val)
{
	return (LBound<=_val) && (_val<=HBound);
}

/// Implementacja sprawdzania poprawności dla typu 'string'
template<> inline
bool OptionalParameter<string>::check(const string& val)
{
	return val.c_str()!=NULL &&  *val.c_str()!='\0';
}

/// Implementacja sprawdzania poprawności dla typu 'wb_pchar'
template<> inline
bool OptionalParameter<wb_pchar>::check(const wb_pchar& val)
{
	return val.get()!=NULL && *val.get()!='\0';
}

/// Implementacja sprawdzania poprawności dla typu 'char*'
template<> inline
bool OptionalParameter<char*>::check(char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}

/// Implementacja sprawdzania poprawności dla typu 'const char*'
template<> inline
bool OptionalParameter<const char*>::check(const char* const& val)
{
	return (val!=NULL) && (*val!='\0');
}


// Funkcje konwersji
// //////////////////////////////////////////////////////////////////////////////

/// Konwersja dla typu  'char*'
template<> inline
char* OptionalParameter<char*>::convert(const char* str)
{
	return clone_str(str);//Bez zwalniania pamięci, bo to przecież parametr wywołania!
}

/// Konwersja dla typu  'const char*'
template<> inline
const char* OptionalParameter<const char*>::convert(const char* str)
{
    return str;//to przecież kawałek parametru wywołania, więc nie może się zmienić
}

/// Konwersja dla typu  'wb_pchar'
template<> inline
wb_pchar OptionalParameter<wb_pchar>::convert(const char* str)
{
	return wb_pchar(str);//Zrobi zapewne kopie tego, co dostanie (kawałka parametru wywołania)
}

/// Konwersja dla typu  'string'
template<> inline
string OptionalParameter<string>::convert(const char* str)
{
	return string(str);
}

/// Konwersja dla typu  'double'
template<> inline
double OptionalParameter<double>::convert(const char* str)
{
	return atof(str);
}

/// Konwersja dla typu  'float'
template<> inline
float OptionalParameter<float>::convert(const char* str)
{
	return (float)atof(str);// conversion from 'double' to 'float', possible loss of data
}

/// Konwersja dla typu  'long'
template<> inline
long OptionalParameter<long>::convert(const char* str)
{
	return atol(str);
}

/// Konwersja dla typu  'long long'
template<> inline
long long OptionalParameter<long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (long long)atof(str);
#else
	return atoll(str);
#endif
}

/// Konwersja dla typu  'unsigned long long'
template<> inline
unsigned long long OptionalParameter<unsigned long long>::convert(const char* str)
{
#if defined(_MSC_VER)
	return (unsigned long long)atof(str);
#else
	return atoll(str);
#endif
}

/// Konwersja dla typu  'unsigned int'
template<> inline
unsigned OptionalParameter<unsigned>::convert(const char* str)
{
	return atol(str);
}

/// Konwersja dla typu  'int'
template<> inline
int OptionalParameter<int>::convert(const char* str)
{
	return atoi(str);
}

/// Konwersja dla typu  'bool'
template<> inline
bool OptionalParameter<bool>::convert(const char* str)
{
	return toupper(*str)=='Y' || toupper(*str)=='T' || (*str)=='1';
}

/// Szablon konwersji dla typów WYLICZENIOWYCH (enum)
/// \details Podstawienie konwersji dostarczonej przez klasę bazową
template<class T> inline
T OptEnumParametr<T>::convert(const char* str)
{
    if(('A'<=str[0] && str[0]<='Z')
            || ('a'<=str[0] && str[0]<='z')
            || (str[0] == '_') )//Czy jest identyfikator
    {
        for(unsigned i=0;i<NofEn;i++)
            if(std::strcmp(str,EnNames[i])==0) //Jest?
            {
                if(EnVals) return EnVals[i];
                else return T(this->LBound+i);
            }
        return T(-9999);
    }
    else //W przeciwnym razie probujemy liczbowo
    {
        int pom=atol(str);
        return T(pom);
    }
}

///  WERSJA OGÓLNA konwersji ZGŁASZAJĄCA AWARIE
///\details Zostaje użyta przy próbie konwersji jakiegoś typu, który nie ma przygotowanej implementacji funkcji 'convert'
template<class T> inline
T OptionalParameter<T>::convert(const char* str)
{
  /*	istrstream Strm(str);
	T Val;
	Strm>>Val;//Wiele typów i tak nie ma, np. różne enum
	*/
	return T(-9999);//Zazwyczaj -9999 nie będzie poprawną daną.
	//Podstawowe typy i tak są obsłużone oddzielnie,
	//więc sprawa dotyczy enums i jakichś pomysłów na używanie klas użytkownika
	//Na razie nic bardziej ogólnego nie wymyśliłem
}


//*????
//typedef OptionalParameter<class T> OptPar<class T>;  ??? TODO HOW TO DECLARE THAT?

//	Główne metody
// //////////////////////////////////////////////////////////////////////////////

/// \brief Zapis parametrów do pliku "raportu"
/// \param Out
/// \param Parameters
/// \param Len
/// \param SeparatorTab
/// \param SeparatorLine
inline
void OptionalParameterBase::report(ostream& Out,OptionalParameterBase* Parameters[],
                                   int  Len,const char* SeparatorTab,const char* SeparatorLine)
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

/// \brief Zapis parametrów w formie tabeli
/// \param Out
/// \param Parameters
/// \param Len
/// \param SepTab
/// \param Head
/// \param ValHd
inline
void OptionalParameterBase::table(ostream& Out,OptionalParameterBase* Parameters[],
                                  int  Len,const char* SepTab,const char* Head,const char* ValHd)
{
  //Out<<Len;
   Out<<Head<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#') //POMIJA ETYKIETKI
         Out<<pom<<SepTab;
   }

   Out<<endl<<ValHd<<SepTab;
   for(int j=0;j<Len;j++)
   {
       const char* pom=Parameters[j]->getName();
       if(pom[0]!='#') //POMIJA ETYKIETKI
         {
             char buff[1024];
             Out<<Parameters[j]->getVal(buff)<<SepTab;
         }
   }
}

/// \brief Główna funkcja parsująca listę parametrów wywołania
/// \param argc
/// \param argv
/// \param Parameters
/// \param Len
/// \return 0 if OK, -1 on syntax error
inline
int OptionalParameterBase::parse_options(const int argc,const char* argv[],
                                         OptionalParameterBase* Parameters[],int  Len)
{
    for(int i=1;i<argc;i++)
    {
        if( *argv[i]=='-' )
            continue;// Opcja X11 lub symshell'a, czy inne, które chcemy obsłużyć inaczej
        if(std::strcmp(argv[i],"HELP")==0) //Bez kwalifikacji std:: może być kłopot w zakresie działania definicji friend'ów wb_pchar
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

/// \brief Szablon funkcji sprawdzania łańcucha parametru
/// \tparam T
/// \param argv
/// \param sep
/// \return 1 if OK, -1 on error, 0 for ignoring
template<class T> inline
int OptionalParameter<T>::CheckStr(const char* argv,char sep/*arator*/)
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

        T temp=convert(++pom);//Musi istnieć taka funkcja pośrednicząca zwracająca wartość niemodyfikowalną

        if(check(temp))
        {
            Value=temp;
            cout<<"* Value "<<Name.get()<<" was changed into '"<<Value<<'\''<<endl;
            return 1;//Moja poprawna wartość
        }
        else
        {
            cerr<<"* Value "<<Name.get()<<" cannot be changed into '"<<Value<<'\''<<endl;
            cerr<<"** Proper values should be beetween "<<LBound<<" and "<<HBound<<endl;
            cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
            return -1;//Moja, ale niepoprawna wartość
        }
    }
    return 0;//Nie MOJA wartość. Szukaj dalej!
}

/// \brief  funkcji sprawdzania łańcucha parametru dla typu 'string'
/// \param argv
/// \param sep
/// \return 1 if OK, -1 on error, 0 for ignoring
template<> inline
int OptionalParameter<string>::CheckStr(const char* argv,char sep/*arator*/)
{
    const char* pom=NULL;
    if((pom=std::strstr(argv,Name.get()))!=NULL) //Bez kwalifikacji std:: może być kłopot w zakresie działania definicji friend'ów wb_pchar
    {
        pom+=std::strlen(Name.get());
        if(*pom!=sep)
        {
            cerr<<argv<<" is malformed parameter value for "<<Name.get()<<endl;
            return -1;
        }

        string temp=convert(++pom);//Musi istnieć taka funkcja pośrednicząca zwracająca wartość niemodyfikowalną

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

// NIETYPOWE METODY DRUKOWANIA HELPU DO PARAMETRÓW TEKSTOWYCH
// //////////////////////////////////////////////////////////////////////////////

/// \brief  funkcja drukowania helpu parametru dla typu 'string'
template<> inline
void OptionalParameter<string>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.c_str()<<"\" or \""<<HBound.c_str()<<"\"; Default: \""<<Value.c_str()<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'wb_pchar'
template<> inline
void OptionalParameter<wb_pchar>::HelpPrn(ostream& o)
	{
		o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.get()<<"\" or \""<<HBound.get()<<"\"; Default: \""<<Value.get()<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'char*'
template<> inline
void OptionalParameter<char*>::HelpPrn(ostream& o)
	{
	  o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound<<"\" or \""<<HBound<<"\"; Default: \""<<Value<<'"'<<endl;
	}

/// \brief  funkcja drukowania helpu parametru dla typu 'bool'
template<> inline
void OptionalParameter<bool>::HelpPrn(ostream& o)
	{ o<<Name.get()<<": "<<Info.get()<<"; allowed are: 0,1,Yes,No,Tak,Nie; Default: "<<Value<<endl; }

/// \brief  Szablon funkcji drukowania helpu parametru dla typu WYLICZENIOWEGO 'enum'
template<class T> inline
void OptEnumParametr<T>::HelpPrn(ostream& o)
{ //G++ chciało tu kiedyś wszędzie this-> ??? TODO - MOŻE JUŻ NIEPOTRZEBNE?
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

/*v*******************************************************************/
/*               SYMSHELLLIGHT  version 2022-01-06                   */
/*v*******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/*v*******************************************************************/
#endif
