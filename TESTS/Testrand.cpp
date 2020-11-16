///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
//Demonstruje skalowanie okna niezalezne od aplikacyjnej strony programu
//i robione w funkcjach symshella. Petla oblsugi zdarzen bez pracy w tle
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#define USES_SYMULATION_SHELL
#include "wb_uses.h"
#define USES_STDC_RAND
#include "randoms.h"
#include "wb_rand.hpp"
#include "wb_cpucl.hpp"

const BOK=128;
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
		int a=TheRandSTDC.Random(BOK);
		int b=TheRandSTDC.Random(BOK);

		assert(a>=0 && a<BOK);
		assert(a>=0 && a<BOK);
		
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

print(0,BOK,"%lu:<%g,%g> T:%g",counter,MinCount,MaxCount,double(Clock));
flush_plot();
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

main(int argc,char* argv[])
{
int i=0,xpos=0,ypos=0,click=0;//Myszowate
int cont=1;//flaga kontynuacji
int std=0;

mouse_activity(0);/*Mysz niepotrzebna */
set_background(128);
buffering_setup(1);/* Wlaczona animacja - tu potrzebna */
shell_setup("SYMSHELL - RANDS ",argc,argv);
printf("COLORS= 256 q-quit s-swich stdout on/off\n");
	   
RANDOMIZE();

if(!init_plot(BOK,BOK,1,1))
		{
      printf("%s\n","Can't initialize graphics");
      exit(1);
      }

while(cont)
{
int inpt;
// Czekaj na wejscie
while(!input_ready())
			replot();

inpt=get_char();
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
