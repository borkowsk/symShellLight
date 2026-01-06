/// @file
/// Created by borkowsk on 06.01.26.
/// @date 2026-01-06 (last modification)

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

#endif //SYMSHELL_ASSERTED_H
