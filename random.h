/* INTERFACE DO ROZMAITYCH GENERATOROW LICZB PSEUDOLOSOWYCH */
/*----------------------------------------------------------*/
#ifndef __RANDOM__H__INCLUDED__
#define __RANDOM__H__INCLUDED__  (1)
#include <time.h> //Potrzebne bo wszedzie time()

#if   defined( USES_RANDG )

#ifdef __cplusplus
extern "C" {
#endif
float  randg(void); 			/* Random number generator from Numerical Recipies*/
void   srandg(short int);	/* Seed setting for generator */
float randnorm(void);
float randexp(void);
#ifdef __cplusplus
}
#endif

#	define RANDOM_MAX  ( MAXINT )
#	define RAND()      ( (int)(randg()*RANDOM_MAX) )
#	define RANDOM(_I_) ( (int)(randg()*(_I_)) )
#	define SRAND(_P_)  { srandg(- (_P_) ); }
#	define DRAND()	   ( randg() )
#	define RANDOMIZE() { srandg( (unsigned) time(NULL) ); }

#elif defined( USES_BSD_RANDOM )

#	if defined(IRIX32)
#	include <math.h>
#	endif

#	define RANDOM_MAX  ( 0x7fffffffL)
#	define RAND()      ( random() )
#	define RANDOM(_I_) ( (int) (((double) (random)() * (_I_) ) / ((double)RANDOM_MAX+1) ) )
#	define SRAND(_P_)  { srandom(_P_);}
#   define DRAND()     ( (double)random()/((double)(RANDOM_MAX)+1) )
#	define RANDOMIZE() { (srandom)( (unsigned) time(NULL) ); }

#elif defined( USES_SVR4_DRAND )

#   define RANDOM_MAX  ( MAXINT)
#	define RAND() 	   ( lrand48() ) /* CHECK RANGE! */
#	define RANDOM(_I_) ( drand48()*(_I_))
#	define SRAND(_P_)  { srand48( _P_ ); }
#	define DRAND()     ( drand48() )
#   define RANDOMIZE() { (srand48)( (long) time(NULL) ); }

#elif defined( USES_STDC_RAND )

#include <stdlib.h>
#	define RANDOM_MAX  ( RAND_MAX )
#	define RAND() 	   ( rand() )
#	define RANDOM(_I_) (int)(((double)rand()*(_I_))/((double)RAND_MAX+1))
#	define SRAND(_P_)  { srand( _P_ ); }
#	define DRAND()     ((double)rand()/(double)RAND_MAX)
//( double d=(double)rand()/(double)RAND_MAX,assert(d>0),d )
#	define RANDOMIZE() {  srand( (unsigned)time(NULL) ); }

#else /* NO USEABLE RANDOM FUNCTIONS */

#	define RANDOM_MAX         ( SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  )
#	define RAND() 	   ( SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  )
#	define RANDOM(_I_) ( SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  )
#	define SRAND(_P_)  { SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  }
#   define DRAND()     ( SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  )
#	define RANDOMIZE() { SELECT_RANDOM_NOT_DEFINED_FOR_THIS_CODE  }

#endif

#ifdef __cplusplus
//extern "C" {

inline  //Funkcja daj¹ca liczbê losow¹ z zakresu 0..1 ale o rozk³adzie albo
//gaussopodobnym (W>0) albo paretopodobnym (W<0) lub 1 gdy W=0
double DRAND_LOOP(int W)
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

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://www.iss.uw.edu.pl/borkowski                  */
/*        MAIL: wborkowski@uw.edu.pl                                */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif


