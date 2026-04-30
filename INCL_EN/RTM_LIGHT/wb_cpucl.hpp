/// \file wb_cpucl.hpp
/// \brief CPU stopwatch class
/// @date 2026-04-29 (last modification)
/// \author borkowsk
/// \warning OBSOLETE
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    /// \brief   CPU stopwatch class.
    /// \details Counts time from creation or `reset` until value is retrieved using "operator `double`".
    class wb_cpu_clock
    {
        clock_t begin_val; ///< Saved initial value from `clock` constructor (or from `reset`)

    public:
        /// CONSTRUCTOR. Reads the initial clock time.
        wb_cpu_clock() { begin_val = clock(); }

        /// \brief Restart time counting.
        void reset() { begin_val = clock(); }

        /// \brief Convert current state to `double`.
        /// \return Time in seconds.
        operator double() const
        {
            return (clock() - begin_val) / (double)CLOCKS_PER_SEC;
        }

        /// \brief Print to stream.
        void PrettyPrint(ostream& o) const
        {
            double val = (double)(*this);
            if (val < 0.001)
                o << val * 1000000.0 << " [us]";
            else
            if (val < 1.0)
                o << val * 1000.0 << " [ms]";
            else
                o << val << " [s]";
        }

#ifdef USES_IOSTREAM
        /// \brief Helper class for elegant stream output in a general version.
        class  MAYBE_UNUSED pretty
        {
            const wb_cpu_clock& what;
        public:
            /// Constructor registers reference to the "clocker".
            pretty(const wb_cpu_clock& iw):what(iw){}
            friend
            /// Printing class "value" to a stream.
            ostream& operator << (ostream& o, const pretty& c)
            {
                c.what.PrettyPrint(o);
                return o;
            }
        };

#endif //defined USES_IOSTREAM

    };

} //end of namespace

typedef wbrtm::wb_cpu_clock cticker; ///< @brief Old name of this class, used in some very old programs.

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


