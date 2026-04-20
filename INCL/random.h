/** @file
 * @brief INTERFACE "C" DO ROZMAITYCH GENERATORÓW LICZB PSEUDOLOSOWYCH.
 * @date 2026-04-20 (last modification)                                */
/* -------------------------------------------------------------------- */
#ifndef WB_RTM_RANDOM_H_INCLUDED_
#define WB_RTM_RANDOM_H_INCLUDED_  (1)
#include <time.h> // Potrzebne, bo wszędzie jest time()

/**
* @defgroup RandomNumbers  Liczby losowe.
* \brief Funkcjonalności związane z liczbami pseudolosowymi.
*/
/// @{

/**
 *  @def USES_RANDG
 *  @brief Zdefiniuj takie makro przed włączeniem pliku "random.h", żeby użyć implementacji z numerical recipie.
 *
 *  @def USES_BSD_RANDOM
 *  @brief Zdefiniuj takie makro przed włączeniem pliku "random.h", żeby użyć implementacji systemu BSD.
 *
 *  @def USES_SVR4_DRAND
 *  @brief Zdefiniuj takie makro przed włączeniem pliku "random.h", żeby użyć implementacji systemu SVR4.
 *
 *  @def USES_STDC_RAND
 *  @brief Zdefiniuj takie makro przed włączeniem pliku "random.h", żeby użyć standardowego generatora języka C.
 *
 *	@def RANDOM_MAX
 *	@brief Największa wartość, jaką można uzyskać z generatora w wersji całkowitoliczbowej.
 *
 *	@def RAND
 *	@brief Losowanie liczby z zakresu 0...RANDOM_MAX.
 *
 *	@def RANDOM
 *	@brief Losowanie liczby z zakresu 0...parametr makra.
 *
 *	@def SRAND
 *	@brief Inicjowanie generatora jakąś liczbą całkowitą.
 *
 *	@def DRAND
  *	@brief Losowanie liczby "rzeczywistej" z zakresu 0...1.
 *
 *	@def RANDOMIZE
 *	@brief Inicjalizowanie generatora czasem systemowym z dokładnością do sekund (?).
 */

#if   defined( USES_RANDG )

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Różne funkcje związane z generowaniem liczb losowych zaimplementowane w języku C.
 * @details Nalezy oczekiwać że zwracają liczbę typu `float` z zakresu <0...1).
 * @{
 */

float  randg(void); 		/**< Generator z Numerical Recipies z lat 90-tych. */
void   srandg(short int);	/**< Inicjalizacja generatora konkretna liczba całkowitą. */
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

/** Funkcja dająca liczbę losową ze skrzywionego rozkładu.
 * Produkowana wartość jest zakresu 0..1, ale o rozkładzie albo gausso-podobnym (W>0)
 * albo pareto-podobnym (W<0). Może być też rozkład płaski, gdy W = 0 */
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


