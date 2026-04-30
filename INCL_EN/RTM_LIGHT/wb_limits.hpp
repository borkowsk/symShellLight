/// @file
/// @brief   LIMITS FOR SCALAR TYPES (Designed before a similar solution appeared in the C++ standard).
/// @date 2026-04-30 (last modification)
///
/// \details Class template implementing limits for scalars
///          with the possibility of adding custom specializations.
///          This one is in the 'wbrtm::' namespace.
///          Concept from times when this was not yet in the C++ standard.
///          In case of specialization conflict, define
///          ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H before including
///          this file.
/// \author borkowsk
/// \warning OBSOLETE
/// @note If you're using the `SymShellLight` library with the full version of WB_RTM, make sure this file and its version
///       in the full library are compatible. Ideally, arrange the include paths so that you only use the full version.
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @ingroup OBSOLETE
#ifdef _MSC_VER
#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#ifndef WB_LIMITS_HPP_
#define WB_LIMITS_HPP_ (1)
#include <float.h>
#include <limits.h>

namespace wbrtm { // Namespace for the WBRTM library

/**
 * @defgroup TypesServiceEN Type information and naming
 * \brief Clean and portable type names, type limits, etc.
 */
/// @{

    /// \brief Template providing a "missing value" (empty/null) for a given type.
    template<class Scalar>
    struct default_missing
    {
        Scalar miss; ///< The value that represents "no data".
        /// Constructor sets the default value for the type.
        default_missing();
    };

    /** \brief version for `double` */ template<>
    inline default_missing<double>::default_missing() { miss = -DBL_MAX; }

    /** \brief version for `float` */ template<>
    inline default_missing<float>::default_missing() { miss = -FLT_MAX; }

    /** \brief version for `long` */ template<>
    inline default_missing<long>::default_missing() { miss = LONG_MIN; }

    /** \brief version for `unsigned long` */ template<>
    inline default_missing<unsigned long>::default_missing() { miss = ULONG_MAX; }

    /** \brief version for `int` */ template<>
    inline default_missing<int>::default_missing() { miss = INT_MIN; }

    /** \brief version for `unsigned int` */ template<>
    inline default_missing<unsigned int>::default_missing() { miss = UINT_MAX; }

    /** \brief version for `signed char` */ template<>
    inline default_missing<signed char>::default_missing() { miss = CHAR_MIN; }

    /** \brief version for `unsigned char` */ template<>
    inline default_missing<unsigned char>::default_missing() { miss = UCHAR_MAX; }

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

    /// In the general version, we rely on a warning and ultimately intelligent conversion.
    template<class Scalar>
    inline default_missing<Scalar>::default_missing() { miss = -DBL_MAX; }

#endif

/// @}

} //namespace wbrtm

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
#endif
