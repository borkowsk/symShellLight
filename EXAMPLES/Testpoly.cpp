///////////////////////////////////////////////////////////////////////////////////////////
///							Przykładowy program SYMSHELL'A
///-----------------------------------------------------------------------------------------
/// Demonstracja użycia poligonów i pseudo 3D słupków. Klikanie myszą powoduje
/// cykliczne dodawanie wieloboków do listy. Funkcja replot() odrysowuje słupki
/// i poligony.
/// Pętla obsługi podobna do tej w 'testsyms.cpp', ale oczekuje na zdarzenia
/// i nie umożliwia pracy w tle (nie używa input_ready() )
///////////////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <cstdio>
#include <cstdlib>

//int x,y,vx,vy;
/* For close_plot() */
int WB_error_enter_before_clean=1;

//BAR3D
const int a=10;
const int b=10;
const int c=6;
const int WHITE=255;

void slupek(int x,int y,int h,unsigned char col1,unsigned char col2)
{
    ssh_point romb[7];
                                           /*      6 -----  5    */
    romb[1].x= x;                          /*     /        / |   */
    romb[1].y= y - h;                      /*    1 ------ 2  |   */
    romb[2].x= x + a;                      /*    |        |  |   */
    romb[2].y= romb[1].y;                  /*    |        |  |   */
    romb[3].x= romb[2].x;                  /*    |        |  |   */
    romb[3].y= y;                          /*    |        |  4   */
    romb[4].x= x + a + b;                  /*    |        | /  c */
    romb[4].y= y - c;                      /*  x,y ------ 3      */
    romb[5].x= romb[4].x;                  /*         a      b   */
    romb[5].y= y - h - c;
    romb[6].x= x + b;
    romb[6].y= romb[5].y;

    fill_rect(x,y-h,x+a,y,col1);               /*front*/

    fill_poly(0,0,romb+1,6,col2);              /*bok i gora*/

    line(x,y,romb[1].x,romb[1].y,WHITE);       /*lewy pion*/

    line(x,y,romb[3].x,romb[3].y,WHITE);       /*doln poziom*/

    line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,WHITE); /*blik*/

    //plot(romb[5].x,romb[5].y,WHITE-1);

    line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,WHITE);

} /* end  slupek() */


//Table for point list
const int numi=100;
struct polydata{
		ssh_point p;
        unsigned char color;
      };
polydata list[numi];

void replot()
{
    static const ssh_point points[]={{-10,-10},{13,-13},{13,13},{-10,10},{-15,0}};
    static const int pnumber=sizeof(points)/sizeof(points[0]);
    unsigned int i;
    int old=mouse_activity(0);
    clear_screen();
    for(i=0;i<numi;i++)
    {
        if(list[i].p.x==0) break;
        //fill_circle(list[i].p.x, list[i].p.y,30,list[i].color);
        fill_poly(list[i].p.x, list[i].p.y,points,pnumber,list[i].color);
    }
    for(i=0;i<16;i++)
    {
        slupek(i*12,255,i*10+10,i*16,i*16-8);
    }
    printbw(0,screen_height()-char_height('X'),"%s","ST:");
    printc(char_width('X')*3,screen_height()-char_height('X'),1,140,"POLYGON & BAR3D TEST");

    printbw(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");
    flush_plot();
    mouse_activity(old);
}

/*  OGÓLNA FUNKCJA MAIN  */
/* ********************** */

int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0;//Myszowate
    int cont=1;//flaga kontynuacji
    int std=0;

    mouse_activity(1);
    set_background(128);
    buffering_setup(1);/* Włączona animacja */
    shell_setup("SYMSHELL TEST",argc,argv);
    printf("COLORS= 256 q-quit s-switch stdout on/off\n"
           "setup options:\n"
           " -mapped -buffered -bestfont -traceevt\n"
           " + for enable\n - for disable\n ! - for check\n");

    /* RANDOMIZE();*/

    if(!init_plot(256,256,0,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    while(cont) //PĘTLA GŁÓWNA
    {
        char znak;

        znak=get_char();/* Tutaj czeka na zdarzenia, więc nie ma pracy w tle */

        switch(znak)
        {
        case '@':
        case '\r':replot();break;
        case '\b':get_mouse_event(&xpos,&ypos,&click);
            list[i%numi].p.x=xpos;list[i%numi].p.y=ypos;
            list[i%numi].color=i%256;
            i++;
            replot();
            break;
        case 's':std=!std;break;
        case 'q':
        case EOF:
            cont=0;
            break;
        }

        if(std)
        {
            printf("stdout<<%c\n",znak);
            fflush(stdout);
        }
        printc(screen_width()-char_width('X'),
               screen_height()-char_height('X'),
               34,255,"%c",znak);
        flush_plot();
    }

    close_plot();
    printf("Do widzenia!!!\n");
    return 0;
}

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
