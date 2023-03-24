/**
 * \file symshx11.c                                                     *
 * \brief X11 implementation of                                         *
 *      SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++            *
 * ******************************************************************** *
 * \details Najprostszy interface wizualizacyjny zaimplementowany       *
 *          pod X-windows za pomocą biblioteki X11                      *
 *                                                                      *
 * \author W.Borkowski from University of Warsaw                        *
 * \n https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI           *
 * \n https://github.com/borkowsk                                       *
 *//*
 * File changed massively: 21.10.2020                                   *
 *                                                                      *
 * UWAGA!                                                               *
 * WCIĄŻ Z BŁĘDEM NA EXPOSE! TODO , choć raz go już gdzieś usunąłem :-/ *
 *                                                                      *
 ************************************************************************
 *               SYMSHELLLIGHT  version 2023-03-14                      *
 ************************************************************************/

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

/* For transparencies to work use: (TODO CHECK?) */
//#define CREATE_FULL_WINDOW 32

#ifndef NULL
#define NULL __null
#endif

/** Maska poziomów śledzenia 1-msgs 2-grafika 3-grafika detaliczna 4-alokacje/zwalnianie */
 int                    ssh_trace_level = 0;

/** Dla \ref close_plot.\brief Czy zamykać okno od razu, czy dawać "enter"? */
 extern int             WB_error_enter_before_clean;

/** Dla \ref close_plot.\brief Zerowane też gdy "broken-pipe" **/
 static int             opened=0;

/** Domyślna nazwa programu, okna i ikony */
 static char            progname[1024]="WB SIMULATION NAME NOT SET";
/** Domyślna nazwa okna */
 static char            window_name[1024] = "WB-sym-shell";//"WB X-window simulation shell";
/** Domyślna nazwa ikony */
 static char            icon_name[1024] = "WB-sym-shell";

/** Zmienne do zapamiętania wskaźników przekazanych przez funkcje setup*/
 static unsigned        largc=0;            /**< liczna parametrów wywołania */
 static const char**    largv=NULL;         /**< wartości paramtrów wywołania */

/** Parametry z inicjalizacji modułu (shell setup) */
 static int             isbuffered=0;       /**< Czy okno jest buforowane mapą pikseli? */
 static int             animate=0;          /**< Czy odświeżanie, tylko gdy flush_plot, czy na bieżąco? */
 static int             ScreenClip=1;       /**< Czy przycinać okno do wielkości ekranu? */
 static int             UseGrayScale=0;     /**< Czy mapowac kolory indeksowane na odcienie szarości? */

 /* These are used as arguments to nearly every Xlib routine, so it
   * saves routine arguments to declare them global; if there were
   * additional source files, they would be declared extern there */
 static Display         *display=0; /* HANDLER TO Display */
 static int             screen_num; /* SCREEN NUMBER */
 static char            *display_name = NULL;/* Display name. To be read. */
 static unsigned int    display_width=0;   /* Will be filled during initialisation */
 static unsigned int    display_height=0;  /* Will be filled during initialisation */
 static XSizeHints      *size_hints;       /* To jest jeszcze gdzieś używane poza init_plot() Chyba do resize? TODO? */

 static unsigned int    mulx=1,muly=1;             /* Multiplication of x & y */
 static unsigned int    org_width,org_height;      /* Starting Window size */
 static int             ini_a,ini_b,ini_ca,ini_cb; /* Konieczne do działania */
                                                   /* screen_width() i screen_height() i zmiany rozmiaru */

 static Window          win;               /* HANDLER TO MAIN WINDOW */
        XID             _ssh_window=0;     /* Handler for check and external use */

 static unsigned int    width,height;      /* Window size */
 static int             iniX, iniY;        /* Window position */

 /* For transparencies to work use: */
 //#define CREATE_FULL_WINDOW 32 TODO CHECK?
#if CREATE_FULL_WINDOW==32
 static unsigned int    default_depth = 32;
#else
 static unsigned int    default_depth = 24;         /**< CURRENTLY 24bit deph WORK, BUT 32 NOT (TODO!) */
#endif
 static unsigned int    border_width = 4;           /* Four pixels margin */
 static int             window_size = TOO_SMALL;    /* BIG_ENOUGH or TOO_SMALL to display contents */

 static unsigned int    icon_width, icon_height;/* the height and width of the program icon */
 static Pixmap          icon_pixmap;       /* program icon handle */
 static unsigned short  alloc_cont=0;      /* Flag that pixmap has been allocated */

 static Pixmap          cont_pixmap=0;     /* Handle for pixmap replacing / mirroring a window */

 static XTextProperty    windowName;
 static XTextProperty    iconName;

 static GC               gc=NULL;          /* GRAPHIC CONTEXT */

 /* FONT */
 static int              ResizeFont=0;     /* Czy probuje zmieniać rozmiar fontu. TODO? */
 static XFontStruct     *font_info=NULL;   /* Current font */
 static unsigned         ori_font_width = 8;
 static unsigned         ori_font_height = 16;
 static unsigned         font_width = 0;
 static unsigned         font_height = 0;

 /* REPAINT DATA */
 static struct XRect {
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
 static void SetScale(XColor RGBarray[512]);//Ustawianie kolorów

 /* Parametry rysowania */
 static unsigned        default_line_width=2;/* grubość linii */
 static int             transparent_print=0; /* czy napisy transparentne */
 static int             pieMode=-1;/* Tryb wypełniania fragmentu okręgu lub elipsy */

 /* Wejście znakowe? */
 static KeySym           thekey;
 static int              buforek[2]; /* Bufor na znaki z klawiatury. Tylko zerowy znak jest przekazywany */
 //static char bfirst=0;             /* Zmienne na implementacje cykliczną bufora */
 //static char blast=0;              /* aktualnie nie używane... */

 /* Wejście myszowe */
 static int              mouse=0;

 static
 struct { int  flags, x, y;
          unsigned buttons;}        LastMouse={0, 0, 0, 0};

 /* OBSŁUGA SYGNAŁÓW */
 const  int             error_limit=3;             /**< Limit odesłanych błędów od x-serwera */
 static int             error_count=3;             /* antylicznik błędów. Gdy osiąga 0 - koniec programu */

 static int             DelayAction=0;             /* Sterowanie zasypianiem, jeśli program czeka */
 static int             pipe_break=0;              /* Informacja o zerwaniu połączenia z X serwerem */

 /// Default signal handler
 void SigPipe(int num)
 {
     pipe_break=1;
     opened=0;
     signal(num,SIG_IGN);
     fprintf(stderr,"\nX11-SYMSHELL received a SIGNAL #%d\n",num);
     exit(num);
 }

 /// Default X IO handler
 /// \warning TODO: How to distinguish X11 network error from window shutdown?
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
     pipe_break=1; //Process finished with after window close should be treated as normal situation!

     fprintf(stderr,"\nX11-SYMSHELL GOT A IOError\n");
     XSetIOErrorHandler(NULL);/* Kasowanie dotychczasowej funkcji obsługi błędów z wywołania XSetIOErrorHandler(MyXIOHandler); */
     exit( 0 );//-11);//245 was my choice??? !!!

     return -11;
 }

 /// Default X Error handler
 /// \return 0 or never return because of exit() call.
 int MyErrorHandler(Display *xDisplay, XErrorEvent *event)
 {
     char buf[80];

     fprintf(stderr,"\nX11-SYMSHELL received an X error!");
     fprintf(stderr,"\nErr.code  : %d", event->error_code);
     fprintf(stderr,"\tRequest: %d", event->request_code);
     fprintf(stderr,"\tMinor: %d", event->minor_code);
     XGetErrorText(xDisplay, event->error_code, buf, 80);
     fprintf(stderr,"\tErr.text : '%s'\n", buf);

     error_count--;

     if(!error_count)
     {
         pipe_break=1;
         exit(event->error_code);
     }

     return 0;
 }

/** Włącza drukowanie tekstu bez zamazywania tla. Zwraca stan poprzedni */
 int	print_transparently(int yes)
 {
     int ret=transparent_print;
     transparent_print=yes;
     return ret;
 }

/** Czy symulować niezmienność rozmiarów okna */
 void fix_size(int Yes)
 {
     if(ssh_trace_level)
        fprintf(stderr,"fix_size() not implemented\n");
 }

/** Ustala czy mysz ma być obsługiwana.
   \details W X11 zawsze jest, ale można ją ignorować */
 int mouse_activity(int yes)
 {
     int pom=mouse;
     mouse=yes;
     return pom;
 }

/** Zwraca aktualny kolor tła - nowa wersja? */
 ssh_color background()
 {
     return bacground;
 }

/** Ustala index koloru do czyszczenia okna itp */
 void set_background(ssh_color c)
 {
    if(c>=0 && c<=NumberOfColors)
       bacground=c;
    else
       bacground=0;
 }

/** Ustala czy ma być buforowanie okna */
void buffering_setup(int _n)
{
    if(_n)
        animate=1;
    else
        animate=0;
    if(animate)        /* Musi byc włączona bitmapa buforująca */
         isbuffered=1;  /* żeby można było na nia pisać */
 }

 /** Czy jest buforowane? */
 unsigned get_buffering()
 {
     return animate;
 }

 /** Zamykanie i zwalnianie zasobów */
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

/** Zabezpiecza przed ukrytą rekurencją w close_plot */
static int inside_close_plot=0;

/** Closing graphics / virtual graphics / semigraphics */
void close_plot()
{
    if(inside_close_plot)
        return;

    if(opened)
    {
        inside_close_plot=1;
        XSync(display,1/*DISCARD EVENTS*/);
        _ssh_window=0; /* Do not use this window anymore!!!*/

        if(WB_error_enter_before_clean)
        {
            fflush(stdout);
            if(!pipe_break)
            {
                char* kom="(Press ANY KEY to close graphic window)";
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

/** Allocates a pixmap of the appropriate size to the contents of the window */
static void ResizeBuffer(unsigned int nwidth,unsigned int nheight)
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

/** Ładowanie jakiegoś fontu */
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
            exit( -1 ); /* Nie ma żadnego fontu */
        else
            return ;    /* Pozostaje stary */
    }

    if( (*font_info) != NULL ) /* Usuwa stary font */
        XUnloadFont(display, (*font_info)->fid);
    (*font_info)=l_font_info;   /* zapamiętuje nowy do użycia */

    /* Specify font in gc */
    XSetFont(display, *gc, l_font_info->fid );

    /* Get string widths & height */
    font_width = XTextWidth(*font_info, "X", 1);
    font_height = (*font_info)->ascent + (*font_info)->descent;

    if(ssh_trace_level)
        fprintf(stderr,"X11: %s:font %ux%u\n",icon_name,font_width,font_height);
}

/** Awaryjna zawartość okna, gdy jest za małe */
static void _tooSmall(Window win, GC gc, XFontStruct* font_info)
{
    char *string1 = "Too Small";
    int y_offset, x_offset;
    y_offset = font_info->ascent + 2;
    x_offset = 2;

    /* Output text, centered on each line */
    XDrawString(display, win, gc, x_offset, y_offset, string1,
                strlen(string1));
}

/** Kopiuje fragment mapy okna na okno */
inline
static void _place_graphics(Window win,GC gc,
                           int      area_x,int          area_y,
                           unsigned area_width,unsigned area_height)
{
    XCopyArea(display, cont_pixmap, win, gc,
              area_x/*src_x*/, area_y /*src_y*/,
              area_width, area_height,
              area_x/*dest_x*/, area_y/*dest_y*/);
}

/** Obsługa zdarzeń X11 */
static void _read_XInput()
{
    static int buffer_empty=1;

    XEvent report;  /* Miejsce na odczytywane zdarzenia */

    if(pipe_break)	/* Musi zwrócić EOF */
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
            _tooSmall(win, gc, font_info);
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
                /* Sumuje z marginesem, także, żeby pokrywało wszystkie zdarzenia expose. TODO. Niezbyt działa! */
                if(last_repaint_data.x>report.xexpose.x)
                    last_repaint_data.x=report.xexpose.x;

                if(last_repaint_data.y>report.xexpose.y)
                    last_repaint_data.y=report.xexpose.y;

                /* Tu jest chyba źle */
                if(last_repaint_data.width<report.xexpose.width)
                    last_repaint_data.width+=report.xexpose.width;

                if(last_repaint_data.height<report.xexpose.height)
                    last_repaint_data.height+=report.xexpose.height;

                repaint_flag=1;/* Są już dane dla repaint */

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
                _place_graphics(win, gc ,
                               report.xexpose.x,report.xexpose.y,
                               report.xexpose.width,report.xexpose.height
                               );
                DelayAction=0;/* Pojawiła się aktywność. Nie należy spać! */
            }
        }
        break;

    case MappingNotify:
        XRefreshKeyboardMapping((XMappingEvent *)&report );
        DelayAction=0;/* Pojawiła się aktywność. Nie należy spać! */
        break;

    case ConfigureNotify:
        DelayAction=0;/* Pojawiła się aktywność. Nie należy spać! */
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
            break; /* Nic się nie zmieniło */
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
        DelayAction=0;/* Pojawiła się aktywność. Nie należy spać! */
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
        DelayAction=0;/* Pojawiła się aktywność. Nie należy spać! */

        *buforek=*Bufor;/* Na zewnątrz widziane w zmiennej "buforek" */

        if(KeyCount!=1)
            *buforek=NODATA;

        if(ssh_trace_level)
            fprintf(stderr,"KeyPress:%c %x \n ",*Bufor,(int)(*Bufor));

        if(*Bufor==0x3 || *Bufor==0x4) /* User przerwał w oknie X11 */
            *buforek=EOF;
    } break;

    case ClientMessage: /* https://tronche.com/gui/x/xlib/events/client-communication/client-message.html */
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


/** Tworzy Graphics Contex */
static void makeGC(Window win,GC* gc,XFontStruct* font_info)
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

/** Zmienia napis w belce okna */
void set_title(const char* window_name)
{
    /* Change window title bar */
    /* https://tronche.com/gui/x/xlib/ICC/client-to-window-manager/XStoreName.html */
    XStoreName(display,win, window_name);
}

/** Przekazanie parametrów wywołania do konfiguracji symmshella */
void shell_setup(const char* title,int iargc,const char* iargv[])
{
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
            fprintf(stderr,"SYMSHELL for X11 environment. Compiled %s\n",__DATE__);
            fprintf(stderr,"Supported switches:"
                           "\n\t -mapped+/-] "
                           "\n\t -buffered[+/-]"
                           "\n\t -sclip[+/-]"
                           "\n\t -bestfont[+/-] "
                           "\n\t -traceevt[+/-] "
                           "\n\t -gray[+/-] "
                           "\n");
            fprintf(stderr,"You can always gracefully break the program sending\n"
                           " the  ^C  or  ^D  key	 to  g_r_a_p_h_i_c_s  w_i_n_d_o_w !\n"
                           "Other methods break the program immediately by exit call!\n");
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
                        /* Jeśli user chce tryb z mapowaniem to nie będziemy */
                        animate=0; /* nie animować? TODO CHECK? */
                    }
                    else
                        if(strncmp(largv[i],"-buffered",9)==0)
                        {
                            animate=(largv[i][9]=='+')?1:0;
                            fprintf(stderr,"Buffered is %s\n",(animate?"ON":"OFF"));
                            /* Musi byc włączona bitmapa buforująca */
                            isbuffered=animate;/* żeby można było na nią pisać */
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

/** Właściwa dla platformy inicjacja grafiki/semigrafiki/grafiki wirtualnej */
ssh_stat init_plot(ssh_natural a,ssh_natural b,ssh_natural ca,ssh_natural cb)
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
        fprintf(stderr," Preferred depth:%d\n",default_depth);
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
    else //Czy to się w ogóle ma szansę zdarzyć? Raczej wyleci wcześniej na błędzie?!
        fprintf( stderr, "X11: %s: Window not created! ", progname);

    /* Get available icon sizes from window manager and create appropriate bitmap */
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

        //May be not supported in moder window managers?
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
    size_hints->min_width =  width;  /* Startup width/height */
    size_hints->min_height = height; /* are minimal */

    /* Set window manager hints:
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
    class_hints->res_class = "ssh_win";
    //void XSetWMProperties(display, w, window_name, icon_name, argv, argc, normal_hints, wm_hints, class_hints);
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
    makeGC(win, &gc, font_info);
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

    /* Czyści, żeby wprowadzić ustalone tlo */
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

/** Aktualne rozmiary łańcucha */
/** \details ...potrzebne do jego pozycjonowania */
/** wysokość - LICZONA BARDZO PRYMITYWNIE - jako wysokość pierwszego znaku */
ssh_natural  string_height(const char* str)
{
    return char_height(*str); /* Pierwszy znak! */
}

/** Aktualne rozmiary łańcucha */
/** \details ...potrzebne do jego pozycjonowania */
/** szerokość liczona porządnie przez XTextWidth z Xlib */
ssh_natural  string_width(const char* str)
{
    int pom=XTextWidth(font_info,str,strlen(str))/mulx;
    if(pom<1)pom=1;
    return pom;
}

/** Reconciliation of memory, queue and screen contents */
void flush_plot()
{
    if(ssh_trace_level>1)
        fprintf(stderr,"X11: FLUSH %s",icon_name);

    if(!opened)
    {
        fprintf(stderr,"X11: %s","SYMSHELL graphics not initialised or pipe broken");
        return;
    }

    if(animate)
        {/* Wyświetlenie z bitmapy */
            if(ssh_trace_level>1)
                fprintf(stderr,"X11: DOING BITBLT");

            XCopyArea(display, cont_pixmap, win, gc,
               0/*src_x*/, 0/*src_y*/,
               width, height,
               0/*dest_x*/, 0/*dest_y*/);
        }

    if(ssh_trace_level>1)
        fprintf(stderr,"X11: XFLUSH\n");

    XFlush(display);/* Xlib function to send everything in the send queue */

    error_count=error_limit;/* Błędy zliczamy od nowa */
}

/* GETTING INPUT */

/** Za pierwszym razem zwraca '\r'
żeby zasygnalizować ze trzeba wyrysować ekran */
static int first_to_read=0;

/** Bardzo zależna od platformy funkcja sprawdzająca, czy jest jakieś wejście "z okna grafiki" */
int  input_ready()
{
    if(first_to_read)
        return 1;

    if(pipe_break==1)
    {
        if(ssh_trace_level)
            fprintf(stderr,"'pipe_break' flag detected in input_ready() in '%s'.\n",icon_name);
        return EOF;
    }

    if(XPending(display)!=0) 	/* Sprawdzenie, czy nie ma zdarzeń */
    {			                /*SĄ JAKIEŚ!*/
        buforek[0]=NODATA; 	    /*Asekuranctwo ? */
        _read_XInput(); 		    /* Przetwarzanie zdarzeń */
        if(buforek[0]!=NODATA)	/* Czy jest cos do zwrócenia jako znak? */
        {
            first_to_read=buforek[0]; /*Zostanie przeczytane przez get_char() */
            buforek[0]=NODATA;
            return 1;		    /* Wiec można wywołać get_char() */
        }
    }

    return 0;
}

/** Odesłanie znaku na wejście. Zwraca 0 jeśli nie ma miejsca */
ssh_stat  set_char(int c)
{
    if(first_to_read!=0)/* Nie odebrano */
            return 0;

    first_to_read=c;
    return 1;
}

/** Odczytywanie znaków sterowania */
int  get_char()
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

    _read_XInput();
    pom=buforek[0];
    buforek[0]=NODATA;
    return pom;
}


/* Used for redraw & in print */

static char straznik1=0x77;
static char bufor[1024];
static char straznik2=0x77;
static int ox,oy;

/** Wyprowadzenie tekstu na ekran (bw) */
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...)
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exceed 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & height */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/                                                                                  assert(font_info);
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplication of coordinates */
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

/** Wyprowadzenie tekstu na ekran (index colors) */
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
        fprintf(stderr,"symshell.print(...) - line exceed 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & height */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/                                                                                  assert(font_info);
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplication of coordinates */
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

/** Wyprowadzenie tekstu na ekran (default colors) */
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
        fprintf(stderr,"symshell.print(...) - line exceed 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & height */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/                                                                                      assert(font_info);
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplication of coordinates */
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

/** BUDOWANIE WEWNĘTRZNYCH KOLORÓW W X11 - TODO CHECK
 *  \brief Tworzenie kolory RBG w X11
 *  \details Funkcja jest inline i tylko dla tego pliku źródłówego
 *  \see https://www.geeksforgeeks.org/inline-function-in-c/
 * */
static inline
unsigned long buildColor(unsigned char red, unsigned char green, unsigned char blue)
{
    return ( (unsigned long)(red) << 16) +
           ( (unsigned long)(green)<< 8) +
           ( (unsigned long)(blue) ) ;
}

/** BUDOWANIE WEWNĘTRZNYCH KOLORÓW W X11 - TODO CHECK
 *  \brief Tworzenie kolory RBGA w X11
 *  \details Funkcja jest inline i tylko dla tego pliku źródłówego
 *  \see https://www.geeksforgeeks.org/inline-function-in-c/
 * */
static inline
unsigned long buildTransparentColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char transparency)
{
    return ( (unsigned long)(transparency) << 24 ) +
           ( (unsigned long)(red) << 16) +
           ( (unsigned long)(green)<< 8) +
           ( (unsigned long)(blue) ) ;
}

/** Drukuje z możliwością ustawienia tuszu poprzez RGB */
void print_rgb(ssh_coordinate x,ssh_coordinate y,
               ssh_intensity r,ssh_intensity g,ssh_intensity b,            /*- składowe koloru tekstu */
               ssh_color back,const char* format,...)
{
    size_t len=0;
    unsigned font_height=0;

    va_list argptr;

    va_start(argptr, format);

    vsprintf(bufor, format, argptr);

    va_end(argptr);

    if(straznik1!=0x77 || straznik2!=0x77)
    {
        fprintf(stderr,"symshell.print(...) - line exceed 1024b!");
        exit(-__LINE__);
    }

    /* Print string in window */
    /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & height */
    /*
    width1 = XTextWidth(font_info, string1, len1);
*/                                                                      assert(font_info);
    font_height = font_info->ascent + font_info->descent;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplication of coordinates */
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

/** Wyświetlenie punktu na ekranie w kolorze rgb ustawionym ze składowych */
void plot_rgb(ssh_coordinate x,ssh_coordinate y,ssh_intensity r,ssh_intensity g,ssh_intensity b)
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

/** Wyświetlenie punktu na ekranie w kolorze domyślnym */
void plot_d(ssh_coordinate x,ssh_coordinate y)
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

/** Wyświetlenie punktu na ekranie w kolorze indeksowanym */
void plot(ssh_coordinate x,ssh_coordinate y,ssh_color c)
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

/** Ustala styl rysowania linii
 * \details possible values: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED
 * \warning NOT IMPLEMENTED!
 * */
int line_style(int Style)
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

/** Ustala aktualny kolor linii za pomocą typu ssh_color
 *  \warning style NOT IMPLEMENTED!
 * */
void set_pen(ssh_color c,ssh_natural size,ssh_mode style)
{
    if(PenColor!=Scale[c])
    {
        PenColor=Scale[c];
        default_line_width=size;
    }
}

/** Ustala aktualny kolor linii za pomocą składowych RGB
 *  \warning style NOT IMPLEMENTED!
 * */
void set_pen_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_natural size,ssh_mode style)
{
    PenColor=buildColor(r,g,b);
    default_line_width=size;
}


/** Ustala aktualny kolor linii za pomocą składowych RGBA TODO TEST
 *  \warning style NOT IMPLEMENTED!
 * */
void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a,ssh_natural size,ssh_mode style)
{
    PenColor=buildTransparentColor(r,g,b,a);
    default_line_width=size;
}

ssh_natural get_line_width()
{
    return default_line_width;
}

/** Ustala szerokość linii. Może być kosztowne. Zwraca stan poprzedni */
ssh_natural line_width(ssh_natural width)
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

/** Ustala aktualny kolor wypełnień za pomocą typu ssh_color */
void set_brush(ssh_color c)
{
    if(BrushColor!=Scale[c])
    {
        BrushColor=Scale[c];
    }
}

/** Ustala aktualny kolor wypełnień za pomocą składowych RGB */
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    BrushColor=buildColor(r,g,b);
}

/** Ustala aktualny kolor wypełnień za pomocą składowych RGBA, TODO TEST */
void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a)
{
    BrushColor=buildTransparentColor(r,g,b,a);
}

/** wyrysowanie linii w kolorze domyślnym */
void line_d(int x1,int y1,int x2,int y2)
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

    x1*=mulx;x2*=mulx; /* Multiplication of coordinates */
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

/** wyrysowanie okręgu w kolorze 'c' */
void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
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

    x*=mulx;y*=muly;      /* Multiplication of coordinates */
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

/** wyrysowanie elipsy w kolorze indeksowanym 'c' */
void ellipse_d(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b)
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

    x*=mulx;y*=muly;      /* Multiplication of coordinates */
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

/** wyrysowanie kola w kolorach domyślnych */
void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
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

    x*=mulx;y*=muly;      /* Multiplication of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys. Nie jest pewne czy toi potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
    }
}

/** Wypełnienie elipsy w kolorach domyślnych*/
void fill_ellipse_d(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b)
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

    x*=mulx;y*=muly;      /* Multiplication of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys. Nie jest pewne czy to potrzebne, ale zdaje się w Windows tak działa
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

/** rysuje łuk kołowy o promieniu r i kolorach DOMYŚLNYCH*/
void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_radian start,ssh_radian stop)
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

/** rysuje łuk kołowy o promieniu r i kolorach indeksowanych */
void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r, ssh_radian start,ssh_radian stop,ssh_color c)
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

/** rysuje łuk eliptyczny o półosiach a i b i domyślnym kolorze */
void earc_d(ssh_coordinate x,ssh_coordinate y,
            ssh_natural a,ssh_natural b,
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

/** rysuje łuk eliptyczny w kolorze indeksowanym c */
void earc(ssh_coordinate x,ssh_coordinate y,
          ssh_natural a,ssh_natural b,
          ssh_radian start,ssh_radian stop,ssh_color c)
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

/** Wypełnia łuk kołowy w kolorze domyślnym */
void fill_arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r, /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start,ssh_radian stop,                /* początku i końcu */
                ssh_bool pie)                                    /* początek i koniec łuku */
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //TODO. Obrys - nie jest pewne czy to potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2,r2*2,angle1,angle2);
    }
}

/** Wypełnia łuk kołowy w kolorze indeksowanym c */
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys. Nie jest pewne czy to potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2,r2*2,angle1,angle2);
    }
}

/** Wypełnia łuk eliptyczny w kolorze domyślnym */
void fill_earc_d(ssh_coordinate x,ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a,ssh_natural b,                          /* o półosiach 'a' i 'b' */
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

    x*=mulx;y*=muly;      /* Multiplications of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys. Nie jest pewne czy to potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
    }
}

/** Wypełnia łuk eliptyczny w kolorze indeksowanym */
void fill_earc(ssh_coordinate x,ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a,ssh_natural b,                            /* o półosiach 'a' i 'b' */
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

    x*=mulx;y*=muly;      /* Multiplication of coordinates */
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

    if(PenColor!=-1 && PenColor!=BrushColor) //Obrys. Nie jest pewne czy to potrzebne, ale zdaje się w Windows tak działa
    {
        XSetForeground(display,gc,PenColor);
        if(!animate)
            XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2,angle1, angle2);
        if(isbuffered)
            XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2,angle1,angle2);
    }
}
/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2, w kolorze rbg określonym składowymi koloru */
void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,
                   ssh_coordinate x2,ssh_coordinate y2,
                   ssh_intensity r,ssh_intensity g,ssh_intensity b)
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

/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2, w kolorze domyślnym */
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

/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2, w kolorze indeksowanym c */
void fill_rect(int x1,int y1,int x2,int y2,ssh_color c)
{                                                                                                 assert( display!=NULL);
                                                                                                   assert( gc != NULL);
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

/** Wypełnia WIELOKĄT przesunięty o vx,vy w kolorach domyślnych */
void fill_poly_d(ssh_coordinate vx,ssh_coordinate vy,
                 const ssh_point points[],int number)
{
    static XPoint   _LocalTable[10];
    XPoint*          LocalPoints=_LocalTable;
    int i;

    if(number<=2)
        return; /*Nie da się rysować
        wielokąta o dwu punktach lub
            mniej*/

    if(number>10) /*Jest za duży. Alokacja*/
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

    if(number>10) /*Byl duży*/
        free(LocalPoints);/* dealokacja */
}

/** Wypełnia kolo w kolorze c */
void fill_circle(ssh_coordinate x, ssh_coordinate y, ssh_natural r, ssh_color c)
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

/** Wyświetla okręg w kolorze c */
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c)
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

    x*=mulx;y*=muly;   /* Multiplications of coordinates */
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

/** Wypełnia wielokąt przesunięty o vx,vy w kolorze indeksowanym c */
void fill_poly(ssh_coordinate vx,ssh_coordinate vy,
               const ssh_point points[],int number,  /* - tablica wierzchołków wielokąta i jej długość */
               ssh_color c)
{
    static XPoint _LocalTable[10];
    XPoint* 	LocalPoints=_LocalTable;
    int i;

    if(number<=2)
        return; /*Nie da się rysować
        wielokąta o dwu punktach lub
            mniej*/

    if(number>10) /*Jest za duży. Alokacja*/
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

/** Wyświetlenie linii w kolorze c */
void line(ssh_coordinate x1,ssh_coordinate y1,
          ssh_coordinate x2,ssh_coordinate y2,
          ssh_color c)
{
    unsigned line_width=default_line_width;

    x1*=mulx;x2*=mulx; /* Multiplications of coordinates */
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


/** Informs the system that we do not care about the visibility of previous graphics operations
 * \details It allows you to inform the graphic system that
 * the entire screen / window will be blurred with new content without cleaning.
 * \warning DO NOTHING UNDER X11
 * \todo XSync(...,discards)? https://tronche.com/gui/x/xlib/event-handling/XSync.html
 */
int invalidate_screen()
{
    //clear_screen();//Dla pewności?
    return 0;
}

/** Clears the screen before changing the content to something new */
void clear_screen()
{
    /* Why not https://tronche.com/gui/x/xlib/GC/convenience-functions/XSetBackground.html */
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

/** Specifies the area of the screen that needs to be redrawed
 *  \details i.e: ... due to interactions between the windows.
 *  \warning UNLESS IT IS WORKING CORRECTLY!
 */
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

/** Reads the last mouse event */
ssh_stat get_mouse_event(int* xpos,int* ypos,int* click)
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

/** Sets the default indexed color scale. Old version. */
static void SetScaleOld(XColor RGBarray[])
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

/** Sets the default indexed color scale */
static void SetScale(XColor RGBarray[])
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

/** Redefines one indexed color. Indices 0..255 */
void    set_rgb(ssh_color color,ssh_intensity r,ssh_intensity g,ssh_intensity b)
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

/** Make the program wait for a certain number of ms
* \see \n http://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds */
void delay_ms(ssh_natural ms)
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */
    usleep(ms*1000);// 1 ms = 1 000 μs
}

/** Make the program wait for a certain number of μs
* \see \n http://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds */
void delay_us(ssh_natural us)
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */
    usleep(us);
}

/** \brief X11 event names.
   \details Used in "type" field in XEvent structures. Not to be
   confused with event masks. They start from 2 because 0 and 1
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

/** \return Name of the X11 event */
const char* event_name(int code)
{
    if(code >= 0
    && code < sizeof(event_names)/sizeof(event_names[0]) )
        return event_names[code];
    else
        return "Undefined";
}

#include <X11/xpm.h>  /*  THIS SHOULD LOOK LIKE WHEN Xpm IS NORMALLY INSTALLED */
//#include "SYMSHELL/Xpm/xpm.h"

/** Saves the screen content to a graphic file in the natural platform format: BMP, XBM, etc. */
ssh_stat dump_screen(const char* Filename)
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
/*v******************************************************************/
/*              SYMSHELLLIGHT  version 2023-03...                   */
/*v******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/*v******************************************************************/
