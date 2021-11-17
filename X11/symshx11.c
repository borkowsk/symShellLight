/* SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++    */
/************************************************************************/
/* Najprostrzy interface wizualizacyjny zaimplementowany pod X-windows  */
/* za pomoca biblioteki X11                                             */
/*                                                                      */
/* designed by W.Borkowski from University of Warsaw                    */
/* https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI              */
/* https://github.com/borkowsk                                          */
/* File changed masively: 21.10.2020                                    */
/*                                                                      */
/* FILE: symshx11.c                                                     */
/* Wersja: 26.10.2021                                                   */
/*                                                                      */
/* UWAGA!                                                               */
/* WCIĄŻ Z BLEDEM NA EXPOSE TODO , choc raz go juz gdzies usunalem :-/  */
/*                                                                      */
/************************************************************************/
/*               SYMSHELLLIGHT  version 2021-11-10                      */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <assert.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include "../symshell.h"
#include "icon.h"

#define BITMAPDEPTH 1
#define TOO_SMALL 0
#define BIG_ENOUGH 1

#define FALSE 0
#define NODATA '\0'

#ifndef NULL
#define NULL __null
#endif

/* For close_plot() */
 extern int             WB_error_enter_before_clean;/* Czy zamykać okno od razu czy dawać "enter"? */
 int                    ssh_trace_level = 0;        /* Maska poziomów śledzenia 1-msgs 2-grafika 3-grafika detaliczna 4-alokacje/zwalnianie */
 static int             opened=0; /* Dla close plot. Zerowane tez gdy "broken-pipe" */

 /* progname is the string by which this program was invoked; this
  * is global because it is needed in most application functions */
 static char            progname[1024]="WB SYMULATION NAME NOT SET";
 static char            window_name[1024] = "WB-sym-shell";//"WB X-window symulation shell";
 static char            icon_name[1024] = "WB-sym-shell";

 static unsigned        largc=0;    /* Do zapamietania przekazanych przez funkcje setup*/
 static const char**    largv=NULL;

 /* Parametry z inicjalizacji modułu (shell setup) */
 static int             isbuffered=0;       /* Czy okno jest buforowane pixmapa */
 static int             animate=0;          /* Czy odswierznaie tylko gdy flush czy na biezaco */
 static int             ScreenClip=1;       /* Czy przycinać okno do wielkości ekranu? */
 static int             UseGrayScale=0;

 /* These are used as arguments to nearly every Xlib routine, so it
   * saves routine arguments to declare them global; if there were
   * additional source files, they would be declared extern there */
 static Display         *display=0; /* HANDLER TO Display */

 static int             screen_num;
 static char            *display_name = NULL;        /* Nazwa wyswietlacza - do odczytania */
 static unsigned int    display_width=0;   /* Will be filled during initialisation */
 static unsigned int    display_height=0;  /* Will be filled during initialisation */
 static XSizeHints      *size_hints;//To jest jeszcze gdzieś używane poza init_plot() Chyba do resize?

 static unsigned int    mulx=1,muly=1;      /* Multiplication of x & y */
 static unsigned int    org_width,org_height;  /* Starting Window size */
 static int             ini_a,ini_b,ini_ca,ini_cb;      /* Konieczne do dzialania
                                               screen_width() i screen_heiht() i zmiany rozmiaru*/

 static Window          win;               /* HANDLER TO MAIN WINDOW */
        XID             _ssh_window=0;     /* Handler for check and external use */

 static unsigned int    width,height;      /* Window size */
 static int             iniX, iniY;        /* Window position */

 //For transperencies to work use:
 //#define CREATE_FULL_WINDOW 32
 static unsigned int    default_depth = 24;/* CURENTLY 24 WORK. BUT 32 NOT */
 static unsigned int    border_width = 4;  /* Four pixels */
 static int             window_size = TOO_SMALL;     /* BIG_ENOUGH or TOO_SMALL to display contents */

 static unsigned int    icon_width, icon_height;/* Rozmiary ikony */
 static Pixmap          icon_pixmap;
 static unsigned short  alloc_cont=0;      /* Sygnalizuje ze pixmapa zaostala zaalokowana */

 static Pixmap          cont_pixmap=0;     /* Uchwyt do Pixmap'y zastępującej/mirrorującej okno */

 static XTextProperty    windowName;
 static XTextProperty    iconName;

 static GC               gc=NULL;          /* KONTEKST GRAFICZNY */

 /* FONT */
 static int              ResizeFont=0;     /* Czy probuje zmieniac rozmiar fontu */
 static XFontStruct     *font_info=NULL;   /* Aktualny font */
 static unsigned         ori_font_width = 8;
 static unsigned         ori_font_height = 16;
 static unsigned         font_width = 0;
 static unsigned         font_height = 0;

 /* REPAINT */
 static struct XRect{
     int x,y,width,height;
 } last_repaint_data;

 static int             repaint_flag=0;     /* Trzeba odrysować? */

 /* TŁO */
 static unsigned long   Black,White;
 static Colormap        colormap=0;
 static int             CurrForeground=-1; /* Index to last set color */
 static unsigned        bacground=0;       /* Index koloru tla */

 /* PALETA */
 static unsigned        NumberOfColors=512; /* */
 static unsigned long   Scale[514];
 static unsigned long   PenColor=-1;
 static unsigned long   BrushColor=-1;
 static XColor          ColorArray[512];
 static void SetScale(XColor RGBarray[512]);//Ustawianie kolorow

 /* Parametry rysowania */
 static unsigned        default_line_width=2;/* grubość lini */
 static int             transparent_print=0; /* czy napisy transparentne */
 static int             pieMode=-1;/* Tryb wypełniania fragmentu okręgu lib ellipsy */

 /* Wejście znakowe? */
 static KeySym           thekey;
 static int              buforek[2]; /* Bufor na znaki z klawiatury - tylko 0 jest przekazywany */
 //static char bfirst=0;  /* Zmienne na implementacje cykliczną bufora */
 //static char blast=0;   /* nie uzywane... */

 /* Wejście myszowe */
 static int              mouse=0;

 static
 struct { int  flags, x, y;
          unsigned buttons;}        LastMouse={0, 0, 0, 0};

 /* OBSŁUGA SYGNAŁÓW */
 static int             error_limit=3; /* Limit odeslanych bledow od x-serwera */
 static int             error_count=3; /*antylicznik bledow - gdy 0 - wyjscie */

 static int             DelayAction=0; /* Sterowanie zasypianiem jesli program czeka */
 static int             pipe_break=0;

 void SigPipe(int num)
 {
     pipe_break=1;
     opened=0;
     signal(num,SIG_IGN);
     fprintf(stderr,"\nX11-SYMSHELL received a SIGNALL #%d\n",num);
     exit(num);
 }

 int MyXIOHandler(Display* d)
 /*
    int (*XSetIOErrorHandler(handler))()
          int (*handler)(Display *);

    Arguments
    handler 	Specifies the program's supplied error handler.
    Description
    The XSetIOErrorHandler() sets the fatal I/O error handler. Xlib calls the program's supplied error handler if any sort of system call error occurs (for example, the connection to the server was lost). This is assumed to be a fatal condition, and the called routine should not return. If the I/O error handler does return, the client process exits.

    Note that the previous error handler is returned.
  */
 {
     pipe_break=1;
     fprintf(stderr,"\nX11-SYMSHELL GOT A IOError\n");
     XSetIOErrorHandler(NULL);/* Kasowanie dotychgczasowego handlera z wywołania XSetIOErrorHandler(MyXIOHandler); */
     exit(-11);
     return -11;
 }

 int MyErrorHandler(Display *xDisplay, XErrorEvent *event)
 {
     char buf[80];

     fprintf(stderr,"\nX11-SYMSHELL received an X error!\n");
     fprintf(stderr,"Ecode  : %d", event->error_code);
     fprintf(stderr,"\tRequest: %d", event->request_code);
     fprintf(stderr,"\tMinor: %d", event->minor_code);
     XGetErrorText(xDisplay, event->error_code, buf, 80);
     fprintf(stderr,"\tEtext : '%s'\n", buf);

     error_count--;
     if(!error_count)
     {
         pipe_break=1;
         exit(event->error_code);
     }

     return 0;
 }


 int	print_transparently(int yes)
 /* Włącza drukowanie tekstu bez zamazywania tla. Zwraca stan poprzedni */
 {
     int ret=transparent_print;
     transparent_print=yes;
     return ret;
 }

 void fix_size(int Yes)
 /* Czy symulowac niezmiennosc rozmiarow okna */
 {
     if(ssh_trace_level)
        fprintf(stderr,"fix_size() not implemented\n");
 }

 int mouse_activity(int yes)
 /* Ustala czy mysz ma byc obslugiwana.
    W X11 zawsze jest, ale można ją ignorować */
 {
     int pom=mouse;
     mouse=yes;
     return pom;
 }

 ssh_color background()
 /* Aktualny kolor tla - nowa wersja */
 {
     return bacground;
 }

 void set_background(ssh_color c)
 /* Ustala index koloru do czyszczenia itp */
 {
    if(c>=0 && c<=NumberOfColors)
       bacground=c;
    else
       bacground=0;
 }

 void buffering_setup(int _n)
 {
     if(_n)
         animate=1;
     else
         animate=0;
     if(animate)        /* Musi byc wlaczona bitmapa buforujaca */
         isbuffered=1;  /* zeby mozna bylo na nia pisac */
 }

 unsigned get_buffering()
 {
     return animate;
 }

 static void CloseAll()
 {
     if(display==0)
     {
         if(ssh_trace_level)
             fprintf(stderr,"X11: Trying to free resources of NULL display!\n");
         return;
     }

     if(font_info){
         XUnloadFont(display, font_info->fid);
         font_info=NULL; }

     if(gc!=0){
         XFreeGC(display, gc);
         gc=0; }

#ifndef __MSDOS__
     if(colormap!=0)
     {
         XUninstallColormap(display,colormap);
         XFreeColormap(display,colormap);
         colormap=0;
     }
#endif

     if(alloc_cont!=0)
     {
         XFreePixmap(display,cont_pixmap);
         if(ssh_trace_level)
             fprintf(stderr,"X11: %s %lx\n","FREE PIXMAP",cont_pixmap);//??? xl?
         cont_pixmap=0;
     }

     if(win!=0)
     {
         if(ssh_trace_level)
             fprintf(stderr, "X11: %s %lx ","DESTROY WINDOW",win);

         XDestroyWindow(display,win);
         if(ssh_trace_level)
             fprintf(stderr, "-OK %lx\n",win);
         win=0;
     }

     if(ssh_trace_level)
         fprintf(stderr,"X11: CLOSE DISPLAY");

     XCloseDisplay(display);

     if(ssh_trace_level)
         fprintf(stderr,"-OK\n");

     display=0;
 }

static int inside_close_plot=0;/* Zabezpiecza przed ukryta rekurencją w close_plot */

void close_plot()
/* --------//---------- zamkniecie grafiki/semigrafiki */
{
    if(inside_close_plot)
        return;

    if(opened)
    {
        inside_close_plot=1;
        XSync(display,1/*DISCARD EVENTS*/);
        _ssh_window=0; /* Do not use this window any more!!!*/

        if(WB_error_enter_before_clean)
        {
            fflush(stdout);
            if(!pipe_break)
            {
                char* kom="(Press ANY KEY to close graphix)";
                /* width,height of Window at this moment */
                printbw(screen_width()/2-(strlen(kom)*char_width('X'))/2,screen_height()/2,kom);
                flush_plot();
                XSync(display,1/*DISCARD EVENTS*/);
                fprintf(stderr,"(See at window \"%s\" )\n",window_name);
                fflush(stderr);
            }

            if(!pipe_break)
                get_char();//X window
            else
            {
                fprintf(stderr,"Window \"%s\" closed. PRESS ENTER\n",window_name);
                fflush(stderr);
                getchar();//Console
            }


            WB_error_enter_before_clean=0;
        }

        //CLOSE:
        CloseAll();
        fflush(stderr);
        fflush(stdout);
        opened=0;
        inside_close_plot=0;
    }
}

static void ResizeBuffer(unsigned int nwidth,unsigned int nheight)
/* Alokuje pixmape na zawartosc okna */
{
    if(alloc_cont && cont_pixmap!=0)
    {
        XFreePixmap(display,cont_pixmap);
        alloc_cont=cont_pixmap=0;
    }

    if(ssh_trace_level)
    {
        fprintf(stderr,"X11: %s %dx%d\n","ALLOC PIXMAP",nwidth,nheight);
        /*getchar();*/
    }

    cont_pixmap=XCreatePixmap(display,win,nwidth,nheight,default_depth);

    assert(cont_pixmap);
    alloc_cont=1;
    XSetForeground(display, gc, Black);
    CurrForeground=-1;
    XFillRectangle(display,cont_pixmap , gc, 0, 0, nwidth+1, nheight+1);
    if(ssh_trace_level)
    {
        XFlush(display);
        if(ssh_trace_level){
            fprintf(stderr,"X11: PIXMAP %lx DISPLAY %p WIN %lx %s\n",
                    cont_pixmap,
                    display,
                    win,
                    "OK?");
        /*getchar();*/
        }
    }
}

static void load_font(XFontStruct **font_info, GC *gc)
{
    char fontname[256];
    XFontStruct* l_font_info;

    sprintf(fontname,"%dx%d",ori_font_width,ori_font_height);

    if(!ResizeFont && (*font_info) != NULL ) return;

    /* Load font and get font information structure */
    if ((l_font_info = XLoadQueryFont(display,fontname)) == NULL)
    {
        fprintf( stderr, "%s: Cannot open %s font\n",
                        progname,fontname);
        if( (*font_info)== NULL )
            exit( -1 ); /* Nie ma zadnego fontu */
        else
            return ;    /* Pozostaje stary */
    }

    if( (*font_info) != NULL ) /* Usuwa stary font */
        XUnloadFont(display, (*font_info)->fid);
    (*font_info)=l_font_info;   /* zapamietuje nowy do uzycia */

    /* Specify font in gc */
    XSetFont(display, *gc, l_font_info->fid );

    /* Get string widths & hight */
    font_width = XTextWidth(*font_info, "X", 1);
    font_height = (*font_info)->ascent + (*font_info)->descent;

    if(ssh_trace_level)
        fprintf(stderr,"X11: %s:font %ux%u\n",icon_name,font_width,font_height);
}

static void TooSmall(Window win,GC gc,XFontStruct* font_info)
{
    char *string1 = "Too Small";
    int y_offset, x_offset;
    y_offset = font_info->ascent + 2;
    x_offset = 2;

    /* Output text, centered on each line */
    XDrawString(display, win, gc, x_offset, y_offset, string1,
                strlen(string1));
}

inline
static void place_graphics(Window win,GC gc,
                           int      area_x,int          area_y,
                           unsigned area_width,unsigned area_height)
{
    XCopyArea(display, cont_pixmap, win, gc,
              area_x/*src_x*/, area_y /*src_y*/,
              area_width, area_height,
              area_x/*dest_x*/, area_y/*dest_y*/);
}

static void Read_XInput()
{
    static int buffer_empty=1;

    XEvent report;  /* Miejsce na odczytywane zdazenia */

    if(pipe_break)	/* Musi zwrocic EOF */
    {
        *buforek=EOF;
        return ;
    }

    /* Get events, use first to display text and graphics */

    XNextEvent(display, &report);

    switch  (report.type) {

    case Expose:
        if(ssh_trace_level)
            fprintf(stderr,"X11: EXPOSE: %s #%d x=%d y=%d %dx%d\n",
                    icon_name,
                    report.xexpose.count,
                    report.xexpose.x,
                    report.xexpose.y,
                    report.xexpose.width,
                    report.xexpose.height);

        /* Unless this is the last contiguous expose,
           * don't draw the window */
        /* if (!isbuffered && report.xexpose.count != 0)
                         break;  */

        /* If window too small to use */
        if(window_size == TOO_SMALL)
            TooSmall(win, gc, font_info);
        else
        {
            XSetForeground(display, gc, Scale[bacground]);
            CurrForeground=-1;

            XFillRectangle(display,win , gc,
                           report.xexpose.x,
                           report.xexpose.y,
                           report.xexpose.width,
                           report.xexpose.height);

            if( repaint_flag==1 || (!isbuffered) || buffer_empty )
            {
                /* Sumuje z marginesem, takz zeby pokrywalo wszytkie expos'y */
                if(last_repaint_data.x>report.xexpose.x)
                    last_repaint_data.x=report.xexpose.x;

                if(last_repaint_data.y>report.xexpose.y)
                    last_repaint_data.y=report.xexpose.y;

                /* Tu jest chyba zle */
                if(last_repaint_data.width<report.xexpose.width)
                    last_repaint_data.width+=report.xexpose.width;

                if(last_repaint_data.height<report.xexpose.height)
                    last_repaint_data.height+=report.xexpose.height;

                repaint_flag=1;/* Sa juz dane dla repaint */

                /* Set information for main program about refresh screen */
                if(report.xexpose.count == 0 ||  buffer_empty )
                {
                    if(ssh_trace_level)
                        fprintf(stderr,"X11: EXPOSE force repaint\n");
                    buforek[0]='\r';
                    buffer_empty=0;

                }
            }
            else
            {
                /* Refresh from pixmap buffer */
                if(ssh_trace_level)
                    fprintf(stderr,"X11: EXPOSE DOING BITBLT\n");
                place_graphics(win, gc ,
                               report.xexpose.x,report.xexpose.y,
                               report.xexpose.width,report.xexpose.height
                               );
                DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
            }
        }
        break;

    case MappingNotify:
        XRefreshKeyboardMapping((XMappingEvent *)&report );
        DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
        break;

    case ConfigureNotify:
        DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
        if(ssh_trace_level)
            fprintf(stderr,"X11: CONFIGURE: %s=%dx%d scale: x=%d:1 y=%d:1 ",
                    icon_name,
                    width,height,mulx,muly);

        /* Window has been resized; change width
           * and height for next Expose */

        if( width== report.xconfigure.width &&
                height== report.xconfigure.height)
        {
            if(ssh_trace_level)
                fprintf(stderr,"The same.\n");
            break; /* Nic sie nie zmienilo */
        }

        width = report.xconfigure.width;
        height = report.xconfigure.height;

        if ((width < size_hints->min_width) ||
                (height < size_hints->min_height))
        {
            window_size = TOO_SMALL;
            if(ssh_trace_level)
                fprintf(stderr,"To small!\n");
        }
        else
        {
            window_size = BIG_ENOUGH;
            /*mulx=width/org_width; STARA WERSJA
              muly=height/org_height;*/

            mulx=(width-ini_ca*font_width)/ini_a;
            muly=(height-ini_cb*font_height)/ini_b;

            if(mulx<=0) mulx=1;//Gdy ekran przymusowo zmniejszony
            if(muly<=0) muly=1;

            load_font(&font_info,&gc); /* New font - size changed */

            if(isbuffered)
            {
                ResizeBuffer(width,height);
                buffer_empty=1;
            }

            if(ssh_trace_level)
                fprintf(stderr,"->%dx%d scale: x=%d:1 y=%d:1 \n",width,height,mulx,muly);
        }
        break;

    case ButtonPress:
        DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
        if(mouse)
        {
            buforek[0]='\b';
            LastMouse.flags=1;
            LastMouse.x=report.xbutton.x;
            LastMouse.y=report.xbutton.y;
            LastMouse.buttons=report.xbutton.button;
            if(ssh_trace_level)
                fprintf(stderr,"ButtonPress:x=%d y=%d b=X0%x\n",
                        LastMouse.x,LastMouse.y,LastMouse.buttons  );
        }
        break;

    case KeyPress:
    {
        char Bufor[8];
        unsigned KeyCount=XLookupString((XKeyEvent *)&report,Bufor,sizeof(buforek),&thekey,0);
        DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */

        *buforek=*Bufor;/* Na zewnatrz widziane w buforku */

        if(KeyCount!=1)
            *buforek=NODATA;

        if(ssh_trace_level)
            fprintf(stderr,"KeyPress:%c %x \n ",*Bufor,(int)(*Bufor));

        if(*Bufor==0x3 || *Bufor==0x4)/* User przerwal w oknie X11 */
            *buforek=EOF;
    } break;

    case ClientMessage: //https://tronche.com/gui/x/xlib/events/client-communication/client-message.html
    {
        if(ssh_trace_level)
        {
            fprintf(stderr," Client message arrived ");
            switch(report.xclient.format){
            case 8:fprintf(stderr,"->%s\n", report.xclient.data.b);
                break;
            case 16:fprintf(stderr,"->0x%x\n", report.xclient.data.s[0]);
                break;
            case 32:fprintf(stderr,"->0x%lu\n", report.xclient.data.l[0]);
                break;
            default:fprintf(stderr,"->Invalid format field %d\n",report.xclient.format );
                break;}
        }
        switch(report.xclient.format){
        case 8: break;
        case 16:if( report.xclient.data.s[0]==(short)0xffff)
            {pipe_break=1;*buforek=EOF;}
            break;
        case 32:*buforek=report.xclient.data.l[0];
            break;
        default: break;}
    } break;

    default:
#	ifdef XDEBUG
        if(ssh_trace_level)
            fprintf(stderr,"Message %d=\"%s\" arrived but ignored \n ",
                    report.type,event_name(report.type) );y2
#	endif
        break;
    } /* End switch */

}

static void getGC(Window win,GC* gc,XFontStruct* font_info)
{
    //XColor pom;//??? TODO
    unsigned long valuemask = 0,i; /* Ignore XGCvalues and
                          * use defaults */
    XGCValues values;
    unsigned int line_width = 6;
    int line_style = LineOnOffDash;
    int cap_style = CapRound;
    int join_style = JoinRound;
    //int dash_offset = 0;
    //static char dash_list[] = {12, 24};
    //int list_length = 2;

    /* Create default Graphics Context */
    *gc = XCreateGC(display, win, valuemask, &values);

    /* Specify black foreground since default window background
     * is white and default foreground is undefined */
    Black=BlackPixel(display,screen_num);
    White=WhitePixel(display,screen_num);
    XSetForeground(display, *gc, Black);

    /* Set line attributes */
    XSetLineAttributes(display, *gc, line_width, line_style,
                       cap_style, join_style);
    /* Set dashes */

    /*
    XSetDashes(display, *gc, dash_offset, dash_list, list_length);
     */

    /* Set Color scale */
    Scale[0]=BlackPixel(display,screen_num);
    Scale[255]=WhitePixel(display,screen_num);
    PenColor=-1;
    BrushColor=-1;

    // TODO (borkowsk#1#): W tej chwili działa tylko na "wyświetlaczach" TRUE COLOR.
    //Cała część na wyświetlacze o mniejszej głębi wymaga poprawienia i przetestowania,
    //choć kiedyś działała poprawnie na 256 kolorach - ale to było prze 2000 rokiem!
    /*
#ifdef __MSDOS__
    colormap=XDefaultColormap(display,screen_num); //* W tej emulacji nie ma map
#else
    colormap=XCreateColormap(display,win,DefaultVisual(display,screen_num),AllocNone);
#endif

    XSetWindowColormap(display,win,colormap);

    pom.red=pom.green=pom.blue=0;
    XAllocColor(display,colormap,&pom);
    Black=pom.pixel;
    Scale[0]=pom.pixel;
    XSetForeground(display, *gc, Black);

    pom.red=pom.green=pom.blue=0xffff;
    XAllocColor(display,colormap,&pom);
    White=pom.pixel;
    Scale[255]=pom.pixel;
    XSetBackground(display, *gc, White);
*/

    SetScale(ColorArray);
    /*
    for(i=1;i<NumberOfColors;i++)
    {
     pom=ColorArray[i];

     pom.red*=256;
     pom.blue*=256;
     pom.green*=256;

     XAllocColor(display,colormap,&pom);

     Scale[i]=pom.pixel;
    }

    XInstallColormap(display,colormap);
*/
}

void set_title(const char* window_name)
{
    /* Change window title bar */
    /* https://tronche.com/gui/x/xlib/ICC/client-to-window-manager/XStoreName.html */
    XStoreName(display,win, window_name);
}

void shell_setup(const char* title,int iargc,const char* iargv[])
{
    /* Przekazanie parametrow wywolania fo konfiguracji symmshella */
    int i;
    largc=iargc;
    largv=iargv;

    strncpy(progname,largv[0],1024);//TODO TEST
    strncpy(window_name, title, 1024);
    strncpy(icon_name, title,1024);

    for(i=1;i<largc;i++)
    {
        if(strncmp(largv[i],"-h",2)==0)
        {
            fprintf(stderr,"SYMSHELL for X11 enviroment. Compiled %s\n",__DATE__);
            fprintf(stderr,"Supported swithes:"
                           "\n\t -mapped+/-] "
                           "\n\t -buffered[+/-]"
                           "\n\t -sclip[+/-]"
                           "\n\t -bestfont[+/-] "
                           "\n\t -traceevt[+/-] "
                           "\n\t -gray[+/-] "
                           "\n");
            fprintf(stderr,"You can always gracefully break the program sending\n"
                           " the  ^C  or  ^D  key	 to  g_r_a_p_h_i_c_s  w_i_n_d_o_w !\n"
                           "Other methods break the program immediatelly by exit call!\n");
            exit(-1);
        }
        else
            if(strncmp(largv[i],"-gray",5)==0)
            {
                UseGrayScale=(largv[i][5]=='+')?1:0;
                fprintf(stderr,"Gray scale is %s\n",(UseGrayScale?"ON":"OFF"));
            }
            else
                if(strncmp(largv[i],"-sclip",6)==0)
                {
                    ScreenClip=(largv[i][6]=='+')?1:0;
                    fprintf(stderr,"Clipping to display size is %s\n",(ScreenClip?"ON":"OFF"));
                }
                else
                    if(strncmp(largv[i],"-mapped",7)==0)
                    {
                        isbuffered=(largv[i][7]=='+')?1:0;
                        fprintf(stderr,"Double mapping is %s\n",(isbuffered?"ON":"OFF"));
                        /* Jesli user chec tryb mapowania to nie bedziemy */
                        animate=0; /* animowac */
                    }
                    else
                        if(strncmp(largv[i],"-buffered",9)==0)
                        {
                            animate=(largv[i][9]=='+')?1:0;
                            fprintf(stderr,"Buffered is %s\n",(animate?"ON":"OFF"));
                            /* Musi byc wlaczona bitmapa buforujaca */
                            isbuffered=animate;/* zeby mozna bylo na nia pisac */
                        }
                        else
                            if(strncmp(largv[i],"-bestfont",9)==0)
                            {
                                ResizeFont=(largv[i][9]=='+')?1:0;
                                fprintf(stderr,"Search best font is %s\n",(ResizeFont?"ON":"OFF"));
                            }
                            else
                                if(strncmp(largv[i],"-traceevt",9)==0)
                                {
                                    ssh_trace_level=(largv[i][9]=='+')?1:0;
                                    fprintf(stderr,"Trace events is %s\n",(ssh_trace_level?"ON":"OFF"));
                                }
    }
}

ssh_stat init_plot(ssh_natural a,ssh_natural b,ssh_natural ca,ssh_natural cb)
/* typowa dla platformy inicjacja grafiki/semigrafik
 a,b to wymagane rozmiary ekranu */
{
    int* disp_depht;
    int  disp_depht_num=0,i;
    int real_font_width=0;
    XSetWindowAttributes attrs;
    ini_a=a;
    ini_b=b;
    ini_ca=ca;
    ini_cb=cb;

    XWMHints        *wm_hints;
    XClassHint      *class_hints;

    if (!(size_hints = XAllocSizeHints())) {
       fprintf(stderr, "X11: %s: failure allocating memory", progname);
         exit(-2);  return 0;
    }
    if (!(wm_hints = XAllocWMHints())) {
       fprintf(stderr, "X11: %s: failure allocating memory", progname);
         exit(-2);  return 0;
    }
    if (!(class_hints = XAllocClassHint())) {
       fprintf(stderr, "X11: %s: failure allocating memory", progname);
         exit(-2); return 0;
    }

    /* Connect to X server */
    if ( (display=XOpenDisplay(display_name)) == NULL )
    {
       fprintf( stderr, "X11: '%s': cannot connect to X server '%s'\n",
             progname, XDisplayName(display_name));
       exit( -1 );  return 0;
    }

    /* Get screen size from display structure macro */
    screen_num = DefaultScreen(display);
    display_width = DisplayWidth(display, screen_num);
    display_height = DisplayHeight(display, screen_num);

    if(ssh_trace_level)
    {
        fprintf(stderr,"X11: Screen: %u x %u ; %u\n",display_width,display_height,screen_num);
    }

    disp_depht = XListDepths(display, screen_num, &disp_depht_num);

    if(ssh_trace_level)
    {
        fprintf(stderr,"X11: Available display depths:");
    	for(i=0;i<disp_depht_num;i++)
    		fprintf(stderr,"%d ",disp_depht[i]);
        fprintf(stderr," Prefered depth:%d\n",default_depth);
    }

    /* Search for depht */
    for(i=0;i<disp_depht_num;i++)
        if(disp_depht[i]>=default_depth)
        {
            default_depth=disp_depht[i];/*Pierwszy >= wymaganemu */
            break;
        }

    //default_deph=32;//??? NOT WORK! WHY???

   if(ssh_trace_level)
        fprintf(stderr,"X11: Select depth %d\n",default_depth);

    /* Note that in a real application, x and y would default
     * to 0 but would be settable from the command line or
     * resource database */
   org_width = width = a+ca*ori_font_width;
   org_height = height = b+cb*ori_font_height;
   if(ScreenClip)
   {
       if(width>display_width)
            org_width = width = display_width;
       if(height>display_height)
            org_height = height = display_height;
   }

   iniX=iniY=0;

   if(ssh_trace_level)
        fprintf(stderr,"X11: %s=%dx%d\n",icon_name,width,height);
   fflush(stderr);

   XSetErrorHandler(MyErrorHandler);

 /* Create opaque window */
 #ifdef CREATE_FULL_WINDOW
 {
   //It work strange - backgrpound of window is transparent!
   //https://stackoverflow.com/questions/3645632/how-to-create-a-window-with-a-bit-depth-of-32

   int i,nxvisuals = 0;
   XVisualInfo  vinfo;
   XVisualInfo *visual_list;
   XVisualInfo  visual_template;
   Visual      *visual;
   Window       parent;
   visual_template.screen = DefaultScreen(display);
   visual_list = XGetVisualInfo (display, VisualScreenMask, &visual_template, &nxvisuals);
   //for (i = 0; i < nxvisuals; ++i)
   //    fprintf(stderr,"X11: %3d: visual 0x%lx class %d (%s) depth %d\n",
   //           i,visual_list[i].visualid,visual_list[i].class,
   //           visual_list[i].class == TrueColor ? "TrueColor" : "unknown",visual_list[i].depth);

   if (!XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &vinfo))
     {
       fprintf(stderr, "X11: no such visual\n");
       return 0;
     }
     else
     fprintf(stderr,"X11:Matched visual 0x%lx class %d (%s) depth %d\n",
              vinfo.visualid,vinfo.class,
              vinfo.class == TrueColor ? "TrueColor" : "unknown",vinfo.depth);

   parent = XDefaultRootWindow(display);
   XSync(display, True);

   visual = vinfo.visual;
   default_depth = vinfo.depth;
   attrs.background_pixel = 0xff000000;//0;//BlackPixel(display, screen_num);???
   attrs.border_pixel = 0;
   attrs.colormap = XCreateColormap(display, XDefaultRootWindow(display), visual, AllocNone);

   win = XCreateWindow(display,
            parent,
            iniX, iniY, width, height,
            border_width,
            default_depth        /* Window deph */,
            InputOutput          /* WindowClass*/,
            visual               /* Visual */,
            CWBackPixel | CWBorderPixel | CWColormap  /* Valuemask */,
            &attrs                   /* Atributes */
	 	);
   XSync(display, True);

   }
 #else
    win = XCreateSimpleWindow(display,
                            RootWindow(display,screen_num),
                            iniX, iniY, width, height, border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num)
                );
    XSync(display, True);
 #endif

    if(win!=0)
        window_size = BIG_ENOUGH ;
    else //Czy to się w ogóle ma szansę zdarzyć? Raczej wyleci wczesniej na błędzie?!
        fprintf( stderr, "X11: %s: Window not created! ", progname);

    /* Get available icon sizes from window manager and create apriopriate bitmap */
    {   int              count;
        XIconSize       *size_list=NULL;
        if((XGetIconSizes(display, RootWindow(display,screen_num),&size_list, &count) == 0)  && ssh_trace_level )
        {
            fprintf( stderr, "X11: %s: Window manager didn't set "
                             "icon sizes - using default.\n", progname);
         }
        else {
           ;
           /* A real application would search through size_list
            * here to find an acceptable icon size and then
            * create a pixmap of that size; this requires that
            * the application have data for several sizes of icons */
            //...
            // WB_icon_bitmap_bits=;WB_icon_bitmap_width=;WB_icon_bitmap_height=;
            /* Create pixmap of depth 1 (bitmap) for icon */
            if(size_list!=NULL) XFree(size_list);
        }

        //May be not supported in moder window menagers?
        //See-> https://stackoverflow.com/questions/10979412/how-to-add-an-icon-to-an-ubuntu-app
        icon_pixmap = XCreateBitmapFromData(display, win,
                WB_icon_bitmap_bits,WB_icon_bitmap_width,WB_icon_bitmap_height);
        if( ssh_trace_level > 1 )
        fprintf( stderr, "X11: %s: creation of "
                         "icon_pixmap return %lu.\n", progname,icon_pixmap);
        if( icon_pixmap==0 )
            fprintf( stderr, "X11: %s: creation of "
                             "icon_pixmap failed.\n", progname);
    }

    /* These calls store window_name and icon_name into
     * XTextProperty structures and set their other fields
     * properly */
    char* ptrName=window_name;// TODO CHECK!>!>!>
    if ((XStringListToTextProperty(&ptrName, 1, &windowName) == 0) )// && ssh_trace_level )
    {
        fprintf( stderr, "X11: %s: structure allocation for "
                               "windowName failed.\n", progname);
       exit(-1);return 0;
    }

    ptrName=icon_name;// TODO CHECK - icon is epty now!!!
    if ((XStringListToTextProperty(&ptrName, 1, &iconName) == 0) )// && ssh_trace_level )
    {
        fprintf( stderr, "X11: %s: structure allocation for "
                         "iconName failed.\n", progname);
       exit(-1);return 0;
    }

    /* Set size hints for window manager:
     * **********************************
     * the window manager
     * may override these settings
     * Note that in a real application, if size or position
     * were set by the user, the flags would be USPosition
     * and USSize and these would override the window manager's
     * preferences for this window
     * x, y, width, and height hints are now taken from
     * the actual settings of the window when mapped; note
     * that PPosition and PSize must be specified anyway
    */
    size_hints->flags =  PSize | PPosition | PMinSize;
    size_hints->min_width =  width;   /* Startup width/haight */
    size_hints->min_height = height; /* are minimal */

    /* Set window menager hints:
     * *************************
     */
    wm_hints->initial_state = NormalState;
    wm_hints->icon_pixmap = icon_pixmap;
    wm_hints->input = True;
    wm_hints->flags = StateHint  | InputHint | IconPixmapHint;

    /* Set window class hints:
     * ***********************
     */
    class_hints->res_name = progname;
    class_hints->res_class = "ssh_win";//"Basicwin";

//void XSetWMProperties(display, w, window_name, icon_name, argv, argc, normal_hints, wm_hints, class_hints)
    XSetWMProperties( display, win,
                      &windowName,
                      &iconName,
                      (char**)largv,largc,//Without cast is "passing argument 5 from incompatible pointer type" expected non const!!!
                      size_hints,
                      wm_hints,
                      class_hints);    /* ERRORS MAY APPEAR LATER! */

    /* Select event types wanted */
    XSelectInput(display, win,
	  ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask );

    /* ...GC for text and drawing */
    getGC(win, &gc, font_info);
    load_font(&font_info,&gc);
    real_font_width=char_width('W');

    /* Display window */
    buforek[0]=NODATA;

    XMapWindow(display, win);

    opened=1;
    if( (atexit(close_plot)==0) && ssh_trace_level)
        fprintf(stderr,"X11: atexit(close_plot) installed\n");

    if( (signal(SIGPIPE,SigPipe)!=SIG_ERR) && ssh_trace_level)
        fprintf(stderr,"X11: SIGPIPE handler installed\n");

    XIOErrorHandler ret;
    if( (ret=XSetIOErrorHandler(MyXIOHandler)) && ssh_trace_level )
        fprintf(stderr,"X11: IOErrorHandler installed. Ret=%p\n",ret);

    /* Alloc pixmap for contens buffering */
    if(isbuffered)
        ResizeBuffer(width,height);

    while(!input_ready()); /* Wait for expose */

    /* Czysci zeby wprowadzic ustalone tlo */
    if(ssh_trace_level>1)
        fprintf(stderr,"X11: Background is %d\n",(int)bacground);

    clear_screen();

    _ssh_window=win;//READY TO USE!
    return 1;
}

ssh_natural screen_height()
{                                                                   assert(muly>0);
   //return ini_b+(ini_cb*font_height)/muly;/* Window size */
   return height/muly;
}

ssh_natural screen_width()
{                                                                   assert(mulx>0);
    //return ini_a+(ini_ca*font_width)/mulx;/* Window size */
    return width/mulx;
}

ssh_natural char_height(char znak)
{                                                                   assert(muly>0);
    int pom=(font_height+muly)/muly;
    /*if(pom<1)pom=1;*/
    return pom;
}

ssh_natural  char_width(char znak)
{
    int width;
    char pom[2];
    pom[0]=znak; pom[1]='\0';                                       assert(mulx>0);
    width=XTextWidth(font_info,pom, 1)/mulx;

    if(width<1) width=1;

    return width;
}

ssh_natural  string_height(const char* str)
/* BARDZO PRYMITYWNIE! */
/* Aktualne rozmiary lancucha */
{
    return char_height(*str); /* Pierwszy znak! */
}

ssh_natural  string_width(const char* str)
/* ...potrzebne do jego pozycjonowania */
{
    int pom=XTextWidth(font_info,str,strlen(str))/mulx;
    if(pom<1)pom=1;
    return pom;
}

void flush_plot()
/* --------//---------- uzgodnienie zawartosci ekranu */
{
    if(ssh_trace_level>1)
        fprintf(stderr,"X11: FLUSH %s",icon_name);

    if(!opened)
    {
        fprintf(stderr,"X11: %s","SYMSHELL graphix not initialised or pipe broken");
        return;
    }

    if(animate)
        {/* Wyswietlenie bitmapy */
        XCopyArea(display, cont_pixmap, win, gc,
               0/*src_x*/, 0/*src_y*/,
               width, height,
               0/*dest_x*/, 0/*dest_y*/);
            if(ssh_trace_level>1)
                fprintf(stderr,"X11: DOING BITBLT");
        }

    XFlush(display);
    if(ssh_trace_level>1)
        fprintf(stderr,"X11: XFLUSH\n");
    error_count=error_limit;
}

/* GETTING INPUT */

/* Za pierwszym razem zwraca '\r'
zeby zasygnalizowac ze trzeba wyrysowac ekran */
static int first_to_read=0;

int  input_ready()
/* bardzo zalezna od platformy funkcja sprawdzajaca czy jest wejscie */
{
    if(first_to_read)
        return 1;

    if(pipe_break==1)
    {
        if(ssh_trace_level)
            fprintf(stderr,"'pipe_break' flag detected in input_ready() in '%s'.\n",icon_name);
        return EOF;
    }

    if(XPending(display)!=0) 	/* Sprawdzenie czy nie ma zdarzen */
    {			/*SA JAKIES!*/
        buforek[0]=NODATA; 	/*Asekuranctwo */
        Read_XInput(); 		/* Przetwarzanie zdarzen */
        if(buforek[0]!=NODATA)	/* Jest cos do zwrocenia jako znak */
        {
            first_to_read=buforek[0]; /*Zostanie przeczytane przez get_char() */
            buforek[0]=NODATA;
            return 1;		/* Wiec mozna wywolac get_char() */
        }
    }

    return 0;
}

ssh_stat  set_char(int c)
/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
{
    if(first_to_read!=0)/* Nieodebrano */
            return 0;

    first_to_read=c;
    return 1;
}

int  get_char()
/* odczytywanie znakow sterowania */
{
    int pom;

    if(first_to_read!=0)
    {
        int pom=first_to_read ;
        first_to_read=0;
        return pom;
    }

    DelayAction=0;

    if(pipe_break==1)
    {
        if(ssh_trace_level)
            fprintf(stderr,"'pipe_break' flag detected in get_char() in '%s'.\n",icon_name);
        return EOF;
    }

    Read_XInput();
    pom=buforek[0];
    buforek[0]=NODATA;
    return pom;
}


/* Used for redraw & in print */

static char straznik1=0x77;
static char bufor[1024];
static char straznik2=0x77;
static int ox,oy;

void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...)
/* ---//--- wyprowadzenie tekstu na ekran */
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exced 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

    ox=x;oy=y;
    CurrForeground=-1;
    XSetForeground(display,gc,Scale[0]);

    if(transparent_print)
    {
        if(!animate)
            XDrawString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
    else
    {
        XSetBackground(display,gc,Scale[255]);
        if(!animate)
            XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
}

void printc(ssh_coordinate x,ssh_coordinate y,
            ssh_color fore,ssh_color back,
            const char* format,...)
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exced 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

    ox=x;oy=y;
    CurrForeground=-1;
    XSetForeground(display,gc,Scale[fore]);

    if(transparent_print)
    {
        if(!animate)
            XDrawString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
    else
    {
        XSetBackground(display,gc,Scale[back]);
        if(!animate)
            XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
}

void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...)
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exced 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

    ox=x;oy=y;

    CurrForeground=-1;
    XSetForeground(display,gc,BrushColor);

    if(transparent_print)
    {
        if(!animate)
            XDrawString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
    else
    {
        XSetForeground(display, gc, Scale[bacground] );
        if(!animate)
            XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
}

unsigned long buildColor(unsigned char red, unsigned char green, unsigned char blue)
/* BUDOWANIE WEWNĘTRZNYCH KOLORÓW W X11 - TODO CHECK*/
{
    return ( (unsigned long)(red) << 16) +
           ( (unsigned long)(green)<< 8) +
           ( (unsigned long)(blue) ) ;
}

unsigned long buildTransparentColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char transparency)
/* BUDOWANIE WEWNĘTRZNYCH KOLORÓW W X11 - TODO CHECK*/
{
    return ( (unsigned long)(transparency) << 24 ) +
           ( (unsigned long)(red) << 16) +
           ( (unsigned long)(green)<< 8) +
           ( (unsigned long)(blue) ) ;
}

void print_rgb(ssh_coordinate x,ssh_coordinate y,
               ssh_intensity r,ssh_intensity g,ssh_intensity b,            /*- składowe koloru tekstu */
               ssh_color back,const char* format,...)
/* Drukuje z mozliwoscia ustawienia tuszu poprzez RGB */
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exced 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

    ox=x;oy=y;
    CurrForeground=-1;
    XSetForeground(display,gc,buildColor(r,g,b) );

    if(transparent_print)
    {
        if(!animate)
            XDrawString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
    else
    {
        XSetBackground(display,gc,Scale[back]);
        if(!animate)
            XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len);
        if(isbuffered)
            XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
    }
}

void plot_rgb(ssh_coordinate x,ssh_coordinate y,
              ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if window is bigger */

    CurrForeground=-1;
    XSetForeground(display,gc,buildColor(r,g,b) );

    if(mulx>1 || muly>1)
    {
        if(!animate)
            XFillRectangle(display, win, gc, x, y, mulx, muly);
        if(isbuffered)
            XFillRectangle(display, cont_pixmap, gc, x, y, mulx, muly);
    }
    else
    {
        if(!animate)
            XDrawPoint(display,win,gc,x,y);
        if(isbuffered)
            XDrawPoint(display,cont_pixmap,gc,x,y);
    }
}

void plot_d(ssh_coordinate x,ssh_coordinate y)
/* ----//--- wyswietlenie punktu na ekranie w kolorze pen'a */
{
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(mulx>1 || muly>1)
    {
        if(!animate)
            XFillRectangle(display, win, gc, x, y, mulx, muly);
        if(isbuffered)
            XFillRectangle(display, cont_pixmap, gc, x, y, mulx, muly);
    }
    else
    {
        if(!animate)
            XDrawPoint(display,win,gc,x,y);
        if(isbuffered)
            XDrawPoint(display,cont_pixmap,gc,x,y);
    }
}

void plot(ssh_coordinate x,ssh_coordinate y,ssh_color c)
/* ----//--- wyswietlenie punktu na ekranie */
{
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if window is bigger */

    if(c!=CurrForeground)
    {
        CurrForeground=c;
        XSetForeground(display,gc,Scale[c]);
    }

    if(mulx>1 || muly>1)
    {
        if(!animate)
            XFillRectangle(display, win, gc, x, y, mulx, muly);
        if(isbuffered)
            XFillRectangle(display, cont_pixmap, gc, x, y, mulx, muly);
    }
    else
    {
        if(!animate)
            XDrawPoint(display,win,gc,x,y);
        if(isbuffered)
            XDrawPoint(display,cont_pixmap,gc,x,y);
    }
}


int line_style(int Style)
/* Ustala styl rysowania lini: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
{
    if(ssh_trace_level>0)
        fprintf(stderr,"%s %s %c", __FUNCTION__ ,"not implemented ",'\t');
    if(ssh_trace_level>0)
        fprintf(stderr,"%u\n", Style);
    /*int old = GrLineStyle;
    GrLineStyle = Style;
    return  GrLineStyle;    //Zwraca poprzedni stan
    SEE: LineSolid in X.h - TODO IMPLEMENT IT!
        XSetLineAttributes(display, gc,default_line_width,
                           LineSolid,  CapRound, JoinRound);
    */
    return SSH_LINE_SOLID; //Nie jest zaimplementowane
}

void set_pen(ssh_color c,ssh_natural size,ssh_mode style)
/* Ustala aktualny kolor linii za pomoca typu ssh_color */
{
    if(PenColor!=Scale[c])
    {
        PenColor=Scale[c];
        default_line_width=size;
    }
}

void set_pen_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_natural size,ssh_mode style)
/* Ustala aktualny kolor linii za pomoca skladowych RGB */
{
    PenColor=buildColor(r,g,b);
    default_line_width=size;
}


void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a,ssh_natural size,ssh_mode style)
/* Ustala aktualny kolor linii za pomoca skladowych RGBA */
{
    PenColor=buildTransparentColor(r,g,b,a);
    default_line_width=size;
}


ssh_natural get_line_width()
{
    return default_line_width;
}

ssh_natural line_width(ssh_natural width)
/* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni */
{
    int old=default_line_width;

    if(old!=width)
    {
        default_line_width=width;
        XSetLineAttributes(display, gc,default_line_width,
                           LineSolid,  CapRound, JoinRound);
        if(ssh_trace_level>2)
           fprintf(stderr,"Set default line width to %d\n",default_line_width);
    }

    return old;
}

void set_brush(ssh_color c)
/* Ustala aktualny kolor wypelnien za pomoca typu ssh_color */
{
    if(BrushColor!=Scale[c])
    {
        BrushColor=Scale[c];
    }
}

void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b)
/* Ustala aktualny kolor wypelnien za pomoca skladowych RGB */
{
    BrushColor=buildColor(r,g,b);
}

void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,
                   ssh_intensity a)
{
    BrushColor=buildTransparentColor(r,g,b,a);
}

void line_d(int x1,int y1,int x2,int y2)
/* wyrysowanie lini */
{
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
    y1*=muly;y2*=muly;  /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(!animate)
        XDrawLine(display,win, gc, x1, y1, x2, y2);

    if(isbuffered)
        XDrawLine(display,cont_pixmap, gc, x1, y1, x2, y2);
}

void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
/* wyrysowanie okregu w kolorze c */
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}

void ellipse_d(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b)
/* wyrysowanie elipsy w kolorze c */
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}

void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
/* wyrysowanie kola w kolorach domyslnych*/
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
                           LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;      /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(BrushColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,BrushColor);
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
    }
}

void fill_ellipse_d(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b)
/* Wypełnienie elipsy w kolorach domyslnych*/
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(BrushColor!=-1)
    {
       CurrForeground=-1;
       XSetForeground(display,gc,BrushColor);
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
    }
}

const double deg_mult = 180/M_PI;

inline
static float degrees(float radians)
{
    return radians * deg_mult;
    //return ( radians * 180 ) / M_PI ;
}

void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,            /*rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop)
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
}

void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
           ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
}

void earc_d(ssh_coordinate x,ssh_coordinate y,                         /*rysuje łuk eliptyczny */
            ssh_natural a,ssh_natural b,                               /* o półosiach a i b */
            ssh_radian start,ssh_radian stop)
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(PenColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,PenColor);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
}

void earc(ssh_coordinate x,ssh_coordinate y,
          ssh_natural a,ssh_natural b,
          ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
    }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
}

void fill_arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,       /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start,ssh_radian stop,                        /* początku i końcu */
                ssh_bool pie)                      /* początek i koniec łuku */
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(BrushColor!=-1)
    {
       CurrForeground=-1;
       XSetForeground(display,gc,BrushColor);
    }

    if(pieMode!=pie)
    {
        pieMode=pie;
        XSetArcMode(display,gc, pie?ArcPieSlice:ArcChord );//Specifies the arc mode. You can pass ArcChord or ArcPieSlice.
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2,r2*2,angle1,angle2);
    }
}

void fill_arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,         /* wirtualny środek i promień łuku */
              ssh_radian start,ssh_radian stop,                        /* początku i końcu */
              ssh_bool pie,ssh_color c)                                /* w kolorze c */
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
    }

    if(pieMode!=pie)
    {
        pieMode=pie;
        XSetArcMode(display,gc, pie?ArcPieSlice:ArcChord );//Specifies the arc mode. You can pass ArcChord or ArcPieSlice.
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2,r2*2,angle1,angle2);
    }
}

void fill_earc_d(ssh_coordinate x,ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a,ssh_natural b,                          /* o półosiach a i b */
                 ssh_radian start,ssh_radian stop,                     /* początku i końcu */
                 ssh_bool pie)
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(BrushColor!=-1)
    {
       CurrForeground=-1;
       XSetForeground(display,gc,BrushColor);
    }

    if(pieMode!=pie)
    {
        pieMode=pie;
        XSetArcMode(display,gc, pie?ArcPieSlice:ArcChord );//Specifies the arc mode. You can pass ArcChord or ArcPieSlice.
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
    }
}

void fill_earc(ssh_coordinate x,ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a,ssh_natural b,                            /* o półosiach a i b */
               ssh_radian start,ssh_radian stop,                       /* początku i końcu */
               ssh_bool pie,                                           /* w kształcie */
               ssh_color c)                                            /* w kolorze c */
{
    start=start;
    stop=stop-start;
    start=2*M_PI-start;
    stop=-stop;

    int angle1=degrees(start)*64;
    int angle2=degrees(stop)*64;
    int r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=a*mulx;r2=b*muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys - nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
    }
}

void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,                /* Wypełnienie prostokata */
                   ssh_coordinate x2,ssh_coordinate y2,                /* rozciągniętego między rogami x1y1 a x2y2 */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b)    /* w kolorze rbg okreslonym składowymi koloru */
{
    x1*=mulx; /* Multiplication of coordinates */
    y1*=muly; /* if window is bigger */
    x2*=mulx; /* Multiplication of 2' coordinates */
    y2*=muly; /* if window is bigger */


    CurrForeground=-1;
    BrushColor=-1;
    XSetForeground(display,gc,buildColor(r,g,b));

    if(!animate)
        XFillRectangle(display, win, gc, x1, y1, x2-x1, y2-y1 );
    if(isbuffered)
        XFillRectangle(display, cont_pixmap, gc, x1, y1, x2-x1, y2-y1 );

}

void fill_rect_d(int x1,int y1,int x2,int y2)
{
    x1*=mulx; /* Multiplication of coordinates */
    y1*=muly; /* if window is bigger */
    x2*=mulx; /* Multiplication of 2' coordinates */
    y2*=muly; /* if window is bigger */

    if(BrushColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,BrushColor);
    }

    if(!animate)
        XFillRectangle(display, win, gc, x1, y1, x2-x1, y2-y1 );
    if(isbuffered)
        XFillRectangle(display, cont_pixmap, gc, x1, y1, x2-x1, y2-y1 );

}

void fill_rect(int x1,int y1,int x2,int y2,ssh_color c)
{
    x1*=mulx; /* Multiplication of coordinates */
    y1*=muly; /* if window is bigger */
    x2*=mulx; /* Multiplication of 2' coordinates */
    y2*=muly; /* if window is bigger */

    if(c!=CurrForeground)
    {
        CurrForeground=c;
        XSetForeground(display,gc,Scale[c]);
    }

    if(!animate)
        XFillRectangle(display, win, gc, x1, y1, x2-x1, y2-y1 );
    if(isbuffered)
        XFillRectangle(display, cont_pixmap, gc, x1, y1, x2-x1, y2-y1 );

}

void fill_poly_d(ssh_coordinate vx,ssh_coordinate vy,
                 const ssh_point points[],int number)
/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
{
    static XPoint   _LocalTable[10];
    XPoint*          LocalPoints=_LocalTable;
    int i;

    if(number<=2)
        return; /*Nie da sie rysowac
        wielokata o dwu punktach lub
            mniej*/

    if(number>10) /*Jest za duzy. Alokacja*/
        LocalPoints=calloc(number,sizeof(XPoint));

    if(LocalPoints==NULL)
    {
        if(ssh_trace_level)
            fprintf(stderr,"No memory for polygon");
        return;
    }

    if(BrushColor!=-1)
    {
        CurrForeground=-1;
        XSetForeground(display,gc,BrushColor);
    }

    vx*=mulx;
    vy*=muly;

    for(i=0;i<number;i++)
    {
        LocalPoints[i].x=points[i].x*mulx+vx;
        LocalPoints[i].y=points[i].y*muly+vy;
    }

    if(!animate)
        XFillPolygon(display, win, gc,
                     LocalPoints,number,Complex,CoordModeOrigin);

    if(isbuffered)
        XFillPolygon(display, cont_pixmap, gc,
                     LocalPoints,number,Complex,CoordModeOrigin);

    if(number>10) /*Byl duzy*/
        free(LocalPoints);
}

void fill_circle(ssh_coordinate x, ssh_coordinate y, ssh_natural r, ssh_color c)
/* Kolo w kolorze c */
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
    }

    if(!animate)
        XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(isbuffered)
        XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}

void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c)
/* Wyswietlenie okregu w kolorze c */
{
    int angle2=360*64,r1,r2;
    unsigned line_width=default_line_width;

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
             LineSolid,  CapRound, JoinRound);
    }

    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */

    if(c!=CurrForeground)
       {
       CurrForeground=c;
       XSetForeground(display,gc,Scale[c]);
       }

    if(!animate)
        XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);

    if(isbuffered)
        XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}


void fill_poly(ssh_coordinate vx,ssh_coordinate vy,
               const ssh_point points[],int number,  /* - tablica wierzchołków wielokąta i jej długość */
               ssh_color c)
/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
{
    static XPoint _LocalTable[10];
    XPoint* 	LocalPoints=_LocalTable;
    int i;

    if(number<=2)
        return; /*Nie da sie rysowac
        wielokata o dwu punktach lub
            mniej*/

    if(number>10) /*Jest za duzy. Alokacja*/
        LocalPoints=calloc(number,sizeof(XPoint));

    if(LocalPoints==NULL)
    {
        if(ssh_trace_level)
            fprintf(stderr,"No memory for polygon");
        return;
    }

    if(c!=CurrForeground)
    {
        CurrForeground=c;
        XSetForeground(display,gc,Scale[c]);
    }

    vx*=mulx;
    vy*=muly;

    for(i=0;i<number;i++)
    {
        LocalPoints[i].x=points[i].x*mulx+vx;
        LocalPoints[i].y=points[i].y*muly+vy;
    }

    if(!animate)
        XFillPolygon(display, win, gc,
                     LocalPoints,number,Complex,CoordModeOrigin);
    if(isbuffered)
        XFillPolygon(display, cont_pixmap, gc,
                     LocalPoints,number,Complex,CoordModeOrigin);

    if(number>10) /*Byl duzy*/
        free(LocalPoints);
}

void line(ssh_coordinate x1,ssh_coordinate y1,
          ssh_coordinate x2,ssh_coordinate y2,
          ssh_color c)
/* Wyswietlenie lini w kolorze c */
{
    unsigned line_width=default_line_width;

    x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
    y1*=muly;y2*=muly;  /* if window is bigger */

    if(c!=CurrForeground)
    {
        CurrForeground=c;
        XSetForeground(display,gc,Scale[c]);
    }

    if( line_width!=(mulx>muly?muly:mulx) )
    {
        line_width=(mulx>muly?muly:mulx);
        if(ssh_trace_level>1)
            fprintf(stderr,"Set line width to %d\n",line_width);

        XSetLineAttributes(display, gc,line_width,
                           LineSolid,  CapRound, JoinRound);
    }

    if(!animate)
        XDrawLine(display,win, gc, x1, y1, x2, y2);

    if(isbuffered)
        XDrawLine(display,cont_pixmap, gc, x1, y1, x2, y2);
}


int invalidate_screen()//Cały ekran/okno zostanie zmazany
{
    //clear_screen();//Dla pewności?
    return 0;
}

void clear_screen()
/* Czysci ekran przed zmiana zawartosci */
{
    //Why not https://tronche.com/gui/x/xlib/GC/convenience-functions/XSetBackground.html ???
    XSetForeground(display, gc, Scale[bacground] );
    CurrForeground=-1;
    /* Clear screen and bitmap */
    if(!animate)
    {
        if(ssh_trace_level)
            fprintf(stderr,"Clear window\n");
        XFillRectangle(display,win , gc, 0,0,width,height);
    }

    if(isbuffered)
    {
        if(ssh_trace_level)
            fprintf(stderr,"X11: Clear pixmap\n");
        XFillRectangle(display,cont_pixmap , gc, 0,0,width,height);
    }
}

ssh_stat repaint_area(ssh_coordinate* x,ssh_coordinate* y,ssh_natural* width,ssh_natural* height)
{
    if(repaint_flag==1)
    {
        *x=last_repaint_data.x/mulx;
        if(*x<0) *x=0;
        if(*x>org_width) *x=org_width;

        *y=last_repaint_data.y/muly;
        if(*y<0) *y=0;
        if(*y>org_height) *y=org_height;

        *width=last_repaint_data.width/mulx;
        if(*x+*width<0) *width=0;
        if(*x+*width>org_width) *width=org_width-*x;

        *height=last_repaint_data.height/muly;
        if(*y+*height <0) *height =0;
        if(*y+*height>org_height) *height =org_height-*y;
        repaint_flag=0;

        return 0;
    }
    else
        return -1;
}

ssh_stat get_mouse_event(int* xpos,int* ypos,int* click)
/* Odczytuje ostatnie zdazenie myszy */
{
    if(LastMouse.flags!=0)
    {
        *xpos=LastMouse.x/mulx;
        *ypos=LastMouse.y/muly;
        *click=LastMouse.buttons;
        LastMouse.flags=0;
        return 0;
    }
    return -1;
}

static
void SetScaleOld(XColor RGBarray[])
{
    unsigned k;
    XColor RGB;

    RGB.red=0;RGB.green=0;RGB.blue=0;

    for(k=0;k<512;k++)
    {
        RGBarray[k]=RGB;
    }


    if(UseGrayScale)
    {
        for(k=0;k<255;k++)
        {
            long wal=k;
            /*fprintf(stderr,"%u %ul\n",k,wal);*/
            RGBarray[k].red=wal;
            RGBarray[k].green=wal;
            RGBarray[k].blue=wal;
        }
    }
    else
    {
        for(k=0;k<255;k++)
        {
            long wal;
            double kat=(M_PI*2)*k/255.;
            wal=255*sin(kat*1.25);
            if(wal>0)  RGBarray[k].red=wal;
            wal=255*(-sin(kat*0.85));
            if(wal>0)  RGBarray[k].green=wal;
            wal=255*(-cos(kat*1.1));
            if(wal>0)  RGBarray[k].blue=wal;
        }
    }

    RGBarray[255].red=0xffff;
    RGBarray[255].green=0xffff;
    RGBarray[255].blue=0xffff;

    if(ssh_trace_level>1)
        fprintf(stderr,"%s\n","X11: SetScaleOld() completed");
}

static
void SetScale(XColor RGBarray[])
{
    unsigned k;
    XColor RGB;

    RGB.red=0;RGB.green=0;RGB.blue=0;

    //Kolor 0 na pewno czarny
    RGBarray[0]=RGB;

    if(UseGrayScale)
    {
        for(k=1;k<256;k++)
        {
            long wal=k;
            /*fprintf(stderr,"%u %ul\n",k,wal);*/
            RGBarray[k]=RGBarray[0];//Inne pola struktury też moga byc wazne
            RGBarray[k].red=wal;
            RGBarray[k].green=wal;
            RGBarray[k].blue=wal;
            Scale[k]=buildColor(wal,wal,wal);
            RGBarray[k+256].red=wal;
            RGBarray[k+256].green=wal;
            RGBarray[k+256].blue=wal;
            Scale[k+256]=buildColor(wal,wal,wal);
        }
    }
    else
    {
        for(k=1;k<256;k++)
        {
            long wal;
            double kat=(M_PI*2)*k/255.;
            RGBarray[k]=RGBarray[0];//Inne pola struktury też moga byc wazne
            wal=255*sin(kat*1.25);
            if(wal>0)  RGBarray[k].red=wal;
            wal=255*(-sin(kat*0.85));
            if(wal>0)  RGBarray[k].green=wal;
            wal=255*(-cos(kat*1.1));
            if(wal>0)  RGBarray[k].blue=wal;
            Scale[k]=buildColor(RGBarray[k].red,RGBarray[k].green,RGBarray[k].blue);
        }
        for(k=256;k<512;k++)
        {
            long wal=k-256;
            RGBarray[k]=RGBarray[0];//Inne pola struktury też moga byc wazne
            /*fprintf(stderr,"%u %ul\n",k,wal);*/
            Scale[k]=buildColor(wal,wal,wal);
            RGBarray[k].red=wal;
            RGBarray[k].green=wal;
            RGBarray[k].blue=wal;
        }
    }

    //Kolor 255 na pewno biały!
    RGBarray[255].red=0xffff;
    RGBarray[255].green=0xffff;
    RGBarray[255].blue=0xffff;
    Scale[255]=buildColor(255,255,255);

    if(ssh_trace_level>1)
        fprintf(stderr,"%s\n","X11: SetScale() completed");
}

void    set_rgb(ssh_color color,ssh_intensity r,ssh_intensity g,ssh_intensity b)
/* Zmienia definicja koloru. Indeksy 0..255 */
{
    XColor pom;
    unsigned long pixels[1];
    pom.red=r*256;
    pom.blue=b*256;
    pom.green=g*256;
    Scale[color]=buildColor(r,g,b);
    pixels[0]=pom.pixel=Scale[color];
    pom.pixel=0;
#if 0   // W trybie true-color to wydaje się niepotrzebne, a co ciekawsze czasem wywraca program
    if(XAllocColor(display,colormap,&pom)!=0)
    {
        Scale[color]=pom.pixel;
        /*Niepewna metoda,lub wrecz zla */
        XFreeColors(display,colormap,pixels, 1,0);
    }
#endif
}

void delay_ms(ssh_natural ms)
/* Wymuszenie oczekiwania przez pewnej liczby ms */
/* http://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds */
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */
    usleep(ms*1000);
}

void delay_us(ssh_natural ms)
/* Wymuszenie oczekiwania przez pewnej liczby ms */
/* http://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds */
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */
    usleep(ms);
}

/* Event names. Used in "type" field in XEvent structures. Not to be
confused with event masks above. They start from 2 because 0 and 1
are reserved in the protocol for errors and replies. */

static const char* event_names[]={
"Event-0-error",
"Event-1-reply",
"KeyPress-2",
"KeyRelease-3",
"ButtonPress-4",
"ButtonRelease-5",
"MotionNotify-6",
"EnterNotify-7",
"LeaveNotify-8",
"FocusIn-9",
"FocusOut-10",
"KeymapNotify-11",
"Expose-12",
"GraphicsExpose-13",
"NoExpose-14",
"VisibilityNotify-15",
"CreateNotify-16",
"DestroyNotify-17",
"UnmapNotify-18",
"MapNotify-19",
"MapRequest-20",
"ReparentNotify-21",
"ConfigureNotify-22",
"ConfigureRequest-23",
"GravityNotify-24",
"ResizeRequest-25",
"CirculateNotify-26",
"CirculateRequest-27",
"PropertyNotify-28",
"SelectionClear-29",
"SelectionRequest-30",
"SelectionNotify-31",
"ColormapNotify-32",
"ClientMessage-33",
"MappingNotify-34",
"LASTEvent-35"};/* must-be-bigger-than-any-event */

const char* event_name(int code)
{
    if(code >= 0
    && code < sizeof(event_names)/sizeof(event_names[0]) )
        return event_names[code];
    else
        return "Undefined";
}

#include <X11/xpm.h>  /*  TAK POWINNO BYĆ - JAK JEST NORMALNIE ZAINSTALOWANE */
//#include "SYMSHELL/Xpm/xpm.h"

ssh_stat dump_screen(const char* Filename)
/* Zapisuje zawartosc ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
{
    char NameBufor[2048];

    if(strlen(Filename)+5>2048)
    {
        fprintf(stderr, "Filename '%s' is too long!\n", Filename);
        return -1;
    }
    
    sprintf(NameBufor, "%s.xpm", Filename);
    XSync(display, True);

    if (cont_pixmap)
    {
     /* int XpmWriteFileFromPixmap (
     //             Display *  	display,
     //             char *  	filename,
     //             Pixmap  	pixmap,
     //             Pixmap  	shapemask,   //???
     //             XpmAttributes *  	attributes
     //         ) 	*/
        if (XpmWriteFileFromPixmap(display, NameBufor, cont_pixmap, 0, NULL) != XpmSuccess)
        {
            fprintf(stderr, "'dump_screen(%s)' failed \n", NameBufor);
        }
        else
        {
            fprintf(stderr, "'dump_screen(%s)' finished with SUCCES! \n", NameBufor);
        }
    }
    else
    {
        fprintf(stderr, "'dump_screen(%s)' have no pixmap to save! \n", NameBufor);
    }

    return 0;
}
/*#pragma exit close_plot*/
/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-10                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
