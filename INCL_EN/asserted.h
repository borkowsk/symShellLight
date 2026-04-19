/** @file
 * @brief Numeric type conversion template secured with assertions. */
/* ============================================================ */
/** @date 2026-04-19 (last modification)
 * Created by borkowsk on 06.01.26. */

#ifndef WB_RTM_ASSERTED_H_
#define WB_RTM_ASSERTED_H_ (1)

#include <cassert>
#include <limits>

/**
 * @defgroup TypesServiceEN Type information and naming
 * \brief Clean and portable type names, type limits, etc.
 */
/// @{

/// \brief It does value "cast" with lowest-highest assertion.
/// \note Not usable for `long long` and `long double`.
/// \tparam T_VALUE target scalar type.
/// \tparam S_VALUE source scalar type.
/// \param value to be cast.
/// \return value cast from type S_VALUE into T_VALUE.
/// Example:
/// ```
///     double d=....
///     float f=asserted<float>(d);
/// ```
template<class T_VALUE,class S_VALUE>
inline
T_VALUE asserted(const S_VALUE& value)
{
    double tmp=(double)(value);
    assert( std::numeric_limits<T_VALUE>::lowest() <= tmp );
    assert( tmp <= std::numeric_limits<T_VALUE>::max() );
    return (T_VALUE)value;
}

/// @}

/* ****************************************************************** */
/*               WB_RTM for SymShell  version 2026                    */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                (Don't change or remove this note)  */
/* ****************************************************************** */
#endif //WB_RTM_ASSERTED_H_
