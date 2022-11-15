/////////////////////////////////////////////////////////////////////////////
///                     Przykładowy program SYMSHELL'A
///-------------------------------------------------------------------------
/// Demonstruje skalowanie okna niezależne od aplikacyjnej strony programu
/// i robione w funkcjach symshell'a.
//////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#define USES_STDC_RAND
#include "random.h"
#include "wb_rand.hpp"
#include "wb_cpucl.hpp"

using namespace wbrtm;

const int BOK=128;
int x,y,vx,vy;

double counters[BOK][BOK];

void replot()
{
    static unsigned long counter=0;
    counter++;
    double MaxCount=-DBL_MAX;
    double MinCount=DBL_MAX;
    unsigned int i,j;
    unsigned char pom;
    wb_cpu_clock Clock;

    clear_screen();

    for(i=0;i<BOK; i++)
        for(j=0;j<BOK; j++)
        {
            int a=TheRandSTDC.Random(BOK);      assert(a>=0 && a<BOK);
            int b=TheRandSTDC.Random(BOK);      assert(a>=0 && a<BOK);

            counters[a][b]++;

            if(counters[a][b]>MaxCount)
                MaxCount=counters[a][b];
            if(counters[a][b]<MinCount)
                MinCount=counters[a][b];
        }

    for(i=0;i<BOK; i++)
        for(j=0;j<BOK; j++)
        {
            pom=ssh_color((counters[i][j]-MinCount)/(MaxCount-MinCount)*256);
            plot(j,i,pom);
        }

    printbw(0,BOK,"%lu:%g..%g T:%g",counter,MinCount,MaxCount,double(Clock));//Nie można używać <> bo aktualnie wywalają SVG
    flush_plot();
}

/*  OGÓLNA FUNKCJA MAIN */
/************************/

int main(int argc,const char* argv[])
{
    int i=0,click=0; //xpos=0,ypos=0, to dane myszowe.
    int cont=1; //flaga kontynuacji
    int std=0;  //flaga użycia stdout

    mouse_activity(0);/* Mysz niepotrzebna */
    set_background(128);
    buffering_setup(1);/* Włączona animacja. Tu będzie potrzebna */
    shell_setup("SYMSHELL - RANDS",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n");

    if(!init_plot(BOK,BOK,1,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont)  //PĘTLA GŁÓWNA
    {
        int inpt;

        while(!input_ready()) // Czekaj na wejście
            replot(); // odrysowując

        inpt=get_char(); //Jest wejście. Trzeba przetworzyć.
        switch(inpt)
        {
        case 's':std=!std;break;
        case 'q':
        case EOF:
            cont=0;
            break;
        }

        if(std)
        {
            printf("stdout<<%c\n",inpt);
            fflush(stdout);
        }

        printc(BOK,BOK, 34,255,"%c",inpt);
        flush_plot();
    }

    close_plot();
    printf("Do widzenia!!!\n");
    return 0;
}
/********************************************************************/
/*              SYMSHELLLIGHT  version 2020-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
