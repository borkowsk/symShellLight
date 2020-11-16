///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
// Zawiera funkcje replot odrysowujaca glowny wzorek oraz symulacje symulacji w postaci
// rysowania rosnacych kolek przerywanego akcjami uzytkownika.
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#define USES_SYMULATION_SHELL /* rownowazne #include "symshell.h" */
#include "wb_uses.h"

int x,y,vx,vy;


int WB_error_enter_before_clean=1;// Wait for user in close_plot() 

void replot()//Repaint all screen - prymitywne ale skuteczne
{
unsigned int i,j;
unsigned char pom;
int old=mouse_activity(0);//Ewentualna dezaktywacja myszy na czas odrysowywania
clear_screen();

//Odrysowywanie "bitmapy"
for(i=0;i<256U; i++)
    for(j=0;j<256U; j++)
	  {
          pom=(unsigned char)(i&j);
          plot(j,i,pom);
	  }

//Odrysowywanie reszty
print(0,screen_height()-char_height('X'),"%s","ST:");
printc(char_width('X')*3,screen_height()-char_height('X'),1,140,"TEST ONLY");
fill_circle(screen_width(),0,10,240);
fill_circle(0,screen_height(),10,240);
line(screen_width(),0,0,screen_height(),251);
circle(screen_width()/2,0,25,200);
fill_circle(128,128,30,140);
print(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");

flush_plot();		//Koniec rysowania
mouse_activity(old);//Ewentualna aktywacja myszy
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

main(int argc,char* argv[])
{
int i=0,xpos=0,ypos=0,click=0;//Myszowate
int cont=1;//flaga kontynuacji
int std=0;

//INICJACJA APLIKACJI
mouse_activity(1);
set_background(128);
buffering_setup(0);/* Wylaczona animacja - tu niepotrzebna */
shell_setup("SYMSHELL TEST",argc,argv);
printf("COLORS= 256 q-quit s-swich stdout on/off\n"
	   "setup options:\n"
	" -mapped -buffered -bestfont -traceevt\n"
	" + for enable\n - for disable\n ! - for check\n");

if(!init_plot(256,256,0,1))
		{
      printf("%s\n","Can't initialize graphics");
      exit(1);
      }

//PETLA GLOWNA
while(cont)
{
int tab;

if(input_ready())//Jesli nie ma wejscia to robi swoje 
   {
	tab=get_char();//Jest wejscie. Nie powinien sie tu zatrzymac. Odczytuje.
   switch(tab)
   	{
    case '@':					//Reczne odpalanie replot()
   	case '\r':replot();break;	//Systemowe odpalanie replot()
	case '\b':get_mouse_event(&xpos,&ypos,&click);//Obsluga zdarzenia myszy
			  if(click&0x1) 
					set_char('X');
			  break;
	case 's':std=!std;break;	//Obsluga echa - specyficzne dla tego przykladu
   	case 'q':					//Reczne zakonczenie aplikacji
   	case EOF:					//Systemowe zakonczenie aplikacji (Close w MSWin, Ctrl-C wX11)
		cont=0; 
		break;
   	}

   i=0;//Reset promienia okregu
   if(std)//Implementacja echa
    {
	printf("stdout<<%c\n",tab);
	fflush(stdout);
    }
	printc(screen_width()-char_width('X'),
		 screen_height()-char_height('X'),
	  34,255,"%c",tab);
   }

//Robmy swoje!!!
fill_circle(xpos,ypos,(i%256+2)/2,i%256);
i++;
flush_plot();
}

//Zakonczenie dzialania aplikacji
close_plot();
printf("Do widzenia!!!\n"); 
return 0;
}
