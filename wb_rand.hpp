/** PODSTAWOWE KLASY GENERATORÓW LICZB PSEUDOLOSOWYCH
*********************************************************
* \INTERFACE
* RandomGenerator
* \IMPLEMENTATIONS
* RandG	- Random generator wrote in C based on "Numerical Recipes"
* RandSTDC	- Random generator build in standard C
* \OBSOLETE
* RandBSD	- Random generator from BSD UNIX
* RandSVR4 - Random generator from System V UNIX
**/
#ifndef __cplusplus
#error Only C++ supported!!!
#endif

#ifndef _WB_RAND_HPP_INCLUDED_
#define _WB_RAND_HPP_INCLUDED_  1
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#ifndef unix
#include <sys/timeb.h>
#endif 

extern "C"
{
    long  my_rand(); //(MUTEXOWE ZABEZPIECZONE TODO!?) ::rand() na wielowątkowość
    float	randg(); //(MUTEXOWE ZABEZPIECZONY TODO!?) Numerical Recipes random number generator
	void	srandg(short int);  /* Seed setting for generator */
	float	randnorm();			/* Normalised output of randg */
	float	randexp();			/* Exponential output of randg */
}

/// Interfejs klasy generatora liczb losowych
class RandomGenerator
{
public:
    ///Max Value that can be returned from Rand()
	virtual 
		unsigned long RandomMax()=0;

    ///Returned unsigned long from 0 to RandomMax
	virtual 
		unsigned long Rand()=0;

    ///Returned unsigned long from 0 to i
	virtual 
		unsigned long Random(unsigned long i)=0;

    ///Returned double from <0 to 1)
	virtual
		double		  DRand()=0;

    ///Initialisation for well defined repeatable sequence
    virtual
		void		  Seed(unsigned long i)=0;

    ///Initialisation for random selected sequence
    virtual
		void		  Reset()=0;
};

/// Specjalizacja generatora losowego używająca funkcji randg()
/// randg jest zabezpieczone mutexem (?)
class RandG: public RandomGenerator
{
public:
    ///Max Value that can be returned from Rand()
	unsigned long RandomMax()
	{ return ( INT_MAX ); }

    ///Returned ulong from 0 to RandomMax
	unsigned long Rand()
	{ return  ( (int)((::randg)()*INT_MAX) ); }

    ///Returned ulong from 0 to i
	unsigned long Random(unsigned long i)
	{ unsigned long ret= (unsigned long)( ((double)(::randg)()*(i)) ); if(ret>=i) ret=i-1; return ret; }

    ///Returned double from <0 to 1)
	double		  DRand()
	{ return ( (::randg)() ); }

    ///Generation of normal distribution. Defined only for this class.
	double		  NormRand()
	{ return ::randnorm();}

    ///Generation of exponential distribution. Defined only for this class.
	double		  ExpRand()
	{ return ::randexp(); }

    ///Initialisation for well defined repeatable sequence
	void		  Seed(unsigned long i)
	{ ::srandg( (short int)i ); }

    ///Initialisation for random selected sequence
	void		  Reset() 
	{ ::srandg( (unsigned) time(NULL) ); }
	
	///CONSTRUCTOR
	RandG()
	{
		RandG::Reset();
	}
	
};

///Specjalizacja generatora losowego używająca standardowego generator języka C
class RandSTDC: public RandomGenerator
{
public:
    ///Max Value that can be returned from Rand()
	unsigned long RandomMax()
	{ return ( RAND_MAX ); }

    ///Returned ulong from 0 to RandomMax
	unsigned long Rand()
	{ return  my_rand(); }

    ///Returned ulong from 0 to i
	unsigned long Random(unsigned long i)
	{ return (int)(((double)(my_rand)()*(i))/((double)RAND_MAX+1)); }

    ///Returned double from <0 to 1)
	double		  DRand()
	{ return  ((double)(my_rand)())/(double)RAND_MAX; }

    ///Initialisation for well defined repeatable sequence
	void		  Seed(unsigned long i)
	{ (::srand)( i ); }

    ///Initialisation for random selected sequence
	void		  Reset() 
	{ (::srand)( (unsigned)time(NULL) );  }
	
	///CONSTRUCTOR
	RandSTDC()
	{
		RandSTDC::Reset();
	}
	
};

extern RandG TheRandG;
extern RandSTDC TheRandSTDC;

/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-24                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif





