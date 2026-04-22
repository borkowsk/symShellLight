/** @file
 * @brief Szablon konwersji typów liczbowych zabezpieczonej asercjami. */
/* ============================================================ */
/** @date 2026-04-20 (ostatnia modyfikacja)
 * Autor: borkowsk, 06.01.26. */

#ifndef WB_RTM_ASSERTED_H_
#define WB_RTM_ASSERTED_H_ (1)

#include <cassert>
#include <limits>

/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief Przejrzyste i przenośne nazwy typów, limity typów itp.
 */
/// @{

/// \brief Wykonuje rzutowanie wartości z asercją sprawdzającą zakres (minimum-maksimum).
/// \note Nie do użytku z typami `long long` oraz `long double`.
/// \tparam T_VALUE docelowy typ skalarny.
/// \tparam S_VALUE źródłowy typ skalarny.
/// \param value wartość do rzutowania.
/// \return wartość rzutowana z typu S_VALUE na T_VALUE.
/// Przykład:
/// ```
///     double d=....
///     float f=asserted<float>(d);
/// ```
template<class T_VALUE,class S_VALUE>
inline
T_VALUE asserted(const S_VALUE& value)
{
    double tmp=static_cast<double>(value);  // NOLINT(*-narrowing-conversions)
    assert( std::numeric_limits<T_VALUE>::lowest() <= tmp );
    assert( tmp <= std::numeric_limits<T_VALUE>::max() );
    return static_cast<T_VALUE>(value);
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
