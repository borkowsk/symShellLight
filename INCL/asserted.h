/// @file
/// Created by borkowsk on 06.01.26.
/// @date 2026-04-17 (last modification)

#ifndef SYMSHELL_ASSERTED_H
#define SYMSHELL_ASSERTED_H

#include <cassert>
#include <limits>

/// Value "cast" with lowest-highest assertion.
template<class T_VALUE,class S_VALUE> inline
T_VALUE asserted(const S_VALUE& value)
{
    assert( std::numeric_limits<T_VALUE>::lowest() <= (double)(value) );
    assert( (double)(value) <= std::numeric_limits<T_VALUE>::max() );
    return value;
}

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2026                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif //SYMSHELL_ASSERTED_H
