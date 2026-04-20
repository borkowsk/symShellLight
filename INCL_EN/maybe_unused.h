
/// @file
/// @brief Universal macro for marking potentially unused objects.
//         ================================================================
/// @date 2026-04-20 (last modification)
//  Created by borkowsk on 17.04.26.
// ////////////////////////////////////////////////////////////////////////
#ifndef WB_RTM_MAYBE_UNUSED_H_INCLUDED_
#define WB_RTM_MAYBE_UNUSED_H_INCLUDED_
/**
 * @defgroup TypesServiceEN Type information and naming
 * \brief Clean and portable type names, type limits, etc.
 */
/// @{

/// \brief Universal macro for marking potentially unused objects.
/// Works for C++17 standard compilers or older GNUC and clang. For others, it has no effect.
/// \details Usage:
/// ```
///  void function(MAYBE_UNUSED int x) { }
/// ```
#if __cplusplus >= 201703L
#define MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

///@}

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
#endif //WB_RTM_MAYBE_UNUSED_H_INCLUDED_
