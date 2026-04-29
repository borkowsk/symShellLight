/// @file optParam.hpp
/// @brief CLASSES needed to handle program call parameters.
/// @date 2026-04-29 (last modification)
// ////////////////////////////////////////////////////////////////
#ifndef OPTIONAL_PARAMETERS_HPP_
#define OPTIONAL_PARAMETERS_HPP_

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "maybe_unused.h"
#include "wb_ptr.hpp"

using namespace std;

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {
    /**
     * @defgroup MAINandPARS Surroundings of the main function and call parameters
     * \brief    Handling program call parameters and other similar aspects.
     */
    ///@{

    /// \brief   Base class for all parameter types.
    /// \details Defines the parameter interface and static functions to handle the entire list of possible parameters.
    ///     Derived classes handle standard numerical types
    ///     and other types that have << >> stream implementations and the <= comparison operator.
    ///     The "const char*" type is also supported, but at the pointer level,
    ///     meaning a pointer to the command line content can be assigned to a variable
    ///     of type "const char*".
    ///     UNFORTUNATELY, the 'string' type cannot be used because it lacks stream support!
    ///     Unless it already has it? TODO CHECK IT!
    class OptionalParameterBase
    {
    public:
        /// Interface function for checking parameters using objects of the `OptionalParameter` hierarchy.
        virtual
        int CheckStr(const char* argv,char sep='=')
        { return 0;} //0: Not my business; 1: Mine and good; -1: Mine, but bad format

        /// Interface function for printing the HELP line.
        virtual
        void HelpPrn(ostream& o) {o<<"UPSSss..."<<endl;}

        /// Interface function for getting the parameter name.
        virtual
        const char* getName() { return "#"; }

        /// Interface function for getting the parameter value.
        virtual
        const char* getVal(char* buff=NULL)  { return "...";}

        /// Static method for handling the parameter list.
        /// \details Must analyze the call parameters. It needs to be called in main() reasonably early.
        static
        int parse_options(const int argc,
                          const char* argv[],
                          OptionalParameterBase* Parameters[],
                          int Len
        );

        /// Static method for saving parameters to a file as a report.
        static MAYBE_UNUSED
        void report(ostream& Out,
                    OptionalParameterBase* Parameters[],
                    int  Len,
                    const char* SeparatorTab="\t=\t",
                    const char* SeparatorLine="\n"
        );

        /// Static method for saving parameters to a file as a table.
        static MAYBE_UNUSED
        void table(ostream& Out,
                   OptionalParameterBase* Parameters[],
                   int  Len,
                   const char* SeparatorTab="\t",
                   const char* Head="$Parameters",
                   const char* ValHd="values"
        );
    };

// Derived classes:
// ////////////////

    /// Class for separating parameters in the array and in help documentation.
    class MAYBE_UNUSED ParameterLabel:public OptionalParameterBase
    {
    protected:
        wb_pchar    Lead;
        wb_pchar    Info;

        /// Implementation of the HELP line printing method required by the base class.
        virtual void HelpPrn(ostream& o)
        {o<<Lead.get()<<' '<<Info.get()<<endl;}

    public:
        /// Constructor.
        ParameterLabel(const char* iInfo,const char* iLead="\n#"):
                Lead(clone_str(iLead)),Info(clone_str(iInfo))
        {}
        /// Destructor.
        ~ParameterLabel(){}
    };

    /// Template class for an optional parameter.
    template<class T>
    class MAYBE_UNUSED OptionalParameter:public OptionalParameterBase
    {
    protected:
        wb_pchar    Name;   ///< Identifier (mnemonic) of the parameter.
        wb_pchar    Info;   ///< Information text for the user.
        T&     Value;  ///< Reference to the variable that will be modified.
        T      LBound; ///< Minimum allowed value.
        T      HBound; ///< Maximum allowed value. May have a different meaning for strings/text.

        /// \brief Printing help for this parameter \details Method required by the base class.
        void HelpPrn(ostream& o)
        { o<<Name.get()<<": "<<Info.get()<<" Range: <"<<LBound<<','<<HBound<<">; Default: "<<Value<<endl; }

    public:
        /// Constructor.
        OptionalParameter(T& iV,const T& iLB,const T& iHB,const char* iName,const char* iInfo):
                Value(iV),LBound(iLB),HBound(iHB),Name(clone_str(iName)),Info(clone_str(iInfo)){}

        /// Destructor.
        ~OptionalParameter(){}

        /// Method for processing a specific parameter. \return ???
        int CheckStr(const char* argv,char sep='=');

        /// Conversion method. \details Different implementations must be provided depending on type T.
        virtual T convert(const char* str);

        /// Default value check.
        /// \details Checks if the value is within range, but can be overridden for a specific type `T`.
        /// (e.g., `wb_pchar` or `char*` require completely different checks)
        virtual bool check(const T& _val);
    };

    /// Template class for an optional parameter that is an ENUMERATION.
    template<class T>
    class MAYBE_UNUSED OptEnumParametr:public OptionalParameter<T>
    {
    protected:
        unsigned     NofEn;    ///< How many names have been defined for the type.
        const char** EnNames;  ///< The names themselves.
        const T* EnVals;   ///< Default value (???).
        MAYBE_UNUSED
        void HelpPrn(ostream& o);     ///< Override provided by the base class (???).

    public:
        /** Constructor. */ MAYBE_UNUSED
        OptEnumParametr(T& iV,const T& iLB,const T& iHB,
                        const char* iName,const char* iInfo,
                        unsigned NofNames,const char** EnumNames,const T* EnumValues=NULL):
                OptionalParameter<T>(iV,iLB,iHB,iName,iInfo),
                NofEn(NofNames),EnNames(EnumNames),EnVals(EnumValues)
        {}

        /// Destructor.
        ~OptEnumParametr(){}

        /// Conversion method. \details Implementation must depend on `EnNames` and `EnVals` types.
        T convert(const char* str);
    };

// /////////////////////////////
// Inline implementations:
// /////////////////////////////

//  Validation functions:
// /////////////////////////////////

    /// General implementation for checking if a parameter is within range <LBound...HBound>.
    template<class T>
    bool OptionalParameter<T>::check(const T& _val)
    {
        return (LBound<=_val) && (_val<=HBound);
    }

    /// Validation implementation for 'string' type.
    template<> inline
    bool OptionalParameter<string>::check(const string& val)
    {
        return val.c_str()!=NULL &&  *val.c_str()!='\0';
    }

    /// Validation implementation for 'wb_pchar' type.
    template<> inline
    bool OptionalParameter<wb_pchar>::check(const wb_pchar& val)
    {
        return val.get()!=NULL && *val.get()!='\0';
    }

    /// Validation implementation for 'char*' type.
    template<> inline
    bool OptionalParameter<char*>::check(char* const& val)
    {
        return (val!=NULL) && (*val!='\0');
    }

    /// Validation implementation for 'const char*' type.
    template<> inline
    bool OptionalParameter<const char*>::check(const char* const& val)
    {
        return (val!=NULL) && (*val!='\0');
    }


// Funkcje konwersji:
// //////////////////

    /// "Konwersja" z `const char*` dla typu  'char*'. Polega na sklonowaniu parametru na stertę. @note WYPŁYW PAMIĘCI!
    template<> inline
    char* OptionalParameter<char*>::convert(const char* str)
    {
        return clone_str(str); //Bez zwalniania pamięci, bo to przecież parametr wywołania!
    }

    /// Konwersja z `const char*` dla typu  'const char*'. Udawana. Po prostu zwraca to samo,
    /// bo zakłada, że to przecież kawałek parametru wywołania, więc nie może się zmienić.
    template<> inline
    const char* OptionalParameter<const char*>::convert(const char* str)
    {
        return str;
    }

    /// Konwersja dla typu  'wb_pchar'.
    template<> inline
    wb_pchar OptionalParameter<wb_pchar>::convert(const char* str)
    {
        return wb_pchar(str); //Zrobi zapewne kopie tego, co dostanie (kawałka parametru wywołania)
    }

    /// Konwersja dla typu  'string'.
    template<> inline
    string OptionalParameter<string>::convert(const char* str)
    {
        return string(str);
    }

    /// Konwersja dla typu  'double'.
    template<> inline
    double OptionalParameter<double>::convert(const char* str)
    {
        return atof(str);
    }

    /// Konwersja dla typu  'float'.
    template<> inline
    float OptionalParameter<float>::convert(const char* str)
    {
        return (float)atof(str);// conversion from 'double' to 'float', possible loss of data
    }

    /// Konwersja dla typu  'long'.
    template<> inline
    long OptionalParameter<long>::convert(const char* str)
    {
        return atol(str);
    }

    /// Konwersja dla typu  'long long'.
    template<> inline
    long long OptionalParameter<long long>::convert(const char* str)
    {
    #if defined(_MSC_VER)
        return (long long)atof(str);
    #else
        return atoll(str);
    #endif
    }

    /// Konwersja dla typu  'unsigned long long'.
    template<> inline
    unsigned long long OptionalParameter<unsigned long long>::convert(const char* str)
    {
    #if defined(_MSC_VER)
        return (unsigned long long)atof(str);
    #else
        return atoll(str);
    #endif
    }

    /// Konwersja dla typu  'unsigned int'.
    template<> inline
    unsigned OptionalParameter<unsigned>::convert(const char* str)
    {
        return atol(str);
    }

    /// Konwersja dla typu  'int'.
    template<> inline
    int OptionalParameter<int>::convert(const char* str)
    {
        return atoi(str);
    }

    /// Konwersja dla typu  'bool'.
    template<> inline
    bool OptionalParameter<bool>::convert(const char* str)
    {
        return toupper(*str)=='Y' || toupper(*str)=='T' || (*str)=='1';
    }

    /// Szablon konwersji dla typów WYLICZENIOWYCH (enum).
    /// \details Podstawienie konwersji dostarczonej przez klasę bazową
    template<class T> inline
    T OptEnumParametr<T>::convert(const char* str)
    {
        if(('A'<=str[0] && str[0]<='Z')
                || ('a'<=str[0] && str[0]<='z')
                || (str[0] == '_') ) //Czy jest identyfikator
        {
            for(unsigned i=0;i<NofEn;i++)
                if(std::strcmp(str,EnNames[i])==0) //Jest?
                {
                    if(EnVals) return EnVals[i];
                    else return T(this->LBound+i);
                }
            return T(-9999);
        }
        else //W przeciwnym razie próbujemy liczbowo.
        {
            int pom=atol(str);
            return T(pom);
        }
    }

    ///  WERSJA OGÓLNA konwersji ZGŁASZAJĄCA AWARIE.
    ///\details Zostaje użyta przy próbie konwersji jakiegoś typu, który nie ma przygotowanej implementacji funkcji 'convert'.
    ///     Podstawowe typy i tak są obsłużone oddzielnie,
    ///     więc sprawa dotyczy enums i jakichś pomysłów na używanie klas użytkownika.
    ///     Na razie nic bardziej ogólnego nie wymyśliłem
    ///\return -9999, bo to zazwyczaj nie będzie poprawną wartością parametru.
    template<class T> inline
    T OptionalParameter<T>::convert(const char* str)
    {
      /* istrstream Strm(str);
        T Val;
        Strm>>Val;//Wiele typów i tak nie ma, np. różne enum
        */
        return T(-9999);
    }


    //*????
    //typedef OptionalParameter<class T> OptPar<class T>;  ??? TODO HOW TO DECLARE THAT?

    // Główne metody:
    // //////////////

    /// \brief Zapis parametrów do strumienia "raportu".
    /// \param Out to strumień.
    /// \param Parameters to tablica wszystkich parametrów (typów potomnych od `OptionalParameterBase`).
    /// \param Len to długość tej tablicy.
    /// \param SeparatorTab to separator pól.
    /// \param SeparatorLine to separator linii, czyli poszczególnych parametrów.
    MAYBE_UNUSED
    void OptionalParameterBase::report(ostream& Out,
                                       OptionalParameterBase* Parameters[],
                                       int  Len,
                                       const char* SeparatorTab,
                                       const char* SeparatorLine)
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

    /// \brief Zapis parametrów w formie tabeli.
    /// \param Out to strumień.
    /// \param Parameters to tablica wszystkich parametrów (typów potomnych od `OptionalParameterBase`).
    /// \param Len to długość tej tablicy.
    /// \param SepTab to separator pól.
    /// \param Head to chyba nagłówek tabeli.
    /// \param ValHd to ???.
    inline MAYBE_UNUSED
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

    /// \brief Główna funkcja parsująca listę parametrów wywołania.
    /// \param argc to liczba parametrów wywołania z funkcji `main`.
    /// \param argv to tabela parametrów wywołania z funkcji `main`.
    /// \param Parameters to tablica wszystkich parametrów (typów potomnych od `OptionalParameterBase`).
    /// \param Len to długość tej tablicy.
    /// \return 0 if OK, -1 on syntax error
    inline
    int OptionalParameterBase::parse_options(const int argc,const char* argv[],
                                             OptionalParameterBase* Parameters[],int  Len)
    {
        for(int i=1;i<argc;i++)
        {
            if( *argv[i]=='-' )
                continue;// Opcja X11 lub symshell'a, czy inne, które chcemy obsłużyć inaczej
            if(std::strcmp(argv[i],"HELP")==0) //Bez kwalifikacji `std::` może być kłopot w zakresie działania definicji friend'ów `wb_pchar`
            {
                cout<<endl<<"*** NAMES OF PARAMETERS:"<<endl<<flush;
                for(int j=0;j<Len;j++) Parameters[j]->HelpPrn(cout);
                cout<<"* USE A LIST OF SPACE SEPARATED PAIRS PARAMETER=VALUE"<<endl;
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
            cerr<<"The unknown parameter "<<argv[i]<<" detected."<<endl;
            return -1;
    CONTINUE:;
        }
        return 0;
    }

    /// \brief Szablon funkcji sprawdzania łańcucha pojedynczego parametru.
    /// \tparam T
    /// \param argv to wartość argumentu z funkcji `main`.
    /// \param sep to oczekiwany separator.
    /// \return 1 gdy OK, -1 gdy error, 0 jeśli to nie jest definicja dla tego obiektu.
    template<class T> inline
    int OptionalParameter<T>::CheckStr(const char* argv,char sep)
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
                cerr<<"** Proper values should be between "<<LBound<<" and "<<HBound<<endl;
                cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
                return -1;//Moja, ale niepoprawna wartość
            }
        }
        return 0;//Nie MOJA wartość. Szukaj dalej!
    }

    /// \brief  funkcji sprawdzania łańcucha parametru dla typu 'string'.
    /// \param argv to wartość argumentu z funkcji `main`.
    /// \param sep to oczekiwany separator.
    /// \return 1, gdy OK, -1, gdy error, jeśli to nie jest definicja dla tego obiektu to 0.
    template<> inline
    int OptionalParameter<string>::CheckStr(const char* argv,char sep)
    {
        const char* pom=NULL;
        if((pom=std::strstr(argv,Name.get()))!=NULL) //Bez kwalifikacji std:: może być kłopot w zakresie działania definicji friend'ów `wb_pchar`
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
                return 1; //Sygnał, że to "moja poprawna wartość".
            }
            else
            {
                cerr<<"* Value "<<Name.get()<<" cannot be changed into \""<<Value.c_str()<<'"'<<endl;
                cerr<<"** Proper value may look like \""<<LBound.c_str()<<"\" and \""<<HBound.c_str()<<"\""<<endl;
                cerr<<"* ("<<Name.get()<<":"<<Info.get()<<')'<<endl;
                return -1; //"Moja, ale niepoprawna wartość".
            }
        }
        return 0;//Nie MOJA sprawa
    }

// UNCONVENTIONAL HELP PRINTING METHODS FOR TEXT PARAMETERS:
// ///////////////////////////////////////////////////////////

    /// \brief Help printing function for 'string' type parameter.
    template<> inline
    void OptionalParameter<string>::HelpPrn(ostream& o)
    {
        o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.c_str()<<"\" or \""<<HBound.c_str()<<"\"; Default: \""<<Value.c_str()<<'"'<<endl;
    }

    /// \brief Help printing function for 'wb_pchar' type parameter.
    template<> inline
    void OptionalParameter<wb_pchar>::HelpPrn(ostream& o)
    {
        o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound.get()<<"\" or \""<<HBound.get()<<"\"; Default: \""<<Value.get()<<'"'<<endl;
    }

    /// \brief Help printing function for 'char*' type parameter.
    template<> inline
    void OptionalParameter<char*>::HelpPrn(ostream& o)
    {
        o<<Name.get()<<": "<<Info.get()<<" f.e.:\""<<LBound<<"\" or \""<<HBound<<"\"; Default: \""<<Value<<'"'<<endl;
    }

    /// \brief Help printing function for 'bool' type parameter.
    template<> inline
    void OptionalParameter<bool>::HelpPrn(ostream& o)
    { o<<Name.get()<<": "<<Info.get()<<"; allowed are: 0, 1, Yes, No, Tak, Nie; Default: "<<Value<<endl; }

    /// \brief Template function for help printing for an 'enum' ENUMERATION type parameter.
    template<class T> inline MAYBE_UNUSED
    void OptEnumParametr<T>::HelpPrn(ostream& o)
    { // G++ used to require this-> everywhere here ??? TODO - PERHAPS NO LONGER NECESSARY?
        o<<this->Name.get()<<": "<<this->Info.get()<<"; allowed are: ";
        for(unsigned i=0;i<this->NofEn;i++) // The array defines names from LBound to HBound!
        {
            o<<this->EnNames[i];
            if(this->EnVals)
                o<<"="<<this->EnVals[i];
            o<<' ';
        }
        o<<"or integers range: <"<<this->LBound<<','<<this->HBound<<"> Default: "
         <<this->EnNames[this->Value-this->LBound]<<"="<<this->Value<<endl;
    }

///@}

}//NAMESPACE WBRTM

/*v*******************************************************************/
/*               SYMSHELLLIGHT  version 2026                         */
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
