/** @file
 * @brief Template for swapping two values of the same type, regardless of the type. *
 * @date 2026-04-29 (last modification)                                              */
/*        ========================================================================== */
/* Created by borkowsk on 06.01.26.                                                  */
/* ********************************************************************************* */
#ifndef WB_SWAP_HPP_INCLUDED_
#define WB_SWAP_HPP_INCLUDED_

/**
 * @defgroup TypesService Type information and naming
 * \brief Nice and portable type names, type limits, etc.
 */
/// @{

/**
 * @brief Swaps the values of two objects of type T.
 * Classic swap implementation using a temporary variable.
 * @tparam T Type of objects being swapped.
 * @param[in,out] a Reference to the first object.
 * @param[in,out] b Reference to the second object.
 * @details
 * This implementation, while universal, may generate the following issues:
 * * __Performance:__ The function performs three copy operations (copy construction
 *          and two assignments). For large objects (e.g., `std::vector` containers or large matrices),
 *          this is a very time-consuming operation compared to the
 *          move semantics available since C++11.
 * * __Type requirements:__ Type `T` must have a __public copy constructor__
 *          and an assignment operator. If type `T` is "move-only" (like `std::unique_ptr`),
 *          this function will not compile.
 * * __Exceptions:__ If the copy constructor or assignment operator of type `T` can throw an exception,
 *          the function does not guarantee strong exception safety.
 *          In case the operation is interrupted halfway, objects `a` and `b` may remain in an
 *       undefined or inconsistent state.
 * * __Lack of optimization:__ Unlike `std::swap`, this function does not use specializations for types
 *          that can be swapped more efficiently (e.g., by swapping internal pointers only).
 * */
template <class T>
inline void wb_swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

#include <utility>
/**
 * @brief Swaps the values of two objects of type T, guaranteeing strong exception safety.
 * @details The function implements the "copy-and-swap" mechanism. If the process of creating
 *          temporary copies fails (throws an exception), the input objects 'a' and 'b' remain intact.
 * @note Requires type T to have a copy constructor and that move operations or the destructor
 *       do not throw exceptions (noexcept). Furthermore, it requires including "utility", and in that case
 *       one might as well use `std::swap` :-D ...
 * @tparam T Type of objects.
 * @param a First object.
 * @param b Second object.
 */
template <class T>
void wb_strong_swap(T& a, T& b) {
    // 1. "Commit or Rollback" phase: We create temporary copies.
    // If an exception is thrown here, a and b are untouched.
    T temp_a = a;
    T temp_b = b;

    // 2. Exception-free phase: We move the ready copies to the destination locations.
    // We use std::move, assuming that moving for T is noexcept.
    a = std::move(temp_b);
    b = std::move(temp_a);
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
#endif //WB_SWAP_HPP_INCLUDED_

