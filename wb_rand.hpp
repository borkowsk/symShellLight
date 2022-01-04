/** \file wb_rand.hpp
*  \brief BASIC CLASSES OF PSEUDORANDOM NUMBER GENERATORS
*  \author Wojciech Borkowski @ Instytut Studiów Społecznych UW
*  \details
* \n RandomGenerator - interface to random generators
* \n RandSTDC	- Random generator build in standard C
* \n RandG	- Random generator wrote in C based on "Numerical Recipes"
*/
/*
* \OBSOLETE
* \n RandBSD	- Random generator from BSD UNIX
* \n RandSVR4 - Random generator from System V UNIX
*/
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
    long    my_rand(); /**< (MUTEX-OWO PROTECTED) :: rand () for multithreaded programs */
    float	randg();   /**< Numerical Recipes random number generator (TODO MUTEX-OWE ZABEZPIECZENIE) */
	void	srandg(short int);  /**< Seed setting for generator */
	float	randnorm();			/**< Normalised output of randg */
	float	randexp();			/**< Exponential output of randg */
}

/// Random number generator class interface
class RandomGenerator
{
public:
    ///Max Value that can be returned from Rand()
	virtual unsigned long RandomMax()=0;

    ///Returned unsigned long from 0 to RandomMax
	virtual unsigned long Rand()=0;

    ///Returned unsigned long from 0 to i
	virtual unsigned long Random(unsigned long i)=0;

    ///Returned double from <0 to 1)
	virtual double		  DRand()=0;

    ///Initialisation for well defined repeatable sequence
    virtual void		  Seed(unsigned long i)=0;

    ///Initialisation for random selected sequence
    virtual void		  Reset()=0;
};

/// Random generator specialization using the randg () function. \details randg is (?) secured with a mutex.
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

/// A random generator specialization that uses the standard C language generator
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

extern RandG    TheRandG;             ///< ready to use generator using randg()
extern RandSTDC TheRandSTDC;          ///< ready to use generator using standard rand()

/* ******************************************************************/
/*              SYMSHELLLIGHT  version 2022-01-04                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif





