/// @file
/// @brief   LIMITY DLA TYPÓW SKALARNYCH (Zaprojektowane, zanim podobne rozwiązanie pojawiło się w standardzie C++).
//           -------------------------------------------------------------------------------------------------------
/// @date 2026-04-20 (last modification)
/// \details Szablon klas implementujących limity dla skalarów
///          z możliwością dodania własnych specjalizacji.
///          Ten jest w przestrzeni nazw 'wbrtm::'.
///          Pomysł z czasów, gdy nie było tego jeszcze w standardzie C++
///          W przypadku konfliktu specjalizacji  zdefiniuj
///          ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H przed włączeniem
///          tego pliku
/// \author borkowsk
// ////////////////////////////////////////////////////////////////////////////////////////////

/// @ingroup OBSOLETE
#ifdef _MSC_VER
#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#ifndef WB_LIMITS_HPP_
#define WB_LIMITS_HPP_ (1)
#include <float.h>
#include <limits.h>

namespace wbrtm { // Przestrzeń nazw biblioteki WBRTM

/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief Ładne i przenośne nazwy typów, limity typów itp.
 */
/// @{

    /// \brief Szablon udostępniający limity dla poszczególnych typów skalarnych.
    template<class Scalar>
    class limit {
    public:
        static Scalar Max(); ///< Musi być zdefiniowane maksimum.
        static Scalar Min(); ///< Musi być zdefiniowane minimum.
    };

    // SPECJALIZACJE DLA TYPÓW WBUDOWANYCH:
    //*////////////////////////////////////
    /** \brief wersja dla `double` */ template<>
    inline double limit<double>::Max() { return DBL_MAX; }

    /** \brief wersja dla `float` */ template<>
    inline float limit<float>::Max() { return FLT_MAX; }

    /** \brief wersja dla `long int` */ template<>
    inline long limit<long>::Max() { return LONG_MAX; }

    /** \brief wersja dla `unsigned long` */ template<>
    inline unsigned long limit<unsigned long>::Max() { return ULONG_MAX; }

    /** \brief wersja dla `int` */ template<>
    inline int limit<int>::Max() { return INT_MAX; }

    /** \brief wersja dla `unsigned int` */ template<>
    inline unsigned int limit<unsigned int>::Max() { return UINT_MAX; }

    /** \brief wersja dla `short int` */ template<>
    inline short limit<short>::Max() { return SHRT_MAX; }

    /** \brief wersja dla `unsigned short` */ template<>
    inline unsigned short limit<unsigned short>::Max() { return USHRT_MAX; }

    /** \brief wersja dla `signed char` */ template<>
    inline signed char limit<signed char>::Max() { return CHAR_MAX; }

    /** \brief wersja dla `unsigned char` */ template<>
    inline unsigned char limit<unsigned char>::Max() { return UCHAR_MAX; }

    /** \brief wersja dla `bool` */ template<>
    inline bool limit<bool>::Max() { return true; }


    /** \brief wersja dla `double` */ template<>
    inline double limit<double>::Min() { return -DBL_MAX; }

    /** \brief wersja dla `float` */ template<>
    inline float limit<float>::Min() { return -FLT_MAX; }

    /** \brief wersja dla `long` */ template<>
    inline long limit<long>::Min() { return LONG_MIN; }

    /** \brief wersja dla `unsigned long` */ template<>
    inline unsigned long limit<unsigned long>::Min() { return 0; }

    /** \brief wersja dla `int` */ template<>
    inline int limit<int>::Min() { return INT_MIN; }

    /** \brief wersja dla `unsigned int` */ template<>
    inline unsigned int limit<unsigned int>::Min() { return 0; }

    /** \brief wersja dla `short` */ template<>
    inline short limit<short>::Min() { return SHRT_MIN; }

    /** \brief wersja dla `unsigned short` */ template<>
    inline unsigned short limit<unsigned short>::Min() { return 0; }

    /** \brief wersja dla `signed char` */ template<>
    inline signed char limit<signed char>::Min() { return CHAR_MIN; }

    /** \brief wersja dla `unsigned char` */ template<>
    inline unsigned char limit<unsigned char>::Min() { return 0; }

    /** \brief wersja dla `bool` */ template<>
    inline bool limit<bool>::Min() { return false; }

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

    /// W przypadku ogólnej implementacji `Max` liczymy na warning i inteligentną konwersję.
    template<class Scalar>
    inline Scalar limit<Scalar>::Max() { return DBL_MAX; }

    /// W przypadku ogólnej implementacji `Min` liczymy na warning i inteligentną konwersję.
    template<class Scalar>
    inline Scalar limit<Scalar>::Min() { return DBL_MIN; }

#endif

    /// \brief Klasa do tworzenia sensownego "missing values".
    /// \details Dla typów ze znakiem wartość "missing" to -maksimum, a dla typów bez znaku maksimum.
    template<class Scalar>
    class default_missing
    {
        Scalar miss;
    public:
        default_missing();

        operator Scalar() {
            return miss;
        }
    };

    /** \brief wersja dla `double` */ template<>
    inline default_missing<double>::default_missing() { miss = -DBL_MAX; } //TODO TU SIĘ ZMIENIŁO 2026 - PRZETESTUJ!

    /** \brief wersja dla `float` */ template<>
    inline default_missing<float>::default_missing() { miss = -FLT_MAX; } //TODO TU SIĘ ZMIENIŁO 2026 - PRZETESTUJ!

    /** \brief wersja dla `long` */ template<>
    inline default_missing<long>::default_missing() { miss = LONG_MIN; }

    /** \brief wersja dla `unsigned long` */ template<>
    inline default_missing<unsigned long>::default_missing() { miss = ULONG_MAX; }

    /** \brief wersja dla `int` */ template<>
    inline default_missing<int>::default_missing() { miss = INT_MIN; }

    /** \brief wersja dla `unsigned int` */ template<>
    inline default_missing<unsigned int>::default_missing() { miss = UINT_MAX; }

    /** \brief wersja dla `signed char` */ template<>
    inline default_missing<signed char>::default_missing() { miss = CHAR_MIN; }

    /** \brief wersja dla `unsigned char` */ template<>
    inline default_missing<unsigned char>::default_missing() { miss = UCHAR_MAX; }

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

    /// W wersji ogólnej liczymy na warning i w ostateczności inteligentną konwersję.
    template<class Scalar>
    inline default_missing<Scalar>::default_missing() { miss = -DBL_MAX; }

#endif

/// @}

} //namespaxe wbrtm

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
