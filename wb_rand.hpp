/* PODSTWOWE KLASY GENERATOROW LICZB PSEUDOLOSOWYCH			*/
//////////////////////////////////////////////////////////////
/* INTERFACE: */
/* RandomGenerator */
/* FLAT: */
/* RandG	- Random generator writed in C based on "Numerical Recipes" */
/* RandBSD	- Random generator from BSD UNIX  */
/* RandSVR4 - Random generator from System V UNIX */
/* RandSTDC	- Reandom generator buildin standard C */
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
        long  my_rand(); //(MUTEXOWE ZABEZPIECZONE TODO!) ::rand() na wielowątkowość
        float	randg(); //(MUTEXOWE ZABEZPIECZONY TODO!) Num.Rec. random number generator
	void	srandg(short int);    		/* Seed setting for generator */
	float	randnorm();			/* Normalised output of randg */
	float	randexp();			/* Expotential output of randg */
}

class RandomGenerator
/////////////////////////////////////////////////
{
public:
	virtual 
		unsigned long RandomMax()=0;    //Max Value that can be returned from Rand()
	virtual 
		unsigned long Rand()=0;			//Returned ulong from 0 to RandomMax
	virtual 
		unsigned long Random(unsigned long i)=0;//Returned ulong from 0 to i
	virtual
		double		  DRand()=0;		//Returned double from <0 to 1)
	virtual
		void		  Seed(unsigned long i)=0;
	virtual
		void		  Reset()=0;
};

class RandG: public RandomGenerator
///////////////////////////////////////////////////
// randg jest zabezpieczone mutexem
{
public:
	
	unsigned long RandomMax()
		//Max Value that can be returned from Rand()
	{ return ( INT_MAX ); }
	
	unsigned long Rand()
		//Returned ulong from 0 to RandomMax
	{ return  ( (int)((::randg)()*INT_MAX) ); }
	
	unsigned long Random(unsigned long i)
		//Returned ulong from 0 to i
	{ unsigned long ret= (unsigned long)( ((double)(::randg)()*(i)) ); if(ret>=i) ret=i-1; return ret; }
	
	double		  DRand()
		//Returned double from <0 to 1)
	{ return ( (::randg)() ); }
	
	double		  NormRand()
	{ return ::randnorm();}

	double		  ExpRand()
	{ return ::randexp(); }

	void		  Seed(unsigned long i)
	{ ::srandg( (short int)i ); }
	
	void		  Reset() 
	{ ::srandg( (unsigned) time(NULL) ); }
	
	//CONSTRUCTOR
	RandG()
	{
		RandG::Reset();
	}
	
};


class RandSTDC: public RandomGenerator
{
public:
	unsigned long RandomMax()
		//Max Value that can be returned from Rand()
	{ return ( RAND_MAX ); }
	
	unsigned long Rand()
		//Returned ulong from 0 to RandomMax
	{ return  my_rand(); }
	
	unsigned long Random(unsigned long i)
		//Returned ulong from 0 to i
	{ return (int)(((double)(my_rand)()*(i))/((double)RAND_MAX+1)); }
	
	double		  DRand()
		//Returned double from <0 to 1)
	{ return  ((double)(my_rand)())/(double)RAND_MAX; }
	
	void		  Seed(unsigned long i)
	{ (::srand)( i ); }
	
	void		  Reset() 
	{ (::srand)( (unsigned)time(NULL) );  }
	
	//CONSTRUCTOR
	RandSTDC()
	{
		RandSTDC::Reset();
	}
	
};

extern RandG TheRandG;
extern RandSTDC TheRandSTDC;


#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        MAIL: wborkowski@uw.edu.pl                                */
/* XII 2017                      (Don't change or remove this note) */
/********************************************************************/




