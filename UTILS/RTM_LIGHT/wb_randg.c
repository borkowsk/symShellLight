/** \file
 *  \brief IMPLEMENTATION FOR PSEUDORANDOM NUMBER GENERATOR "randg". */
/*        ========================================================== */
/** @date 2026-05-11 (last modification) */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define USES_RANDG
#include "random.h"

static short int di=-1;

#ifdef __cplusplus
extern "C" {
#endif

float randg();
void  srandg(short int srandg_init_val);
float randnorm();
float randexp();

#ifdef __cplusplus
} //extern C
#endif

long my_rand()
{  //TODO - MUTEXOWE ZABEZPIECZENIE ::rand() na wielowątkowość ALE JAK TO ZROBIĆ W STD C ???
    return rand();
}

/*INT_MAX musi być 0x7fffffff*/
typedef long int32; /*Wg ANSI C long ma zawsze 32bity */

void srandg(short int srandg_init_val)
{                                                                 assert(srandg_init_val!=0);
    di=-abs(srandg_init_val);
}

/* randg generuje liczby losowe o rozkładzie prostym w przedziale (0,1) */
float randg()
{
  //TODO - MUTEXOWE ZABEZPIECZENIE ::rand() na wielowątkowość!!!
    static int16_t       inext, inextp;
    static int32_t       ma[55];
    int32_t              mj, mk, mbig = 1000000000;
    int16_t              i, k;

    if (di < 0)
    {
        mj = 161803398 - abs(di);
        mj %= mbig;
        ma[54] = mj;
        mk = 1;
        for (i=1; i <= 54; i++)
        {
            k = (21 * i) % 55;
            ma[k - 1] = mk;
            mk = mj - mk;
            if (mk < 0)
                mk += mbig;
            mj = ma[k - 1];
        };
        for (k = 1; k <= 4; k++)
            for (i = 1; i <= 55; i++)
            {
                ma[i - 1] -= ma[(i + 30) % 55];
                if (ma[i - 1] < 0)
                    ma[i - 1] += mbig;
            };
        inext = 0;
        inextp = 31;
        di= 1;
    };
    inext++;
    if (inext == 56)
        inext = 1;
    inextp++;
    if (inextp == 56)
        inextp = 1;
    mj=ma[inext - 1] - ma[inextp - 1];
    if (mj < 0)
        mj += mbig;
    ma[inext - 1] = mj;
    float tmp=(mj * 1.e-9f);                //assert(tmp<1.0f);
    if(tmp>=1) /* Czasem się tak jednak zdarza... */
        tmp=0.999999940395355224609375f;
    return tmp;/* CZY MOŻE BYC 1.e-9f czy winno BYC DOUBLE */
}

float randnorm()
{
    //float ran1(long *idum);
    static int iset=0;
    static float gset;
    float fac,rsq,v1,v2;
   
    //if (*idum < 0) iset=0;
    if  (iset == 0) 
    {
        do {
            v1=2.0*randg()-1.0;
            v2=2.0*randg()-1.0;
            rsq=v1*v1+v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);

        fac=sqrt(-2.0*log(rsq)/rsq);
        gset=v1*fac;
        iset=1;
        return v2*fac;
    } 
    else 
    {
        iset=0;
        return gset;
    }
}

float randexp()
{
    float dum;
   
    do
     dum=randg();
    while (dum == 0.0);

    return (float)( -log(dum) );
}

/* ********************************************* */
/* For WBRTM (light) FROM OLD Numerical Recipies */
/* ********************************************* */
