///////////////////////////////////////////////////////////////////////////////////////////
///                      Przykładowy program SYMSHELL'A
///-----------------------------------------------------------------------------------------
/// Zawiera funkcje replot() odrysowująca główny wzorek oraz symulacje symulacji ;-)
/// w postaci rysowania rosnących kołek przerywanego akcjami użytkownika.
///////////////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <cstdlib>
#include <cstdio>

int x,y,vx,vy;//Współrzędne i wektory prędkości

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
    printbw(0,screen_height()-char_height('X'),"%s","ST:");
    printc(char_width('X')*3,screen_height()-char_height('X'),1,140,"TEST ONLY");
    fill_circle(screen_width(),0,10,240);
    fill_circle(0,screen_height(),10,240);
    line(screen_width(),0,0,screen_height(),251);
    circle(screen_width()/2,0,25,200);
    fill_circle(128,128,30,140);
    printbw(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");

    flush_plot();       //Koniec rysowania
    mouse_activity(old);//Ewentualna aktywacja myszy
}

/*  OGÓLNA FUNKCJA MAIN */
/************************/

int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0;//Myszowate
    int cont=1;//flaga kontynuacji
    int std=0;

    //INICJACJA APLIKACJI
    mouse_activity(1);
    set_background(128);
    buffering_setup(0);/* Wyłączona animacja, bo tu niekonieczna */
    shell_setup("SYMSHELL TEST",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");

    if(!init_plot(256,256,0,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont) //PĘTLA GŁÓWNA
    {
        int tab;

        if(input_ready())//Jeśli nie ma wejścia to robi swoje
        {
            tab=get_char();//Jest wejście. Nie powinien się tu zatrzymać. Odczytuje.
            switch(tab)
            {
            case '@':					//Ręczne odpalanie replot()
            case '\r':replot();break;	//Systemowe odpalanie replot()
            case '\b':get_mouse_event(&xpos,&ypos,&click);//Obsługa zdarzenia myszy
                if(click&0x1)
                    set_char('X');
                break;
            case 's':std=!std;break;	//Obsługa echa.
            case 'q':					//Ręczne zakończenie aplikacji
            case EOF:					//Systemowe zakończenie aplikacji (Close w MSWin, Ctrl-C wX11)
                cont=0;
                break;
            }

            i=0;//Reset promienia okręgu
            if(std)//Implementacja echa
            {
                printf("stdout<<%c\n",tab);
                fflush(stdout);
            }

            printc(screen_width()-char_width('X'),
                   screen_height()-char_height('X'),
                   34,255,"%c",tab);
        }

        //Róbmy swoje!!!
        fill_circle(xpos,ypos,(i%256+2)/2,i%256);
        i++;
        flush_plot();
    }

    //Zakończenie działania aplikacji
    close_plot();
    printf("Do widzenia!!!\n");
    return 0;
}
/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-19                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
