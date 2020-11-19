// Klasa stopera liczacego czas CPU od momentu
// utworzenia lub wywolania reset() do chwili pobrania
// wartosci za pomoca "operator double()"
/////////////////////////////////////////////////////////
#ifndef __CPU_CLOCK_HPP_
#define __CPU_CLOCK_HPP_

#include <time.h>
//#include "platform.hpp"

#include <iostream>

//#include "wbminmax.hpp"

class wb_cpu_clock
{
clock_t begin_val;

public:
wb_cpu_clock()
{ begin_val=clock(); }

void reset()
{ begin_val=clock(); }

operator double() const
{
return (double( clock() ) - double( begin_val ))/
        CLOCKS_PER_SEC;
#ifdef __BORLAND__ //????
	 CLK_TCK; 
#endif
}

void finalise()
{
this->~wb_cpu_clock();
}

#ifndef USES_IOSTREAM
};
#else //DEF

friend
ostream& operator << (ostream& o, const wb_cpu_clock& c)
{ o<<double(c)<<"s ";return o; }

void PrettyPrint(ostream& o) const
{
	double pom=double(*this);
	double pmm=0;
	if(pom>3600)
	{
		pmm=trunc(pom/3600);
		o<<pmm<<"h ";
		pom-=pmm*3600;
	}
	if(pom>60)
	{
		pmm=trunc(pom/60);
		o<<pmm<<"m ";
		pom-=pmm*60;
	}
	o<<pom<<"s ";
	
	//if(pom!=double(*this))
	//	o<<"DEBUG["<<double(*this)<<"s]";
}

};

/* SLICZNE ALE SIE NIE KOMPILUJE - DIABLI WIEDZA DLACZEGO
typedef const wb_cpu_clock& const_wb_cpu_clock_ref;
IOMANIPdeclare(const_wb_cpu_clock_ref)
inline ostream& __pretty(ostream& s,const_wb_cpu_clock_ref _cl) { _cl.PrettyPrint(s); return s; }
inline SMANIP(const_wb_cpu_clock_ref)      pretty(const_wb_cpu_clock_ref _c) { return SMANIP(const_wb_cpu_clock_ref)(__pretty, _c); }
*/

//Wersja mniej generalna, ale tez powinna dzialac
class pretty
{
    const wb_cpu_clock& what;
public:
    pretty(const wb_cpu_clock& iw):what(iw){}
friend
    ostream& operator << (ostream& o, const pretty& c)
    {
        c.what.PrettyPrint(o);
        return o;
    }
};

#endif

typedef wb_cpu_clock cticker;

/********************************************************************/
/*              SYMSHELLLIGHT  version 2020-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif //__CPU_CLOCK_HPP_


