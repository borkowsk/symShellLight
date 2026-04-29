/** @file
 * @brief "C" INTERFACE FOR VARIOUS PSEUDORANDOM NUMBER GENERATORS.
 * @date 2026-04-29 (last modification)                                 */
/* -------------------------------------------------------------------- */
#ifndef WB_RTM_RANDOM_H_INCLUDED_
#define WB_RTM_RANDOM_H_INCLUDED_  (1)
#include <time.h> // Needed as time() is used everywhere

/**
* @defgroup RandomNumbersEN Random numbers.
* \brief Functionalities related to pseudorandom numbers.
*/
/// @{

/**
 * @def USES_RANDG
 * @brief Define this macro before including "random.h" to use the Numerical Recipes implementation.
 *
 * @def USES_BSD_RANDOM
 * @brief Define this macro before including "random.h" to use the BSD system implementation.
 *
 * @def USES_SVR4_DRAND
 * @brief Define this macro before including "random.h" to use the SVR4 system implementation.
 *
 * @def USES_STDC_RAND
 * @brief Define this macro before including "random.h" to use the standard C generator.
 *
 *	@def RANDOM_MAX
 *	@brief Maximum value that can be obtained from the integer version of the generator.
 *
 *	@def RAND
 *	@brief Draws a number from the range 0...RANDOM_MAX.
 *
 *	@def RANDOM
 *	@brief Draws a number from the range 0...macro parameter.
 *
 *	@def SRAND
 *	@brief Initializes the generator with an integer seed.
 *
 *	@def DRAND
  *	@brief Draws a "real" number from the range 0...1.
 *
 *	@def RANDOMIZE
 *	@brief Initializes the generator with system time (seconds).
 */

#if   defined( USES_RANDG )

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Various functions related to random number generation implemented in C.
 * @details Expected to return a `float` value in the range <0...1).
 * @{
 */

float  randg(void); 		/**< Random number generator from Numerical Recipes. */
void   srandg(short int);	/**< Seed setting for generator. */
float  randnorm(void);		/**< TODO... ??? */
float  randexp(void);		/**< TODO... ??? */

/** @} */
#ifdef __cplusplus
}
#endif

#	define RANDOM_MAX                       ( MAXINT )
#	define RAND()                           ( (int)(randg()*RANDOM_MAX) )
#	define RANDOM(_I_)                      ( (int)(randg()*(_I_)) )
#	define SRAND(_P_)                       { srandg(- (_P_) ); }
#	define DRAND()	                        ( randg() )
#	define RANDOMIZE()                      { srandg( (unsigned) time(NULL) ); }

#elif defined( USES_BSD_RANDOM )

#	if defined(IRIX32)
#	include <math.h>
#	endif

#	define RANDOM_MAX                       ( 0x7fffffffL)
#	define RAND()                           ( random() )
#	define RANDOM(_I_)                      ( (int) (((double) (random)() * (_I_) ) / ((double)RANDOM_MAX+1) ) )
#	define SRAND(_P_)                       { srandom(_P_);}
#	define DRAND()                          ( (double)random()/((double)(RANDOM_MAX)+1) )
#	define RANDOMIZE()                      { (srandom)( (unsigned) time(NULL) ); }

#elif defined( USES_SVR4_DRAND )

#   define RANDOM_MAX                       ( MAXINT)
#	define RAND() 	                        ( lrand48() ) /* CHECK RANGE! */
#	define RANDOM(_I_)                      ( drand48()*(_I_))
#	define SRAND(_P_)                       { srand48( _P_ ); }
#	define DRAND()                          ( drand48() )
#	define RANDOMIZE()                      { (srand48)( (long) time(NULL) ); }

#elif defined( USES_STDC_RAND )

#include <stdlib.h>
#	define RANDOM_MAX                       ( RAND_MAX )
#	define RAND() 	                        ( rand() )
#	define RANDOM(_I_)                      (int)(((double)rand()*(_I_))/((double)RAND_MAX+1))
#	define SRAND(_P_)                       { srand( _P_ ); }
#	define DRAND()                          ((double)rand()/(double)RAND_MAX)
//( double d=(double)rand()/(double)RAND_MAX,assert(d>0),d )
#	define RANDOMIZE()                      {  srand( (unsigned)time(NULL) ); }

#else /* NO USEABLE RANDOM FUNCTIONS */

#warning  Random functions set not selected!
#	define RANDOM_MAX                       ( RANDOM_FUNCTIONS_SET_NOT_SELECTED )
#	define RAND() 	                        ( RANDOM_FUNCTIONS_SET_NOT_SELECTED )
#	define RANDOM(_I_)                      ( RANDOM_FUNCTIONS_SET_NOT_SELECTED )
#	define SRAND(_P_)                       { RANDOM_FUNCTIONS_SET_NOT_SELECTED }
#	define DRAND()                          ( RANDOM_FUNCTIONS_SET_NOT_SELECTED )
#	define RANDOMIZE()                      { RANDOM_FUNCTIONS_SET_NOT_SELECTED }

#endif

#ifdef __cplusplus
//extern "C" {

/** Function providing a random number from a skewed distribution.
 * Produced value is in the range 0..1, but with either a Gaussian-like (W>0)
 * or Pareto-like (W<0) distribution. Can also be a flat distribution when W = 0. */
inline  double DRAND_LOOP(int W)
{
    double pom;
    int i;
    if(W>0)
    {
        pom=0;
        for(i=0;i<W;i++)
            pom+=DRAND();
        return pom/W;
    }
    else
    {
        pom=1;
        for(i=0;i<-W;i++)
            pom*=DRAND();
        return pom;
    }
}

//#ifdef __cplusplus
//}
#endif

/// @}

/* *******************************************************************/
/*                 SYMSHELLLIGHT  version 2026                       */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif


