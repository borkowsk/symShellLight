/** \file wb_rand.hpp
 * \brief BASIC CLASSES OF PSEUDORANDOM NUMBER GENERATORS
 * @date 2026-04-29 (last modification)
 *
 * \details
 *       - RandomGenerator - interface to random generators
 *       - RandSTDC	- Random generator build in standard C
 *       - RandG	- Random generator wrote in C based on "Numerical Recipes"
 * \author Wojciech Borkowski @ Instytut Studiów Społecznych UW
 * ****************************************************************************************************************** */
#ifndef _WB_RAND_HPP_INCLUDED_
#define _WB_RAND_HPP_INCLUDED_  1

#ifndef __cplusplus
#error Of course, only C++ supported!
#endif

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#ifndef unix
#include <sys/timeb.h>
#endif

/**
* @defgroup RandomNumbersEN Random numbers.
* \brief Functionalities related to pseudorandom numbers.
*/
/// @{

extern "C"
{
long    my_rand();          /**< \brief (MUTEX PROTECTED?) `::rand()` for multithreaded programs. */
void    my_srand(unsigned); /**< \brief Initialisation for `my_rand()`. */

float	randg();            /**< \brief Numerical Recipes random number generator. (TODO MUTEX INSIDE!). */
void	srandg(short int);  /**< \brief Seed setting for generator. */
float	randnorm();			/**< \brief Normalised output of `randg`. */
float	randexp();			/**< \brief Exponential output of `randg`. */
}

/// \namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// \brief Random number generator class interface.
class RandomGenerator {
    public:
        /// \brief Max Value that can be returned from `Rand ()`.
        virtual unsigned long RandomMax() = 0;

        /// \brief Returned `unsigned long` from `0` to `RandomMax`.
        virtual unsigned long Rand() = 0;

        /// \brief Returned `unsigned long` from `0` to `i`.
        virtual unsigned long Random(unsigned long i) = 0;

        /// \brief Returned `double` from <0 to 1).
        virtual double DRand() = 0;

        /// \brief Initialisation for a well-defined repeatable sequence.
        virtual void Seed(unsigned long i) = 0;

        /// \brief Initialisation for a random draw sequence.
        virtual void Reset() = 0;

        /// \brief Required virtual destructor.
        virtual ~RandomGenerator(){};
    };

/// \brief Random generator specialization using the `randg ()` function.
/// \note `randg` is (still not!) secured with a mutex.
class RandG : public RandomGenerator {
    public:
        /// \brief Max Value that can be returned from `Rand ()`.
        unsigned long RandomMax() override { return (INT_MAX); }

        /// \brief Returned `ulong` from `0` to `RandomMax`.
        unsigned long Rand() override { return ((int) ((::randg)() * INT_MAX)); }

        /// \brief Returned `ulong` from `0` to `i`.
        unsigned long Random(unsigned long i) override {
            unsigned long ret = (unsigned long) (((double) (::randg)() * (i)));
            if (ret >= i) ret = i - 1;
            return ret;
        }

        /// \brief Returned `double` from <0 to 1).
        double DRand() override { return ((::randg)()); }

        /// \brief Generation of normal distribution. Defined only for this class.
        double NormRand() { return ::randnorm(); }

        /// \brief Generation of exponential distribution. Defined only for this class.
        double ExpRand() { return ::randexp(); }

        /// \brief Initialisation for a well-defined repeatable sequence.
        void Seed(unsigned long i) override { ::srandg((short int) i); }

        /// \brief Initialisation for a random draw sequence.
        void Reset() override { ::srandg((unsigned) time(NULL)); }

        /// \brief CONSTRUCTOR.
        RandG() { RandG::Reset(); }

        /// \brief DESTRUCTOR.
        ~RandG() override;
    };

///  \brief A random generator specialization that uses the standard C language generator.
class RandSTDC : public RandomGenerator {
    public:
        /// \brief Max value that can be returned from `Rand()`.
        unsigned long RandomMax() override { return (RAND_MAX); }

        /// \brief Returned `ulong` from `0` to `RandomMax`.
        unsigned long Rand() override { return my_rand(); }

        /// \brief Returned `ulong` from `0` to `i`.
        unsigned long Random(unsigned long i) override { return (int) (((double) (my_rand)() * (i)) / ((double) RAND_MAX + 1)); }

        /// \brief Returned `double` from <0 to 1).
        double DRand() override { return ((double) (my_rand)()) / (double) RAND_MAX; }

        /// \brief Initialisation for a well-defined repeatable sequence.
        void Seed(unsigned long i) override { (::srand)(i); }

        /// \brief Initialisation for a random selected sequence.
        void Reset() override { (::srand)((unsigned) time(NULL)); }

        /// \brief CONSTRUCTOR.
        RandSTDC() {
            RandSTDC::Reset();
        }

        ~RandSTDC() override;

    };

} //namespace

extern wbrtm::RandG    TheRandG;             ///< Ready to use generator using `randg()`.
extern wbrtm::RandSTDC TheRandSTDC;          ///< Ready to use generator using standard `rand()`.

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





