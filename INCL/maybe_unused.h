/// @file
/// @brief Uniwersalne makro do zaznaczania być może nieużywanych obiektów.
//         ================================================================
/// @date 2026-04-18 (last modification)
//  Created by borkowsk on 17.04.26.
// ////////////////////////////////////////////////////////////////////////
#ifndef WB_RTM_MAYBE_UNUSED_H_INCLUDED_
#define WB_RTM_MAYBE_UNUSED_H_INCLUDED_
/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief Ładne i przenośne nazwy typów, limity typów itp.
 */
/// @{

/// \brief Uniwersalne makro do zaznaczania być może nieużywanych obiektów.
/// Działa dla kompilatorów standardu C++17 albo starszych GNUC i clang. Dla pozostałych nie ma efektu.
/// \details Użycie:
/// ```
///  void funkcja(MAYBE_UNUSED int x) { }
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
