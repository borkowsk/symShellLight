/** \file wb_rand.hpp
 * \brief PODSTAWOWE KLASY GENERATORÓW LICZB PSEUDOLOSOWYCH
 * @date 2026-05-11 (ostatnia modyfikacja)
 *
 * \details
 *  - RandomGenerator - interfejs do generatorów losowych.
 *  - RandSTDC - generator losowy zbudowany w oparciu o standard C.
 *  - RandG - generator losowy napisany w C na podstawie "Numerical Recipes".
 * \author Wojciech Borkowski @ Instytut Studiów Społecznych UW.
 * @note Jeśli używasz biblioteki `SymShellLight` razem z pełną wersją WB_RTM to upewnij się, czy ten plik i jego
 *         wersja w pełnej bibliotece są zgodne. A najlepiej tak ustal kolejność ścieżek INCLUDE, żeby korzystać
 *         tylko z pełnej wersji.
 * ****************************************************************************************************************** */
#ifndef WB_RAND_HPP_INCLUDED_
#define WB_RAND_HPP_INCLUDED_  1

#ifndef __cplusplus
#error Obsługiwany jest tylko język C++!!!
#endif

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#ifndef unix
#include <sys/timeb.h>
#endif

/**
* @defgroup RandomNumbers  Liczby losowe.
* \brief Funkcjonalności związane z liczbami pseudolosowymi.
*/
/// @{

extern "C"
{
long    my_rand();          /**< \brief (CHRONIONY PRZEZ MUTEX?) `::rand()` dla programów wielowątkowych. */
void    my_srand(unsigned); /**< \brief Inicjalizacja dla `my_rand()`. */

float   randg();            /**< \brief Generator losowy z "Numerical Recipes". (TODO: DODAĆ MUTEX WEWNĄTRZ!). */
void    srandg(short int);  /**< \brief Ustawianie ziarna (seed) dla generatora. */
float   randnorm();         /**< \brief Znormalizowany wynik `randg`. */
float   randexp();          /**< \brief Wykładniczy wynik `randg`. */
}

/// \namespace wbrtm \brief BIBLIOTEKA RUN-TIME WOJCIECHA BORKOWSKIEGO.
namespace wbrtm {

    /// \brief Wirtualna klasa bazowa dla generatorów.
    /// \details Przydatna, gdy chcemy łatwo wymieniać generatory w obiekcie.
    class RandomGenerator
    {
    public:
        /// \brief Wartość maksymalna, jaką może wypluć `Rand`.
        virtual unsigned long RandomMax() = 0;

        /// \brief Funkcja dająca liczbę losową całkowitą z zakresu 0..RAND_MAX.
        virtual long   Rand() = 0;

        /// \brief Funkcja dająca liczbę losową rzeczywistą z zakresu 0..1 (rzadko dokładnie 1).
        virtual double DRand() = 0;

        /// \brief Zwraca `unsigned long` from `0` to `i`.
        virtual unsigned long Random(unsigned long i) = 0;

        /// \brief Inicjalizacja dla dobrze zdefiniowanej, powtarzalnej sekwencji.
        virtual void   Seed(unsigned long i) = 0;

        /// \brief Inicjalizacja dla losowo wybranej sekwencji.
        virtual void   Reset() = 0;

        /// Wirtualny destruktor dla bezpiecznego usuwania przez wskaźnik bazowy.
        virtual ~RandomGenerator() {};
    };

    /// \brief Klasa generatora G (Numerical Recipes).
    class RandG : public RandomGenerator
    {
    public:
        /// \brief Wartość maksymalna, jaką może wypluć `Rand`.
        unsigned long RandomMax() override { return (INT_MAX); }

        /// \brief Daje liczbę losową całkowitą z zakresu 0..RAND_MAX.
        long   Rand() override { return ((int) ((::randg)() * INT_MAX)); }

        /// \brief Zwraca `ulong` od `0` do `i`.
        unsigned long Random(unsigned long i) override {
            unsigned long ret = (unsigned long) (((double) (::randg)() * (i)));
            if (ret >= i) ret = i - 1;
            return ret;
        }

        /// \brief Daje liczbę losową rzeczywistą z zakresu 0..1 (rzadko dokładnie 1).
        double DRand() override  { return ((::randg)()); }

        /// \brief Znormalizowany wynik `randg`.
        double NormRand() { return ::randnorm(); }

        /// \brief Wykładniczy wynik `randg`.
        double ExpRand() { return ::randexp(); }

        /// \brief Inicjalizacja dla dobrze zdefiniowanej, powtarzalnej sekwencji.
        void Seed(unsigned long i) override  { ::srandg((short int) i); }

        /// \brief Inicjalizacja dla losowo wybranej sekwencji.
        void Reset() override { ::srandg((unsigned) time(NULL)); }

        /// \brief KONSTRUKTOR.
        RandG() { RandG::Reset(); }

        /// \brief DESTRUKTOR.
        ~RandG() override;
    };

    /// \brief Klasa generatora standardowego C.
    class RandSTDC : public RandomGenerator
    {
    public:
        /// \brief Wartość maksymalna, jaką może wypluć `Rand`.
        unsigned long RandomMax() override { return (RAND_MAX); }

        /// \brief Daje liczbę losową całkowitą z zakresu 0..RAND_MAX.
        long   Rand() override { return (my_rand)(); }

        /// \brief Zwraca `ulong` from `0` to `i`.
        unsigned long Random(unsigned long i) override { return (int) (((double) (my_rand)() * (i)) / ((double) RAND_MAX + 1)); }

        /// \brief Daje liczbę losową rzeczywistą z zakresu 0..1 (rzadko dokładnie 1).
        double DRand() override { return ((double)(my_rand)()) / (double)RAND_MAX; }

        /// \brief Inicjalizacja dla dobrze zdefiniowanej, powtarzalnej sekwencji.
        void Seed(unsigned long i) override { (::srand)(i); }

        /// \brief Inicjalizacja dla losowo wybranej sekwencji.
        void Reset() override { (::srand)((unsigned)time(NULL)); }

        /// \brief KONSTRUKTOR.
        RandSTDC() { RandSTDC::Reset(); }

        /// \brief DESTRUKTOR.
        ~RandSTDC() override;
    };

} //namespace

extern wbrtm::RandG    TheRandG;             ///< Gotowy do użycia generator używający `randg()`.
extern wbrtm::RandSTDC TheRandSTDC;          ///< Gotowy do użycia generator używający standardowego `rand()`.

/// @}

/* ***************************************************************** */
/*               WB_RTM for SymShell  version 2026                   */
/* ***************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*    GITHUB: https://github.com/borkowsk                            */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif





