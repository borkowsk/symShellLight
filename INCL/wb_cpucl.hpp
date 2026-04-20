/// \file wb_cpucl.hpp
/// \brief Klasa stopera liczącego czas CPU
//        ==================================
/** @date 2026-04-20 (last modification */
/// \author borkowsk
/// \warning OBSOLETE
// ///////////////////////////////////////////////////////
#ifndef __CPU_CLOCK_HPP_
#define __CPU_CLOCK_HPP_

#define USES_IOSTREAM
#include <time.h>
#include <iostream>
#include "asserted.h"
#include "maybe_unused.h"

/// \namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY.
namespace wbrtm {
    using std::ostream;
    using std::istream;

    /// \brief   Klasa stopera liczącego czas CPU.
    /// \details Liczy czas od momentu utworzenia lub wywołania `reset` do chwili pobrania wartości za pomocą "operator `double`".
    class wb_cpu_clock
    {
        clock_t begin_val; ///< Zapamiętana wartość początkowa z konstruktora `clock` (albo z `reset`)

    public:
        /// CONSTRUCTOR. Czyta początkowy czas zegara.
        wb_cpu_clock() { begin_val = clock(); }

        /// \brief Restart liczenia czasu.
        void reset() { begin_val = clock(); }

        /// \brief Konwersja aktualnego stanu na wartość `double`.
        operator double() const
        {
            return (double(clock()) - double(begin_val)) /
#ifdef __BORLAND__ //????
                    CLK_TCK;
#else
                    CLOCKS_PER_SEC;
#endif
        }

        /// \brief Obsolete! Jawne wywołanie destruktora, trochę na wzór języka JAVA. Chociaż całkiem nadmiarowe.
        void finalise()
        {
            this->~wb_cpu_clock();
        }

#ifndef USES_IOSTREAM
    }; //OD RAZU KONIEC KLASY
#else //DEFINED USES_IOSTREAM

        /// Klasyczne wypisywanie na strumień.
        friend
        ostream& operator << (ostream& o, const wb_cpu_clock& c)
        { o<<double(c)<<"s "; return o; }

        /// Wymyślne wypisywanie na strumień.
        void PrettyPrint(ostream& o) const
        {
            double pom=double(*this);
            long pmm=0;
            if(pom>3600)
            {
                pmm=asserted<long>(pom/3600);
                o<<pmm<<"h ";
                pom-=pmm*3600;
            }
            if(pom>60)
            {
                pmm=asserted<long>(pom/60);
                o<<pmm<<"m ";
                pom-=pmm*60;
            }
            o<<pom<<"s ";

            //if(pom!=double(*this))
            //	o<<"DEBUG["<<double(*this)<<"s]";
        }

    };

    /// Wymuszanie eleganckiego wypisywania na strumień w wersji dosyć generalnej.
    class  MAYBE_UNUSED pretty
    {
        const wb_cpu_clock& what;
    public:
        /// Konstruktor rejestruje referencje do "clockera".
        pretty(const wb_cpu_clock& iw):what(iw){}
    friend
        /// Wypisanie "wartości" klasy na strumień.
        ostream& operator << (ostream& o, const pretty& c)
        {
            c.what.PrettyPrint(o);
            return o;
        }
    };

#endif //defined USES_IOSTREAM

} //end of namespace

typedef wbrtm::wb_cpu_clock cticker; ///< @brief Stara nazwa tej klasy, używana w niektórych bardzo starych programach.

/* ***************************************************************** */
/*               WB_RTM for SymShell  version 2026                   */
/* ***************************************************************** */
/*             THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*              W O J C I E C H   B O R K O W S K I                  */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif //__CPU_CLOCK_HPP_


