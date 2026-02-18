/*****************************************************************************************
* @file
* @brief Implementacja najprostrzego interface'u wizualizacyjnego dla MS Windows 32.
* ====================================================================================== */
/// @date 2026-02-18 (last modification)
/**
* @details
*   U¿ywano w of 1997 roku w ró¿nych kompilatorach C++ Borland i Microsoftu oraz w GCC na Windows
*   Ostatnia modyfikacja/Last modification: 28.10.2014
*   UWAGA: `ProcessMsg` u¿ywa teraz, zamiast NULL, uchwytu okna pobieraj¹c komunikat — to powinno
*   dzia³aæ lepiej, ale pewnoœci nie ma.
* */

//Partially based on Borland and Microsoft examples
static char* Copyright=	"SYMSHELL LIBRARY(c)1997-2014 ;\n"
                        "By Wojciech Borkowski, Warsaw University\n("
                        __DATE__" - compilation timestamp)\n";
//Define EXTERN_WB_ABOUT and function of following prototype for redefine usage of ShellAbout
//int wb_about(const char* window_name); //Z biblioteki albo dostarczona z programem

extern int WB_error_enter_before_clean; /* For controlling a closing graphics window on error. */

#include <windows.h>
#include <windowsx.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <setjmp.h>

#include "../INCL/symshell.h"       // prototypes of symshell graphix primitives

#include <process.h>
#include "_sig_msg.h"       // for compatibility with wb_posix.
#include "symshwin.h"       // prototypes specific to this application

#define OLD_COLOUR_SCALE (0)   //Skala kolorów jak na mapie fizycznej

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
//TYMCZASEM - OSTRZE¯ENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif

#define MY_WIN_STYLE		      (WS_OVERLAPPEDWINDOW/* | WS_HSCROLL | WS_VSCROLL*/)
#define MAXWRITE		0xfff0	  ///< Bezpieczny rozmiar bufora
#define MINUSERCOMMAND  IDM_EXIT  ///< Komenda u¿ytkownika o najni¿szym mo¿liwym numerze

/* FOR OTHER MODULES */
const char *_ssh_grx_module_name="WINDOWS";
HINSTANCE	WB_Instance=0;
HINSTANCE	WB_PrevInstance=0;
HWND		WB_Hwnd=0;					  ///< ???
HWND		MyHwnd=0;					  ///< Main window handle.
char szAppName[128] = "SYMSHELL";         ///< The name of this application, if not in resources
char szClassName[128] = "CLASS_SYMSHELL"; ///< The name of window class

/* LOCAL VARIABLES */
static const char* progname="WB SYMSHELL APPLICATION "__DATE__;
static char window_name[1024]="Windows WB SYMSHELL interface "__DATE__; //TODO - z jakiegoœ powodu MessageBox traktuje to niepoprawnie?
static char icon_name[1024]="Windows SYMSHELL "__DATE__;

static	HACCEL	    hAccelTable; ///< For read from resources.
static	HDC     	    MyHdc=0; ///< Current HDC.
static	HDC	            WnHdc=0; ///< Window HDC.
static	HDC	            MbHdc=0; ///< Memory bitmap HDC.
static	HMENU	     MainMenu=0; ///< Menu handle loaded from resources.
static	HBITMAP	VirtualScreen=0; ///< Memory bitmap CreateCompatible BitBlt.
static	HFONT	     UserFont=0; ///< Handle to extra font.
static	TEXTMETRIC	  font_info; ///< Struktura na in formacje o aktualnym foncie.
static	MSG	                msg; ///< Struktura na aktualny komunikat.

static COLORREF curent_pen_rgb=RGB(0,0,0);
//static COLORREF curent_brush_rgb=RGB(0,0,0);
static COLORREF colors[PALETE_LENGHT];	    ///< Tablice uchwytów do kolorów.

static HBRUSH curent_brush=NULL;			///< Aktualny uchwyt do pêdzla.
static HBRUSH free_style_brush=NULL;		///< Alokowany pêdzel dowolny.
static HBRUSH brushes[PALETE_LENGHT];		///< Tablice uchwytów do pêdzli.

static HPEN curent_pen=NULL;				///< Aktualny uchwyt do pióra.
static HPEN free_style_pen=NULL;			///< Alokowane dowolne pióro.

typedef struct
{
    HPEN handle;
    int  size;	///< Gruboœci zaalokowanych piór.
    int  style;	///< Style dla zaalokowanych piór.
}	piora;

static piora	pens[PALETE_LENGHT];		///< Tablice uchwytów do piór.

static int NoResources=1;			///<  No resources attached to exe file.

static int WindowClosed=1;			///<  Window destroyed/notopen flag for close_plot.
static int curr_color=-1;			///<  Indeks koloru w aktualnym pisaku.
static int curr_fill=-1;			///<  Indeks koloru w aktualnym pêdzlu.
static int Background=0;			///<  Indeks koloru t³a.

static int is_mouse=0;
static int animate=0;
static int is_buffered=0;
static int transparently=0;
static int Flexible=0;			    ///<  Czy dopuœciæ elastyczny "resizing" okna.

static int DelayTime=0;			    ///<  Delay time after gr. sync.
static int LineWidth=1;
static int LineStyle=SSH_LINE_SOLID;
static int mulx=1;
static int muly=1;
static int W_width,W_height;
static int ini_width;		       ///<  MAXX+1 obszaru bitowego.
static int ini_height;		       ///<  MAXY+1 obszaru bitowego.
static int ForceHeight=0;	       ///<  Na rozmiary okna wymuszone parametrami wywo³ania ("-height=").
static int ForceWidth=0;	       ///<  Na rozmiary okna wymuszone parametrami wywo³ania ("-width=").
static int ini_col;			       ///<  Ile kolumn tekstu dodatkowo.
static int ini_row;			       ///<  Ile wierszy tekstu dodatkowo.
static int Is_finfo=0;
static const char* UserFontStr=NULL;

static int CharToGet=0;			   ///<  ???
static int UseGrayScale=0;	       ///<  Flaga u¿ycia skali szaroœci.

// PREDECLARATIONS OF LOCAL FUNCTIONS:
// ===================================

static HPEN GetMyPen(ssh_color color,int size,int style);
static HBRUSH GetMyBrush(ssh_color color);
static HFONT MyCreateFont( const char* fontstr );
static HDC GetVirtualScreen(void);
static HDC GetRealScreen(void);

// Wewnêtrzne œledzenie wywo³añ.
// /////////////////////////////
static  int trace_level=0;			//Maska poziomów œledzenia 1-msgs 2-grafika 4-alokacje/zwalnianie
//  np:
//  if(trace_level & 4)
//      fprintf(stderr," FREE RESOURCES.\n");
//
// ALBO
//
//  _TRACE(4)
//  fprintf(stderr," FREE RESOURCES.\n");
// _TREND

#define _TRACE(__LL__)   if(trace_level & __LL__) { fprintf(stderr,"SYMSHWIN:::%s ",__FUNCTION__);
#define _TREND        fprintf(stderr,"\n"); }

#define LOCAL static

#ifdef EXTERN_WB_ABOUT
extern int wb_about(const char* window_name); //Z biblioteki albo dostarczona z programem
#else
static int wb_about(const char* window_name)
{
    char dummy[256]; //Trochê pustych bajtów dla zmy³y (???)
    extern HINSTANCE WB_Instance;
    extern HWND		 WB_Hwnd;
    HICON hIcon=0;

    hIcon=LoadIcon(WB_Instance, MAKEINTRESOURCE(IDI_APPICON));
    ShellAbout(WB_Hwnd,window_name,Copyright ,hIcon); // Powinno byæ LPCWSTR?

    DeleteObject(hIcon);
    return 0;
}
#endif

void symshell_about(const char* my_window_name)
{
    wb_about(my_window_name);
}

static int error_proc(void)
{
    DWORD code=GetLastError();
    char  bufor[1024];
    sprintf(bufor,"Windows code of error: GetLastError()==%ul",code);
    MessageBoxA(MyHwnd,bufor,"SYMSHELL GRAPHICS ERROR",MB_ICONSTOP);
    return 'A';// abort();
}

static HDC GetRealScreen(void)
{
    if(MyHdc==0)
        {
        MyHdc=GetDC(MyHwnd);
        //Gdyby mia³y byæ inne fonty ni¿ domyœlny
        if(UserFont) //Not NULL
        {
            SelectObject(MyHdc,UserFont);
        }
        GetTextMetrics(MyHdc,&font_info);
        Is_finfo=1;
        }
    return MyHdc;
}


static HDC GetVirtualScreen(void)
{
    if(MbHdc==0)
        {
        RECT rc;

        HDC WindowDC=GetDC(MyHwnd);
        MbHdc=CreateCompatibleDC(WindowDC);
        VirtualScreen=CreateCompatibleBitmap(WindowDC,W_width*mulx,W_height*muly);
        SelectObject(MbHdc,VirtualScreen);
        GetClientRect(MyHwnd, &rc);
        FillRect(MbHdc, &rc, GetMyBrush(Background));
        ReleaseDC(MyHwnd,WindowDC);
        //Gdyby mia³y byæ inne fonty ni¿ domyœlny
        if(UserFont) //Not NULL!
        {
            SelectObject(MbHdc,UserFont);
        }
        GetTextMetrics(MbHdc,&font_info);
        Is_finfo=1;
        }
    return MbHdc;
}

static int ReadFontSettings(const char* fontsettingsfile,LOGFONT* LogFont)
{
    if(LogFont==NULL) goto CATCHERROR; //B³¹d!
    if(fontsettingsfile==NULL)  goto CATCHERROR; //B³¹d!

    LogFont->lfHeight=GetPrivateProfileInt("FONT","lfHeight",0,fontsettingsfile);
    LogFont->lfWidth=GetPrivateProfileInt("FONT","lfWidth",0,fontsettingsfile);
    LogFont->lfEscapement=GetPrivateProfileInt("FONT","lfEscapement",0,fontsettingsfile);;
    LogFont->lfOrientation=GetPrivateProfileInt("FONT","lfOrientation",0,fontsettingsfile);;
    LogFont->lfWeight=GetPrivateProfileInt("FONT","lfWeight",0,fontsettingsfile);;
    LogFont->lfItalic=GetPrivateProfileInt("FONT","lfItalic",0,fontsettingsfile);;
    LogFont->lfUnderline=GetPrivateProfileInt("FONT","lfUnderline",0,fontsettingsfile);;
    LogFont->lfStrikeOut=GetPrivateProfileInt("FONT","lfStrikeOut",0,fontsettingsfile);;
    LogFont->lfCharSet=GetPrivateProfileInt("FONT","lfCharSet",0,fontsettingsfile);;
    LogFont->lfOutPrecision=GetPrivateProfileInt("FONT","lfOutPrecision",0,fontsettingsfile);;
    LogFont->lfClipPrecision=GetPrivateProfileInt("FONT","lfClipPrecision",0,fontsettingsfile);;
    LogFont->lfQuality=GetPrivateProfileInt("FONT","lfQuality",0,fontsettingsfile);;
    LogFont->lfPitchAndFamily=GetPrivateProfileInt("FONT","lfPitchAndFamily",0,fontsettingsfile);;
    GetPrivateProfileString(
        "FONT",             //LPCTSTR lpAppName,        // points to section name
        "lfFaceName",       //LPCTSTR lpKeyName,        // points to key name
        "",                 //LPCTSTR lpDefault,        // points to default string
        LogFont->lfFaceName,//LPTSTR lpReturnedString,  // points to destination buffer
        LF_FACESIZE,        //DWORD nSize,              // size of destination buffer
        fontsettingsfile    //LPCTSTR lpFileName        // points to initialization filename
        );
   return 1;
CATCHERROR:
   return 0;
}

static void WriteFontSettings(const char* fontsettingsfile,LOGFONT* LogFont)
{
    char buf[1024];
                            //pointer to section name, pointer to key name, pointer to string to add, pointer to initialization filename
    WritePrivateProfileString("FONT","lfHeight",_ltoa(LogFont->lfHeight,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfWidth",_ltoa(LogFont->lfWidth,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfEscapement",_ltoa(LogFont->lfEscapement,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfOrientation",_ltoa(LogFont->lfOrientation,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfWeight",_ltoa(LogFont->lfWeight,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfItalic",_ltoa(LogFont->lfItalic,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfUnderline",_ltoa(LogFont->lfUnderline,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfStrikeOut",_ltoa(LogFont->lfStrikeOut,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfCharSet",_ltoa(LogFont->lfCharSet,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfOutPrecision",_ltoa(LogFont->lfOutPrecision,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfClipPrecision",_ltoa(LogFont->lfClipPrecision,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfQuality",_ltoa(LogFont->lfQuality,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfPitchAndFamily",_ltoa(LogFont->lfPitchAndFamily,buf,10),fontsettingsfile);
    WritePrivateProfileString("FONT","lfFaceName",LogFont->lfFaceName,fontsettingsfile);
}

static HFONT MyCreateFont( const char* fontstr )
{
    CHOOSEFONT cf;
    LOGFONT lf;
    HFONT hfont;      // Initialize members of the CHOOSEFONT structure.
    int req_font_size=atoi(fontstr); //Jeœli nie liczba to wychodzi i tak 0

    _TRACE(4) //if(trace_level & 4)
        fprintf(stderr,"Create my font %s.\n",fontstr);
    _TREND

    cf.lStructSize = sizeof(CHOOSEFONT);

    cf.hInstance = (HINSTANCE) WB_Instance;
    cf.hwndOwner = MyHwnd;
    cf.hDC = (HDC)NULL;

    cf.lpLogFont = &lf;
    cf.iPointSize = 0;
    cf.Flags = CF_SCREENFONTS;
    cf.rgbColors = RGB(0,0,0);
    cf.lCustData = 0L;
    cf.lpfnHook = (LPCFHOOKPROC)NULL;
    cf.lpTemplateName = (LPSTR)NULL;

    cf.lpszStyle = (LPSTR)NULL;
    cf.nFontType = SCREEN_FONTTYPE;
    cf.nSizeMin = 0;
    cf.nSizeMax = req_font_size;

    // Display the CHOOSEFONT common-dialog box.
    if(fontstr!=NULL && *fontstr=='!')
    {
        if( ReadFontSettings("./SYMSHELL.INI",cf.lpLogFont)==0) //Czytanie z domyœlnego pliku
            ChooseFont(&cf); //Awaryjnie, jeœli nie uda siê wczytaæ
    }
    else
    if(fontstr==NULL || *fontstr=='?' ||  strlen(fontstr)<3)
    {
        ChooseFont(&cf);
        WriteFontSettings("./SYMSHELL.INI",cf.lpLogFont);
    }
    else
    {
        if( ReadFontSettings(fontstr,cf.lpLogFont)==0)  //Czytanie z ustalonego pliku
            ChooseFont(&cf); //Awaryjnie, jeœli nie uda siê wczytaæ
    }

    // Create a logical font based on the user's
    // selection and return a handle identifying that font.
    hfont = CreateFontIndirect(cf.lpLogFont);
    return (hfont);
}

/** DC caching */
static HDC GetMyHdc(void)
{
    if(!UserFont && UserFontStr) //If NULL but need not be, then create
        {
        UserFont=MyCreateFont(UserFontStr);
        if(!UserFont)
            {
            MessageBoxA(MyHwnd,UserFontStr,"Font allocation failed",MB_ICONSTOP);
            return 0;
            }
        }

    if(MyHdc==0)
        {
        if(animate || is_buffered)
            {
            MyHdc=GetVirtualScreen();
            }
            else
            {
            MyHdc=GetRealScreen();
            }
        if(transparently)
            SetBkMode(MyHdc, TRANSPARENT);
        }
    return MyHdc;
}

static HBRUSH GetMyBrush(ssh_color color)
{
    assert(color<PALETE_LENGHT);

    if(brushes[color]==0) //Trzeba alokowaæ pêdzel
    {
        DeleteObject(brushes[color]); //???
        brushes[color]=CreateSolidBrush(colors[color]);
    }
    return curent_brush=brushes[color];
}

void set_title(const char* WindowName)
{
    assert(WindowName != 0);
    if (!SetWindowText(WB_Hwnd, WindowName)) // SetWindowText return TRUE on success!
    {
        _TRACE(0) //if(trace_level & 4)
            fprintf(stderr, "Cannot set window name into \"%s\"\n", WindowName);
        _TREND
    }
}

/* Ustala aktualny kolor wype³nieñ za pomoc¹ typu ssh_color */
void	set_brush(ssh_color c)
{
    HDC hdc=GetMyHdc();
    HBRUSH MyBrush=GetMyBrush(c);
    SelectObject(hdc,MyBrush);
    curr_fill=-1; //Funkcje same ustawiaj¹ce brush musz¹ to zrobiæ po u¿yciu set_brush
}

/* Aktualny kolor wype³nieñ jako `ssh_color`. */
ssh_color get_brush()
{
    return curr_fill;
}

/* Ustala aktualny kolor wype³nieñ za pomoc¹ sk³adowych RGB */
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    COLORREF MyColor = RGB(r,g,b);

    if(free_style_brush!=NULL)
        DeleteObject(free_style_brush);
    free_style_brush=curent_brush=CreateSolidBrush(MyColor);

    SelectObject(GetMyHdc(),free_style_brush);
    curr_fill=-1; //Funkcje same ustawiaj¹ce brush musz¹ to zrobiæ po u¿yciu set_brush
}

/* Ustala aktualny kolor wype³nieñ za pomoc¹ sk³adowych RGB */
void set_brush_rgba(ssh_intensity r, ssh_intensity g, ssh_intensity b, ssh_intensity a )
{
    COLORREF MyColor = RGB(r, g, b);
    //MyColor |= a << 24; //Teoretycznie, ale w Windows tak nie dzia³a
    if (free_style_brush != NULL)
        DeleteObject(free_style_brush);
    free_style_brush = curent_brush = CreateSolidBrush(MyColor);

    SelectObject(GetMyHdc(), free_style_brush);
    curr_fill = -1; //Funkcje same ustawiaj¹ce brush musz¹ to zrobiæ po u¿yciu set_brush
}

static HPEN GetMyPen(ssh_color color,int size,int style)
{
    assert(color<PALETE_LENGHT);

    if(size<=0)
        size=1; // co najmniej gruboœci 1

    if( pens[color].handle==0 ||
        pens[color].size!=size ||
        pens[color].style!=style) //Trzeba alokowaæ pêdzel, gdy zmienia siê rozmiar lub styl
    {
        if(pens[color].handle!=0)
        {
            DeleteObject(pens[color].handle); //Wymaga usuwania
            pens[color].handle=0;
        }

        _TRACE(4)
            fprintf(stderr,"Pen allocation c:%x s:%d style:%d.\n",color,size,style);
        _TREND

        {	//W£AŒCIWA ALOKACJA
            int sX=mulx*size;
            int sY=muly*size;
            int pom=sX<sY?sX:sY;
            int win_style=0;

            switch(style)
            {
            case SSH_LINE_SOLID :win_style=PS_SOLID;break;
            case SSH_LINE_DOTTED:win_style=PS_DOT;break;
            case SSH_LINE_DASHED:win_style=PS_DASH;break;
            }

            pens[color].handle=CreatePen(win_style,pom,colors[color]);
            pens[color].size=size;
            pens[color].style=style;
        }
    }
    else
    {
        assert(pens[color].handle!=0);
        assert(pens[color].size==size);
        assert(pens[color].style==style);
    }

    curent_pen_rgb=colors[color];
    return curent_pen=pens[color].handle;
}

/* Ustala aktualny kolor linii za pomoc¹ typu ssh_color */
void set_pen(ssh_color c, ssh_natural width, ssh_mode style)
{
    HDC hdc=GetMyHdc();

    curent_pen=GetMyPen(c,width,style);
    SelectObject(hdc,curent_pen);

    curr_color=-1; /* Funkcje same ustawiaj¹ce pen s¹ zmuszone to zrobiæ po u¿yciu set_pen */
    LineStyle=style;//???
    LineWidth=width;
}

/* Aktualny kolor linii jako ssh_color (indeks). */
ssh_color get_pen()
{
    return curr_color;
}

/* Ustala aktualny kolor linii za pomoc¹ sk³adowych RGB */
void set_pen_rgb(ssh_intensity r, ssh_intensity g, ssh_intensity b,
                    ssh_natural size, ssh_mode style)
{
    if(free_style_pen)
        DeleteObject(free_style_pen);

    _TRACE( 4)
            fprintf(stderr,"Free style pen allocation rgb:%x%x%x s:%d style:%d.\n",r,g,b,size,style);
    _TREND

    //Tworzenie nowego pióra
    {
        int sX=mulx*size;
        int sY=muly*size;
        int pom=sX<sY?sX:sY;

        int win_style=0;
        switch(style)
        {
        case SSH_LINE_SOLID :win_style=PS_SOLID;break;
        case SSH_LINE_DOTTED:win_style=PS_DOT;break;
        case SSH_LINE_DASHED:win_style=PS_DASH;break;
        }

        free_style_pen=curent_pen=CreatePen(win_style,pom,RGB(r,g,b));
    }

    //Od razu u¿ycie pióra
    SelectObject(GetMyHdc(),free_style_pen);

    curr_color=-1; /* Funkcje same ustawiaj¹ce pen sa zmuszone to zrobiæ po u¿yciu set_pen */
    LineStyle=style; //???
    LineWidth=size;
}

/* Ustala aktualny domyœlny kolor linii za pomoc¹ sk³adowych RGBA */
void set_pen_rgba(ssh_intensity r, ssh_intensity g, ssh_intensity b,
                  ssh_intensity a,                             /**< kana³ alfa */
                  ssh_natural size, ssh_mode style)
{
    if (free_style_pen)
        DeleteObject(free_style_pen);

    _TRACE(4)
        fprintf(stderr, "Free style pen allocation rgba:%x%x%x%x s:%d style:%d.\n", r, g, b,a, size, style);
    _TREND

    //Tworzenie nowego pióra
    {
        int sX = mulx * size;
        int sY = muly * size;
        int pom = sX < sY ? sX : sY;

        int win_style = 0;
        switch (style)
        {
        case SSH_LINE_SOLID:win_style = PS_SOLID; break;
        case SSH_LINE_DOTTED:win_style = PS_DOT; break;
        case SSH_LINE_DASHED:win_style = PS_DASH; break;
        }

        COLORREF MyColor = RGB(r, g, b);
        //MyColor |= a << 24; TO TAK NIE DZIA£A W WINDOWS
        free_style_pen = curent_pen = CreatePen(win_style,pom, MyColor);
    }

    //Od razu uzycie piora
    SelectObject(GetMyHdc(), free_style_pen);

    curr_color = -1;
    LineStyle = style; //???
    LineWidth = size;
}

/* Zmienia definicje koloru w palecie kolorów. Indeksy 0..PALETE_LENGHT */
void set_rgb(ssh_color color,                                  /* - indeks koloru */
             ssh_intensity r,ssh_intensity g,ssh_intensity b)  /* - wartoœci sk³adowych */
{
    if (color >= PALETE_LENGHT)
    {
        fprintf(stderr, "WARNING! Invalid index of color: %i \n", (int)color);
        color = PALETE_LENGHT-1;
    }

    colors[color]=RGB(r,g,b);

    if(brushes[color]!=0)
    {
        if(curr_fill==color)
        {
            curr_fill=-1; /* Wymusza wymianê pêdzla przy nastêpnym rysowaniu */
            if(MyHdc!=0)
                SelectObject(MyHdc,GetStockObject(NULL_BRUSH)); /*Wymiata z kontekstu */
        }
        DeleteObject(brushes[color]); /* Teraz mo¿na ju¿ zwolnic */
        brushes[color]=0;
    }

    if(pens[color].handle!=0)
    {
        if(curr_color==color)
        {
            curr_color=-1; /* Wymusza wymianê pióra przy nastêpnym rysowaniu */
            if(MyHdc!=0)
                SelectObject(MyHdc,GetStockObject(NULL_PEN)); /*Wymiata pióro z kontekstu */
        }

        DeleteObject(pens[color].handle); /* Teraz mo¿na ju¿ zwolnic */
        pens[color].handle=0;
        pens[color].size=0;
        pens[color].style=0;
    }
}

/* Jakie s¹ ustawienia RGB konkretnego kolorku w palecie */
ssh_rgb   get_rgb_from(ssh_color c)
{
    ssh_rgb pom;
    pom.r=GetRValue(colors[c]);
    pom.g=GetGValue(colors[c]);
    pom.b=GetBValue(colors[c]);
    return pom;
}

/* Zmiana definicje odcienia szaroœci w palecie szaroœci. Indeksy 256..511 */
void set_gray(ssh_color shade,ssh_intensity intensity)
{
    if( ! (255<shade && shade<PALETE_LENGHT) )
        fprintf(stderr, "WARNING! Invalid shade of gray: %i \n",(int)shade);
    set_rgb(shade,intensity,intensity,intensity);
}

/*
static HPEN Get1Pen(int color)
{
    if(pens[color]==0) //Trzeba alokowaæ pêdzel
        {
        pens[color]=CreatePen(PS_SOLID,1,colors[color]);
        }
    return pens[color];
}
*/


static void FreeResources(void)
{
    int k;
    curr_color=curr_fill=-1;

    _TRACE( 4 )
        fprintf(stderr," FREE RESOURCES.\n");
    _TREND

    if(MbHdc)		  //Zwalnia wirtualny kontekst, jeœli jest jakiœ
    {
        if(MbHdc==MyHdc)
            MyHdc=0;
        DeleteDC(MbHdc);
        MbHdc=0;
    }

    if(VirtualScreen!=0) //Zwalnia ekran wirtualny, jeœli jest
    {
        DeleteObject(VirtualScreen);
        VirtualScreen=0;
    }

    if(MyHdc!=0)		//Zwalnia kontekst okna, jeœli jest
    {
        int ret=ReleaseDC(MyHwnd,MyHdc);            //Czy to aby dzia³a?
                                                    assert(ret==1);
        MyHdc=0;
    }

    for(k=0;k<PALETE_LENGHT;k++) //Zwalnia wszystkie cache'owane pêdzle
    {
        if(pens[k].handle!=0)
        {
            DeleteObject(pens[k].handle);
            pens[k].handle=0;
            pens[k].size=0;
            pens[k].style=0;
        }

        if(brushes[k]!=0)
        {
            DeleteObject(brushes[k]);
            brushes[k]=0;
        }
    }
}

/*
   Return an RGB color value given a scalar v in the range [vmin,vmax].
   In this case, each color component ranges from 0 (no contribution) to
   1 (fully saturated), modifications for other ranges is trivial.
   The color is clipped at the end of the scales if v is outside
   the range [vmin,vmax]

   from http://paulbourke.net/texture_colour/colourspace/
*/

typedef struct {
      double r,g,b;
   } D_COLOUR;

D_COLOUR GetColour(double v,double vmin,double vmax)
{
   D_COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }

   return(c);
}


 /*
     * Computes the color gradiant
     * color: the output vector
     * x: the gradiant (beetween 0 and 360)
     * min and max: variation of the RGB channels (Move3D 0 -> 1)
     http://stackoverflow.com/questions/7139825/colormap-library-for-c-which-converts-a-given-value-into-red-green-and-blue-v
     */
 /* To siê bezpoœrednio nie nadaje bo ma 360 wartoœci!
void GroundColorMix(double* color, double x, double min, double max)
{
   // Red = 0 Green = 1 Blue = 2

    double posSlope = (max-min)/60;
    double negSlope = (min-max)/60;

    if( x < 60 )
    {
        color[0] = max;
        color[1] = posSlope*x+min;
        color[2] = min;
        return;
    }
    else if ( x < 120 )
    {
        color[0] = negSlope*x+2*max+min;
        color[1] = max;
        color[2] = min;
        return;
    }
    else if ( x < 180  )
    {
        color[0] = min;
        color[1] = max;
        color[2] = posSlope*x-2*max+min;
        return;
    }
    else if ( x < 240  )
    {
        color[0] = min;
        color[1] = negSlope*x+4*max+min;
        color[2] = max;
        return;
    }
    else if ( x < 300  )
    {
        color[0] = posSlope*x-4*max+min;
        color[1] = min;
        color[2] = max;
        return;
    }
    else
    {
        color[0] = max;
        color[1] = min;
        color[2] = negSlope*x+6*max;
        return;
    }
}

          double rgb[3];
          GroundColorMix(rgb,(double)k,(double)0,(double)255);
          set_rgb(k,rgb[0]*255,rgb[1]*255,rgb[2]*255);
*/

/* DO UWZGLÊDNIENIA TE¯
http://www.cs.uml.edu/~haim/ColorCenter/ColorCenterColormaps.htm
*/

/* Wewnêtrzna implementacja termicznej skali kolorów */
static void SetScale(void)
{
#ifndef M_PI
const double M_PI=3.141595;
#endif
//extern void set_rgb(ssh_color color,int r,int g,int b);

if(UseGrayScale) //U¿ywa skali szaroœci tam, gdzie normalnie s¹ kolory
  {
    int k;
    for(k=0;k<255;k++)
    {
        long wal=k;
        //fprintf(stderr,"%u %ul\n",k,wal);
        set_rgb(k,wal,wal,wal); //Color part
        set_rgb(256+k,wal,wal,wal); //Gray scale part
    }
    _TRACE( 4 )
        fprintf(stderr,"%s\n","SetScale (0-255 Gray) completed");
    _TREND
  }
  else
  {
  int k;
  for(k=0;k<255;k++)
    {      /* VERY_OLD_SCALE
           long wal1,wal2,wal3;
          double kat=(M_PI*2)*k/255.;
          wal1=(long)(255*sin(kat*1.25));

           if(wal1<0) wal1=0;
           wal2=(long)(255*(-sin(kat*0.85)));
           if(wal2<0) wal2=0;
           wal3=(long)(255*(-cos(kat*1.1)));
           if(wal3<0) wal3=0;
          */
#if OLD_COLOUR_SCALE != 1
          long wal1,wal2,wal3;
          double kat=(M_PI*2)*k/255.;

          //  LONG USED SCALE
          wal1=(long)(255*sin(kat*1.22));
          if(wal1<0) wal1=0;

          wal2=(long)(255*(-cos(kat*0.46)));
          if(wal2<0) wal2=0;

          wal3=(long)(255*(-cos(kat*0.9)));
          if(wal3<0) wal3=0;

          set_rgb(k,wal1,wal2,wal3);
/*
          wal1=(long)(255*sin(kat*1.2)*0.9);
          if(wal1<0) wal1=0;

          wal2=(long)(255*(-cos(kat*0.38+1.25)));
          if(wal2<0) wal2=0;

          wal3=(long)(255*(-cos(kat*0.9)*0.9));
          if(wal3<0) wal3=0;

          set_rgb(k,wal1,wal2,wal3);  */

/*
          wal1=(long)(255*sin(kat*1.2));
          if(wal1<0) wal1=0;

          wal2=(long)(255*(-cos(kat*0.38+1.25)));
          if(wal2<0) wal2=0;

          wal3=(long)(255*(-cos(kat*0.9)));
          if(wal3<0) wal3=0;

          set_rgb(k,wal1,wal2,wal3);
*/
#else

          D_COLOUR pom = GetColour( (double)k,(double)0,(double)255);
          set_rgb(k,pom.r*255,pom.g*255,pom.b*255);
#endif

      }
      //POWY¯EJ 255 - I TAK SKALA SZAROŒCI:
      {
      unsigned k;
      for(k=256;k<PALETE_LENGHT; k++)
            set_rgb(k,(unsigned char)k,(unsigned char)k,(unsigned char)k );

     _TRACE( 4)
        fprintf(stderr,"SetScale (Colors: 0-255; Gray: 256--> %d) completed",PALETE_LENGHT);
      _TREND
      }
  }
    set_rgb(0,0,0,0);
    set_rgb(255,255,255,255);
}



static int InitWindowClass(void)
{
    // Other instances of app running?
    if (!WB_PrevInstance)
        {
            // Initialize shared things
            _TRACE( 4)
                fprintf(stderr,"%s\n","Init Window Class");
            _TREND

            if (!InitApplication(WB_Instance))
            {
                _TRACE( 4)
                    fprintf(stderr,"%s\n","  FAILED!!!");
                _TREND
                return FALSE;               // Exits if unable to initialize
            }
        }
    return TRUE;
}

static int InitMainWindow(void)
{
    // Perform initializations that apply to a specific instance
    _TRACE( 4)
        fprintf(stderr,"%s\n","Init Main Window");
    _TREND

    if(!InitInstance(WB_Instance))
        {
            return FALSE;
        }
    if(!NoResources)//Jesli sa zasoby to staramy sie zaladowac akcelerator
        hAccelTable = LoadAccelerators(WB_Instance, szAppName);
    return TRUE;
}


/* CONFIGURATION AND MENAGE SHELL - USE IN THIS ORDER!   */
/* ***************************************************** */

/* Ustala czy rozmiar okna mo¿e byæ p³ynnie zmieniany */
void fix_size(int yes)
{
    Flexible=!yes;
}

/* Czy okno jest zafiksowanej wielkoœci */
int fixed()
{
    return !Flexible;
}

/* Ustala szerokoœæ linij - rysowanie grubych mo¿e byc kosztowne. Zwraca stan poprzedni WHAT ABOUT 0?*/
ssh_natural line_width(ssh_natural width)
{
    unsigned pom=LineWidth;
    LineWidth=width;
    curr_color=-1; //Wymusza realokacj¹ jeœli zostanie u¿yty aktualny kolor
    if(MyHdc)
        SelectObject(MyHdc,GetStockObject(NULL_PEN)); /*Wymiata pen z kontekstu */
    return pom;
}

ssh_natural get_line_width()
{
    return LineWidth;
}

int line_style(int S)
{
    int pom=LineStyle;
    LineStyle=S;
    curr_color=-1; //Wymusza realokacjê, jeœli zostanie u¿yty aktualny kolor
    if(MyHdc)
        SelectObject(MyHdc,GetStockObject(NULL_PEN));/*Wymiata z kontekstu */
    return pom;
}

// Zmiana stylu nak³adania grafiki: SSH_SOLID_PUT albo SSH_XOR_PUT
int		put_style(int S)
{
    HDC hdc=GetMyHdc();
    int old=GetROP2(hdc);

    if(S==SSH_SOLID_PUT)
    {
        if(old!=R2_COPYPEN)
            SetROP2(hdc,R2_COPYPEN);
    }
    else
    if(S==SSH_XOR_PUT)
    {
        if(old!=R2_XORPEN)
            SetROP2(hdc,R2_XORPEN);
    }
    return old;
}

/* Ustala czy mysz ma byæ obs³ugiwana. Zwraca poprzedni stan flagi. */
int  mouse_activity(int yes)
{
    int pom=is_mouse;
    is_mouse=yes;
    if(MyHwnd!=0)
    {
        if(is_mouse)
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        else
            SetCursor(LoadCursor(NULL, IDC_NO));
    }
    return pom;
}

/* Ustala index koloru do czyszczenia itp */
void set_background(ssh_color color)
{
    assert(color<PALETE_LENGHT);
    Background=color;
}

ssh_color background()
{
    return Background;
}

/* Prze³¹czanie buforowanie okna */
void buffering_setup(int _n)
{
    if(_n)
        animate=1;
    else
        animate=0;
    if(animate)	/* Musi byc w³¹czona bitmapa buforuj¹ca */
        is_buffered=1; /* ¯eby mo¿na by³o na niej pisaæ */
}

int buffered()
{
    return is_buffered;
}

/* GETTING SCREEN PARAMETERS */
/*---------------------------*/

/* ostateczne rozmiary okna po przeliczeniach */
ssh_natural  screen_height()
{
    return W_height;
}

/* do pozycjonowania na bocznych marginesach */
ssh_natural  screen_width()
{
    return W_width;
}

/* Aktualne rozmiary znaku  */
int  raw_char_height()
{
    if(!Is_finfo)
        GetMyHdc(); //Wymusza przeczytanie informacji o foncie
    return  font_info.tmHeight;
}

/* potrzebne do pozycjonowania tekstu */
int  raw_char_width()
{
    if(!Is_finfo)
        GetMyHdc(); //Wymusza przeczytanie informacji o foncie
    return  font_info.tmMaxCharWidth;
}

/* Aktualne rozmiary znaku  */
ssh_natural  char_height(char znak)

{
    int pom=raw_char_height();
    pom/=muly;
    if(pom%muly>0) pom++;
    return pom;
}

/* potrzebne do pozycjonowania tekstu */
ssh_natural  char_width(char znak)
{
    int pom=raw_char_width();
    pom/=mulx;
    if(pom%mulx>0) pom++;
    return pom;
}

/* Aktualne rozmiary ³añcucha znaków */
ssh_natural  string_height(const char* str)
{
    SIZE sizes;
    if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
            return sizes.cy/muly;
            else
            return -1;
}

/* ...potrzebne do jego pozycjonowania */
ssh_natural  string_width(const char* str)
{
    SIZE sizes;
    if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
            return sizes.cx/mulx;
            else
            return -1;
}

/** ¯eby mo¿na by³o swobodnie manipulowaæ sposobem wywo³ania innych funkcji */
static void __cdecl special_close_plot(void)
{
    close_plot();
}

/* inicjacja grafiki/semigrafiki */
int  init_plot(ssh_natural  a,ssh_natural   b,                 /* ile pikseli mam mieæ okno */
               ssh_natural ca, ssh_natural cb)                 /* ile dodatkowo linij i kolumn tekstu na dole i po bokach przy domyœlnej czcionce */
{
    int ScreenRealH=0,ScreenRealW=0;
    RECT rect;
    if(WB_Instance==0)
                WB_Instance=GetModuleHandle(NULL);

    ini_width=a;  // MAXX+1 obszaru pikselowego
    ini_height=b; // MAXY+1 obszaru pikselowego
    ini_col=ca;   // Ile kolumn tekstu dodatkowo
    ini_row=cb;   // Ile wierszy tekstu dodatkowo

    SetScale(); //SKALA KOLORÓW.

    W_width=a;W_height=b; // ¯eby cos by³o na pocz¹tek

    if(!InitWindowClass()||
        !InitMainWindow())
        {
        assert("Window initialisation"==NULL);
        return /*FALSE*/0;
        }

    atexit(special_close_plot);
    //ShowWindow(MyHwnd, SW_SHOW); // DEBUG

    // Resize
    GetRealScreen(); //Zeby zaladowac niezbedna informacje o czcionce
    ScreenRealW=GetDeviceCaps(MyHdc,HORZRES);
    ScreenRealH=GetDeviceCaps(MyHdc,VERTRES);

    W_width=a+ca*(char_width)('X');    //by³o  _char_width - mo¿e z powodu makra które by³o kiedyœ?
    W_height=b+cb*(char_height)('Y');  // i analogicznie

    if(ForceHeight!=0)                 //Test na rozmiary okna wymuszone parametrami wywo³ania
        W_height=ForceHeight;
    if(ForceWidth!=0)
        W_width=ForceWidth;

    rect.left=rect.top=0;
    rect.right=W_width;
    rect.bottom=W_height;

    //Trzeba policzyæ obszar na menu jeœli jest takowe
    {
        HMENU MyMenu=GetMenu(MyHwnd);
        AdjustWindowRect(&rect,MY_WIN_STYLE,MyMenu!=NULL);
    }

    //Teraz trzeba uaktualniæ rozmiary okna
    {
    char buf[128];
    //Liczymy rozmiary ze wzglêdu na zadany obszar roboczy oraz ewentualne menu
    int real_W_height=abs(rect.top-rect.bottom);
    int real_W_width=abs(rect.left-rect.right);
    //Jednak nie mo¿e byæ wiêksze ni¿ rozmiar ekranu!
    //if(ForceHeight==0) //I tak nie mo¿na powiêkszyæ poza ekran — takie okno nie powstaje...
    if(real_W_height>ScreenRealH)
            real_W_height=ScreenRealH;
    //if(ForceWidth==0)
    if(real_W_width>ScreenRealW)
            real_W_width=ScreenRealW;

    //Ustawiamy wg tego, co obliczy³o AdjustWindowRect()
    SetWindowPos(  MyHwnd,
                        HWND_TOP,0,0,
                        real_W_width,real_W_height,
                        SWP_NOMOVE | SWP_NOCOPYBITS);


    //O ile okno jest mniejsze od ekranu
    if(real_W_height<=ScreenRealH && real_W_width<=ScreenRealW)
    {
        int cur_Cl_height,cur_Cl_width;
        //Sprawdzenie, czy siê uda³o, bo poprzednia funkcja nie uwzglêdnia zawijania menu
        GetClientRect(MyHwnd,&rect);
        cur_Cl_height=abs(rect.top-rect.bottom);
        cur_Cl_width=abs(rect.left-rect.right);

        if(cur_Cl_height<W_height || cur_Cl_width<W_width)
        {
            real_W_height+=W_height-cur_Cl_height;
            real_W_width+=W_width-cur_Cl_width;
            //I jeszcze raz ustawiamy
            SetWindowPos(  MyHwnd,
                        HWND_TOP,0,0,
                        real_W_width,real_W_height,
                        SWP_NOMOVE | SWP_NOCOPYBITS);
        }
    }

    //Zapisujemy to, co wysz³o!
    WritePrivateProfileString("WINDOW","Height",_ltoa(real_W_height,buf,10),"./SYMSHELL.INI");
    WritePrivateProfileString("WINDOW","Width",_ltoa(real_W_width,buf,10),"./SYMSHELL.INI");
    }


    // Make the window visible; update its client area
    ShowWindow(MyHwnd, SW_SHOW); // Show the window
    WindowClosed=0;// Window open flag for close_plot
    //if(!is_mouse)
    //      SetCursor(LoadCursor(NULL, IDC_NO));
    FreeResources(); //Bo mog¹ siê za³adowaæ konteksty dla z³ego ekranu (pod WINDOWS?)

    PostMessage(MyHwnd,WM_PAINT,0,0); // Sends WM_PAINT message
    if(is_mouse)
          SetCursor(LoadCursor(NULL, IDC_ARROW));
          else
          SetCursor(LoadCursor(NULL, IDC_NO));

    return /*TRUE*/1;
}

/* uzgodnienie zawartoœci ekranu  */
void flush_plot()
{
    _TRACE( 2 )
        fprintf(stderr," FLUSH PLOT ");
    _TREND

    if(MyHdc!=0) //Byl u¿ywany. Zwalnia tylko RelaseResources
    {
        if(animate && VirtualScreen && MyHdc==MbHdc )//Animujemy i jest juz bitmapa
        {
            HDC ScreenDc=GetDC(MyHwnd);
            _TRACE(2)
                fprintf(stderr,"-> BITBLT ");
            _TREND
            BitBlt(ScreenDc,0,0,W_width*mulx,W_height*muly,MyHdc,0,0,SRCCOPY);
            ReleaseDC(MyHwnd,ScreenDc);
        }
        _TRACE( 2)
            fprintf(stderr,"-> RELEASE DC ");
        _TREND
        ReleaseDC(MyHwnd,MyHdc);
        curr_color=curr_fill=-1;
        MyHdc=0;//!!!
    }

    if(DelayTime>0)
        Sleep(DelayTime); //Uœpienie programu na pewien czas

    _TRACE(2)
        fprintf(stderr," * \n");
    _TREND
}

/* Wymuszenie oczekiwania przez pewn¹ liczbê ms */
void	delay_ms(unsigned ms)
{
    Sleep(ms); //Uœpiene programu na pewien czas
}

/* GETTING INPUT */

//zmienne do komunikacji ProcesMsg z procedurami obs³ugi komunikatów
//------------------------------------------------------------------
static int InputChar=0;
static int InputXpos;
static int InputYpos;
static int InputClick;
static int MouseInput=0; //Flaga sygnalizuj¹ca, ¿e s¹ nieodczytane dane myszowe

/// Przerabia wejœcie i zwraca znak.
/// EOF to jest koniec, 0 jeœli nic nie by³o do wziêcia lub jakiœ inny.
static int ProcessMsg(int peek)
{
    BOOL msg_avail=0;
    int ret;

    _TRACE(1)
        fprintf(stderr,"* PrcsMsg(p=%d) ",peek);
    _TREND

    InputChar='\0';
    // Acquire and dispatch messages until a WM_QUIT a message is received.
JESZCZE_RAZ:
    if(peek)
    {
        _TRACE( 1)
            fprintf(stderr,"[");
        _TREND

            msg_avail=PeekMessage(&msg,/*NULL*/MyHwnd,0,0,PM_REMOVE);

        _TRACE( 1)
            fprintf(stderr,"%d]",msg_avail);
        _TREND
    }
    else
    {
        _TRACE(1)
            fprintf(stderr,"{");
        _TREND

            msg_avail=GetMessage(&msg,/*NULL*/MyHwnd, 0, 0);

        _TRACE( 1)
            fprintf(stderr,"%d}",msg_avail);
        _TREND

            if(msg_avail==FALSE && msg.hwnd==MyHwnd)
            {
                _TRACE( 1)
                    fprintf(stderr,"msg_avail=%d!",msg_avail);
                _TREND

                WindowClosed=1; // Window destroyed?
                InputChar='\0';
                return EOF;
            }
    }

    if(msg_avail==-1) /* ??? BOOL i -1 ???? */
    {
        _TRACE( 1)
            fprintf(stderr,"msg_avail=%d",msg_avail);
        _TREND

            if(error_proc()=='A')
                abort();
    }

    if(msg.hwnd!=MyHwnd)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        _TRACE( 1)
            fprintf(stderr,"* PrcsMsg GOTO \"ONES MORE\" ");
        _TREND

            goto JESZCZE_RAZ;         //!!!!!!!!!!!!!!!!!!!!!
    }

    if(msg_avail==TRUE)
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ret=InputChar;
        InputChar='\0';
        return ret;
}

LOCAL 
int first_to_read=0;

/* (nie)zale¿na od platformy funkcja sprawdzaj¹ca, czy jest wejœcie */
int  input_ready()
{
    int input;
    _TRACE(1)
        fprintf(stderr,"* input_ready() ");
    _TREND

    if(CharToGet!=0||first_to_read!=0) //Nieodebrano
        return TRUE;

    input=ProcessMsg(1); //PEEK! Trzeba sprawdziæ nowe komunikaty

    if(input!='\0')
    {CharToGet=input;return TRUE;}
    else
    {CharToGet='\0';return FALSE;}
}

/* Odes³anie znaku na wejœcie — zwraca 0, jeœli nie ma miejsca */
int  set_char(int c)
{
    _TRACE( 1)
        fprintf(stderr,"* set_char(%c) ",c);
    _TREND

    if(first_to_read!=0) //Jeszcze nieodebrane
        return 0;

    first_to_read=c;
    return 1;
}

/* odczytywanie znaków sterowania, z oczekiwaniem, jeœli brak */
int  get_char()
{
    int input;

    _TRACE(1)
        fprintf(stderr,"* get_char() ");
    _TREND
    
    if(first_to_read!=0) //Sprawdza, czy nie ma zwrotów
    {
        input=first_to_read;
        first_to_read=0;
        goto READY_TO_USE;//return input;
    }

    if(CharToGet!=0) //Zwraca znak odczytany przez input_ready()
    {
        input=CharToGet;
        CharToGet='\0';
        goto READY_TO_USE;//return input;
    }

    // TODO - WSTAWIÆ Delay ? w aktywn¹ petle?
    while((input=ProcessMsg(0))=='\0'); //sam musi poczekaæ na znak

READY_TO_USE:
    _TRACE(1)
        fprintf(stderr,"---> %u %c ",input,input);
    _TREND

    return input;
}

/* Odczytuje ostatnie zdarzenie myszy */
int  get_mouse_event(int* xpos,int* ypos,int* click)
{
    _TRACE(1)
        fprintf(stderr,"* get_mouse_event() ");
    _TREND

    if(MouseInput!=0)
    {
        *xpos=InputXpos/mulx;
        *ypos=InputYpos/muly;
        *click=InputClick;
        MouseInput=0;
        return 1;
    }
    return 0;
}

//Finishing graphics
//******************

/* zamkniecie grafiki/semigrafiki */
void close_plot(void)
{
    int ret=0; //Dla GetMessage

    _TRACE( 2)
        fprintf(stderr," CLOSE PLOT ");
    _TREND

    if(!WindowClosed) // Jeszcze nie, zamkniête wczeœniej & Valid HANDLE?
        {
        WindowClosed=1; //Zabezpieczenie przed powtórnym wywo³aniem tej funkcji

        if(WB_error_enter_before_clean)
            {
            char* kom="Press OK to close the window";
            /* width,height of Window at this moment */
            //print(ini_width/2-(strlen(kom)*char_width('X'))/2,ini_height/2,kom);
            //      FILE
            //fprintf(stderr,"(See at window: %s )",window_name);
            if(IsWindow(MyHwnd)!=0) //IsWindow
                {
                if(MessageBoxA(MyHwnd,kom,window_name,MB_ICONQUESTION)==0)
                    MessageBoxA(NULL,"Your window is probably closed",window_name,MB_ICONQUESTION);;
                }
                else
                MessageBoxA(NULL,"Your window already closed",window_name,MB_ICONQUESTION);
            //get_char();
            WB_error_enter_before_clean=0;
            }

        FreeResources();
        if(UserFont)
            { //Alokowane raz na program
                DeleteObject(UserFont);
                UserFont=0;
            }

        if(IsWindow(MyHwnd)!=0) //Jeœli IsWindow to trzeba je zamkn¹æ
            {
            DestroyWindow(MyHwnd);
            // Message loop for ending meesages - should be?(WB)
            while((ret=GetMessage(&msg,MyHwnd, 0, 0))!=0)     //Zamiast MyHwnd by³o NULL...
                {
                    if (ret == -1)
                        {
                            // handle the error and possibly exit
                            //fprintf(stderr,"GetMessage return -1");
                            return; //Abort this function
                        }
                    else
                    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                        {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                }
            }
        }
}



/*  PRINTING */
/* ********** */
static char straznik1=0x77;
static char bufor[2048];
static char straznik2=0x77;
static int ox,oy;

/* ---//--- wyprowadzenie tekstu na ekran */
void printbw(int x,int y,const char* format,...)       // int x,int y,const char* format,...
{
    static double MyHdc;
    HDC LocalMyHdc;
    RECT rc;
    size_t len; //Bêdzie przypisane d³ugoœci¹ bufora

    unsigned font_height=0;
    unsigned font_width=0;

   va_list argptr;

   va_start(argptr, format);

   vsprintf(bufor, format, argptr);

   va_end(argptr);

   if(straznik1!=0x77 || straznik2!=0x77)
        {
        fprintf(stderr,"symshell.print(...) - line exced 2048b!");
        abort();
        }

  /* Print string in a window */
  /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & height */
    LocalMyHdc=GetMyHdc();
    font_height = font_info.tmHeight;
    font_width = font_info.tmAveCharWidth;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */

//    ox=x;oy=y;
    rc.left=x;rc.top=y;
    rc.right=x+font_width*(len+1);
    rc.bottom=y+font_height;
    //FillRect (MyHdc, &rc, GetMyBrush(255));

    SetTextColor(LocalMyHdc,colors[0]);
    SetBkColor(LocalMyHdc,colors[255]);

//if(!animate)
    TextOut(LocalMyHdc,x,y,bufor,len);
//if(is_buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}

void print_rgb( int x,int y,
                unsigned r,unsigned g,unsigned b,
                ssh_color back,
                const char* format,...)
{
    static double MyHdc;
    HDC LocalMyHdc;
    //RECT rc;
    size_t len; //Bêdzie przypisane d³ugoœci bufora
    //unsigned font_height=0;
    //unsigned font_width=0;

   va_list argptr;

   va_start(argptr, format);

   vsprintf(bufor, format, argptr);

   va_end(argptr);

   if(straznik1!=0x77 || straznik2!=0x77)
        {
        fprintf(stderr,"FATAL: symshell.print(...) - bufor exced 1024b!");
        abort();
        }

  /* Need length for FillRect*/
    len = strlen(bufor);

    /* Get string widths & hight */
                assert(back<PALETE_LENGHT);
    LocalMyHdc=GetMyHdc();
    //font_height = font_info.tmHeight;
    //font_width = font_info.tmAveCharWidth;
    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

    /* Output text, centered on each line */
    //rc.left=x;
    //rc.top=y;
    //rc.right=x+font_width*(len+1);
    //rc.bottom=y+font_height;
    //FillRect (LocalMyHdc, &rc, GetMyBrush(back));

    SetTextColor(LocalMyHdc,RGB(r,g,b));
    SetBkColor(LocalMyHdc,colors[back]);
//if(!animate) ???
    TextOut(LocalMyHdc,x,y,bufor,len); // SetBkMode(hdc, TRANSPARENT)???
//if(is_buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}

void printc(    int x,int y,
                ssh_color fore,ssh_color back,
                const char* format,...)
{
    static double MyHdc;
    HDC LocalMyHdc;
    //RECT rc;
    size_t len; //Bêdzie przypisane d³ugoœci bufora
    //unsigned font_height=0;
    //unsigned font_width=0;

   va_list argptr;

   va_start(argptr, format);

   vsprintf(bufor, format, argptr);

   va_end(argptr);

   if(straznik1!=0x77 || straznik2!=0x77)
        {
        fprintf(stderr,"FATAL: symshell.print(...) - bufor exced 1024b!");
        abort();
        }

  /* Need length for FillRect*/
    len = strlen(bufor);

    /* Get string widths & height */
                assert(fore<PALETE_LENGHT);
                assert(back<PALETE_LENGHT);
    LocalMyHdc=GetMyHdc();
    //font_height = font_info.tmHeight;
    //font_width = font_info.tmAveCharWidth;
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */

    /* Output text, centered on each line */
    //rc.left=x;
    //rc.top=y;
    //rc.right=x+font_width*(len+1);
    //rc.bottom=y+font_height;
    //FillRect (LocalMyHdc, &rc, GetMyBrush(back));

    SetTextColor(LocalMyHdc,colors[fore]);
    SetBkColor(LocalMyHdc,colors[back]);
//if(!animate) ???
    TextOut(LocalMyHdc,x,y,bufor,len); // SetBkMode(hdc, TRANSPARENT)???
//if(is_buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}

/* W³¹cza mo¿liwoœæ drukowania tekstu bez zamazywania t³a. Zwraca stan poprzedni */
int  print_transparently(int yes)
{
    int old=transparently;
    MyHdc=GetMyHdc();
    transparently=yes;
    if(yes)
        SetBkMode(MyHdc, TRANSPARENT);
    else
        SetBkMode(MyHdc, OPAQUE );
    return old;
}

/* DRAWING  */
/* *******  */

/* wyœwietlenie punktu na ekranie */
void plot(int x,int y,ssh_color color)
{
    //	if(color>=PALETE_LENGHT) //DEBUG
    assert(color<PALETE_LENGHT);
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */
    if(mulx>1 || muly>1)
       {
       RECT rc;
       rc.left=x;rc.top=y;
       rc.right=x+mulx;
       rc.bottom=y+muly;
        FillRect (GetMyHdc(), &rc, GetMyBrush(color));
       }
       else
       {
        SetPixelV(GetMyHdc(),x,y,colors[color]);
       }
}

/* wyœwietlenie punktu na ekranie */
void plot_rgb(ssh_coordinate x,ssh_coordinate y,                       /* Wspó³rzêdne */
              ssh_intensity r,ssh_intensity g,ssh_intensity b)         /*- sk³adowe koloru */
{
    COLORREF color;
    color=RGB(r,g,b);

    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */
    if(mulx>1 || muly>1)
       {
       RECT rc;
       HBRUSH  hbrush;
       rc.left=x;rc.top=y;
       rc.right=x+mulx;
       rc.bottom=y+muly;
       hbrush=CreateSolidBrush(color);
       FillRect (GetMyHdc(), &rc, hbrush);
       DeleteObject(hbrush);
       }
       else
       {
        SetPixelV(GetMyHdc(),x,y,color);
       }
}

static
void _fill_seed(HDC hdc,int x,int y,COLORREF R_fill,COLORREF R_border)
{
    COLORREF what;
    what=GetPixel(hdc,x,y);
    if(what==R_border || what==CLR_INVALID || what==R_fill)
            return; //granica albo poza dozwolonym obszarem ekranu
    SetPixelV(hdc,x,y,R_fill);
    //Seeding
    _fill_seed(hdc,x+1,y,R_fill,R_border);
    _fill_seed(hdc,x-1,y,R_fill,R_border);
    _fill_seed(hdc,x,y+1,R_fill,R_border);
    _fill_seed(hdc,x,y-1,R_fill,R_border);
}

//Wype³nia powodziowo lub algorytmem siania
void fill_flood(int x,int y,ssh_color fill,ssh_color border)
{
    COLORREF R_fill,R_border;

                    assert(fill<PALETE_LENGHT);
                    assert(border<PALETE_LENGHT);

    R_fill=colors[fill];
    R_border=colors[border];
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */
    _fill_seed(GetMyHdc(),x,y,R_fill,R_border);
}

//Wype³nia powodziowo lub algorytmem siania
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,
                ssh_intensity rf,ssh_intensity gf,ssh_intensity bf,    /*- kolor wype³nienia podany sk³adowymi */
                ssh_intensity rb,ssh_intensity gb,ssh_intensity bb)    /*- kolor brzegu podany sk³adowymi */
{
    COLORREF R_fill,R_border;

    R_fill=RGB(rf,gf,gf);
    R_border=RGB(rb,gb,gb);
    x*=mulx; /* Multiplication of coordinates */
    y*=muly; /* if the window is bigger */
    _fill_seed(GetMyHdc(),x,y,R_fill,R_border);
}

void fill_rect(int x1,int y1,int x2,int y2,ssh_color color)
{
    RECT rc;                                                    //assert(color<PALETE_LENGHT);
    if(color>=PALETE_LENGHT) //Postêpowanie awaryjne
    {
        color=PALETE_LENGHT-1;
    }

    x1*=mulx;x2*=mulx; /* Multiplication of coordinates */
    y1*=muly;y2*=muly;  /* if the window is bigger */
    rc.left=x1;rc.top=y1;
    rc.right=x2;
    rc.bottom=y2;
    FillRect (GetMyHdc(), &rc, GetMyBrush(color)); /* Bez dolnego wiersza i prawej kolumny!!! */
}

/* Wyœwietla prostok¹t w kolorach domyœlnych*/
void fill_rect_d(int x1,int y1,int x2,int y2)
{
    RECT rc;
    x1*=mulx;x2*=mulx; /* Multiplication of coordinates */
    y1*=muly;y2*=muly;  /* if a window is bigger */
    rc.left=x1;rc.top=y1;
    rc.right=x2;
    rc.bottom=y2;
    FillRect (GetMyHdc(), &rc,curent_brush); /* Bez dolnego wiersza i prawej kolumny!!! */
}

/* wyœwietlenie jednej linii */
void line(int x1,int y1,int x2,int y2,ssh_color color)
{
    POINT points[2];                                           assert(color<PALETE_LENGHT);

    plot(x2,y2,color);
    plot(x1,y1,color);

    x1*=mulx;x2*=mulx; /* Multiplication of coordinates */
    y1*=muly;y2*=muly;  /* if a window is bigger */

    if(curr_color!=color /*???*/)
    {
        HPEN MyPen=GetMyPen(color,LineWidth,LineStyle);
        SelectObject(GetMyHdc(),MyPen);
    }

    points[0].x=x1;points[0].y=y1;
    points[1].x=x2;points[1].y=y2;
    Polyline(GetMyHdc(),points,2);
}

/* Wyœwietlenie linii w kolorze domyœlnym */
void line_d(int x1,int y1,int x2,int y2)
{
    POINT points[2];

    x1*=mulx;x2*=mulx; /* Multiplication of coordinates */
    y1*=muly;y2*=muly;  /* if a window is bigger */

    points[0].x=x1;points[0].y=y1;
    points[1].x=x2;points[1].y=y2;
    Polyline(GetMyHdc(),points,2);

    // Chyba niepotrzebne:
    // plot_rgb(x2,y2,GetRValue(curent_pen_rgb),GetGValue(curent_pen_rgb),GetBValue(curent_pen_rgb));
    // plot_rgb(x1,y1,GetRValue(curent_pen_rgb),GetGValue(curent_pen_rgb),GetBValue(curent_pen_rgb));
}

/* Wyœwietlenie okrêgu w kolorze domyœlnym */
void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
{
    int r1,r2;

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=r*mulx;r2=r*muly;  /* if a window is bigger */

    Arc(GetMyHdc(),x-r1,y-r2,x+r1,y+r2,x,y+r1,x,y+r1);
}

/* Wyœwietlenie okrêgu w kolorze c --> color_index */
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color color_index)
{
    int r1,r2;      assert(color_index<PALETE_LENGHT);

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=r*mulx;r2=r*muly;  /* if a window is bigger */
    if(curr_color!=color_index /*???*/)
    {
        HPEN MyPen=GetMyPen(color_index,LineWidth,LineStyle);
        SelectObject(GetMyHdc(),MyPen);
    }
    Arc(GetMyHdc(),x-r1,y-r2,x+r1,y+r2,x,y+r1,x,y+r1);
}

/* Wype³nienie kola w kolorze c */
void fill_circle(ssh_coordinate x,ssh_coordinate y,
                 ssh_natural r,
                 ssh_color color_index)
{
    int r1,r2;      assert(color_index<PALETE_LENGHT);
                    if(color_index>=PALETE_LENGHT) color_index=PALETE_LENGHT-1;

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=r*mulx;r2=r*muly;  /* if a window is bigger */
    if(curr_color!=color_index /*???*/)
    {
        HPEN MyPen=GetMyPen(color_index,1,SSH_LINE_SOLID);
        SelectObject(GetMyHdc(),MyPen);
    }
    if(curr_fill!=color_index)
    {
        HBRUSH MyBrush=GetMyBrush(color_index);
        SelectObject(GetMyHdc(),MyBrush);
    }
    Ellipse(GetMyHdc(),x-r1,y-r2,x+r1,y+r2);
}

/* Wype³nienie kola o promieniu r w kolorach domyœlnych*/
void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
{
    int r1,r2;

    x*=mulx;y*=muly;   /* Multiplication of coordinates */
    r1=r*mulx;r2=r*muly;  /* if a window is bigger */

    Ellipse(GetMyHdc(),x-r1,y-r2,x+r1,y+r2);
}

/* Wype³nia wielok¹t przesuniêty o vx, vy w kolorze c */
void fill_poly(int vx,int vy,
                    const ssh_point points[],int number,
                    ssh_color color)
{
    static POINT _LocalTable[10];
    POINT* LocalPoints=_LocalTable;
    int i;

    if(number<=2)
            return; //Nie da sie rysowaæ wielok¹ta o dwu punktach lub mniej

    if(number>10) //Jest za du¿y. Alokacja
        LocalPoints=(POINT*)calloc(number,sizeof(POINT));

    if(LocalPoints==NULL)
             {assert("Memory for polygon");return;}

    if(curr_color!=color /*???*/)
        {
        HPEN MyPen=GetMyPen(color,1,SSH_LINE_SOLID);
        SelectObject(GetMyHdc(),MyPen);
        }

    if(curr_fill!=color)
        {
        HBRUSH MyBrush=GetMyBrush(color);
        SelectObject(GetMyHdc(),MyBrush);
        }

    vx*=mulx;
    vy*=muly;
    for(i=0;i<number;i++)
        {
        LocalPoints[i].x=points[i].x*mulx+vx;
        LocalPoints[i].y=points[i].y*muly+vy;
        }

    Polygon(GetMyHdc(),LocalPoints,number);

    if(number>10) //Gdy by³ du¿y, to mamy pamiêæ do zwolnienia.
        free(LocalPoints);
}


/* Wype³nia wielok¹t przesuniêty o vx, vy w kolorach domyœlnych */
void fill_poly_d(int vx,int vy, const ssh_point points[],int number)
{
    static POINT _LocalTable[10];
    POINT* LocalPoints=_LocalTable;
    int i;

    if(number<=2)
            return; //Nie da siê rysowaæ wielok¹ta o dwu punktach lub mniej

    if(number>10) //Jest za du¿y. Alokacja pamiêci na punkty.
        LocalPoints=calloc(number,sizeof(POINT));

    if(LocalPoints==NULL)
             {assert("Memory for polygon");return;}

    vx*=mulx;
    vy*=muly;
    for(i=0;i<number;i++)
        {
        LocalPoints[i].x=points[i].x*mulx+vx;
        LocalPoints[i].y=points[i].y*muly+vy;
        }

    Polygon(GetMyHdc(),LocalPoints,number);

    if(number>10) //Byl du¿y
        free(LocalPoints);
}

/* Czyœci ekran przed zmian¹ zawartoœci. Kosztowne... */
void clear_screen()
{
    RECT rc;
    _TRACE( 2)
        fprintf(stderr,"CLEAR SCREEN\n");
    _TREND
    GetClientRect(MyHwnd, &rc);
    rc.right++;  //Niweluje dziwny sposób wype³niania FillRect
    rc.bottom++; //pomijaj¹cy dolny wiersz i skrajnie praw¹ kolumnê
    FillRect (GetMyHdc(), &rc, GetMyBrush(Background));
}


// MESSAGE SUPPORT COPIED FROM BORLAND SAMPLE
LOCAL
LRESULT DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM);

//
//  FUNCTION: DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Call the function associated with a message.
//
//  PARAMETERS:
//    lpmsdi - Structure containing the message dispatch information.
//    hwnd - The window handle
//    uMessage - The message number
//    wparam - Message specific data
//    lparam - Message specific data
//
//  RETURN VALUE:
//    The value returned by the message function that was called.
//
//  COMMENTS:
//    Runs the table of messages stored in lpmsdi->rgmsd searching
//    for a message number that matches uMessage.  If a match is found,
//    call the associated function.  Otherwise, call DispDefault to
//    call the default function, if any, associated with the message
//    structure.  In either case, return the value received from the
//    message or default function.
//
LOCAL 
LRESULT DispMessage(LPMSDI lpmsdi,
                    HWND   hwnd,
                    UINT   uMessage,
                    WPARAM wparam,
                    LPARAM lparam)
{
    int  imsd;

    MSD *rgmsd = lpmsdi->rgmsd;
    int  cmsd  = lpmsdi->cmsd;

    for (imsd = 0; imsd < cmsd; imsd++)
    {
        if (rgmsd[imsd].uMessage == uMessage)
            return rgmsd[imsd].pfnmsg(hwnd, uMessage, wparam, lparam);
    }

    return DispDefault(lpmsdi->edwp, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Call the appropriate default window procedure.
//
//  PARAMETERS:
//    edwp - Enumerate specifying the appropriate default winow procedure.
//    hwnd - The window handle
//    uMessage - The message number
//    wparam - Message specific data
//    lparam - Message specific data
//
//  RETURN VALUE:
//    If there is a default proc, return the value returned by the
//    default proc.  Otherwise, return 0.
//
//  COMMENTS:
//    Calls the default procedure associated with edwp using the specified
//    parameters.
//
LOCAL
LRESULT DispDefault(EDWP   edwp,
                    HWND   hwnd,
                    UINT   uMessage,
                    WPARAM wparam,
                    LPARAM lparam)
{
    switch (edwp)
    {
        case edwpNone:
            return 0;
        case edwpWindow:
            return DefWindowProc(hwnd, uMessage, wparam, lparam);
        case edwpDialog:
            return DefDlgProc(hwnd, uMessage, wparam, lparam);
        case edwpMDIFrame:
            return DefFrameProc(hwnd, hwndMDIClient, uMessage, wparam, lparam);
        case edwpMDIChild:
            return DefMDIChildProc(hwnd, uMessage, wparam, lparam);
    }
    return 0;
}

LOCAL
BOOL InitApplication(HINSTANCE hInstance)
{
    #ifdef __WIN16__
    WNDCLASS  wc;
    #else
    WNDCLASSEX wc;
    #endif

    static int AppInitialised=0;
    if(AppInitialised==1)
        return TRUE; //Ju¿ zosta³a zainicjalizowana

    // Load the application name  strings.
    if(LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName))==0)
        NoResources=1; //Nie pod³¹czono zasobów
        else
        {
            NoResources=0; //S¹ zasoby
            MainMenu=LoadMenu(hInstance,szAppName);
        }

    // Fill in window class structure with parameters that describe the
    // main window.

    #ifndef __WIN16__
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.hIconSm       =  NoResources?
                        NULL:
                        LoadImage(hInstance,		// Load small icon image
                                 MAKEINTRESOURCE(IDI_APPICON),
                                 IMAGE_ICON,
                                 16, 16,
                                 0);
    #endif
    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;        // Window Procedure
    wc.cbClsExtra    = 0;                       // No per-class extra data.
    wc.cbWndExtra    = 0;                       // No per-window extra data.
    wc.hInstance     = hInstance;               // Owner of this class
    wc.hIcon         = NoResources?
                            LoadIcon(NULL,IDI_APPLICATION):
                            LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON)); // Icon name from .RC
    wc.hCursor       = is_mouse!=0?
                            LoadCursor(NULL, IDC_ARROW):
                            LoadCursor(NULL, IDC_NO); // Cursor
    wc.hbrBackground = //(HBRUSH)(COLOR_WINDOW + 1); // Default color
                                CreateSolidBrush(colors[Background]);
    wc.lpszMenuName  = (NoResources?NULL:szAppName);       // Menu name from .RC

    if(NoResources)
        {
            assert(hInstance!=NULL);
            sprintf(szClassName,"SSH_%p_CLASS",hInstance);
        }
        else
        {
            sprintf(szClassName,"SSH_CLASS_%s",szAppName);
        }

    wc.lpszClassName = szClassName; // Name to register as

    // Register the window class and return FALSE if unsuccessful.
    #ifdef __WIN16__
    if (!RegisterClass(&wc))
    {
        goto FAIL;
    }
    #else
    if (!RegisterClassEx(&wc))
    {
        if( !RegisterClass((LPWNDCLASS)&wc.style) )
        {
            goto FAIL;
        }
    }
    #endif

    //
    //Module-specific application initialization functions or so...
    //
    AppInitialised=1;
    return TRUE;

FAIL:
    {
    DWORD err=GetLastError();
    char buf[1024];
    sprintf(buf,"Can't registry window class! code:%ld",err);
    MessageBoxA(0,buf,"Unexpected error",MB_ICONSTOP);
    return FALSE;
    }
}

LOCAL
BOOL InitInstance(HINSTANCE hInstance)
{

    // Create a main window for this application instance.
    MyHwnd = CreateWindow(szClassName,           // See RegisterClass() call.
                        window_name,           // Text for a window title bar.
                        MY_WIN_STYLE,
                        CW_USEDEFAULT, 0,    // Use default positioning
                        W_width,W_height,    // Use user defined size
                        NULL,                // Overlapped has no parent.
                        NULL,                // Use the window class menu.
                        hInstance,
                        NULL);

    // If window could not be created, return "failure"
    if (!MyHwnd)
        return FALSE;

    WB_Hwnd=MyHwnd; //Save to global variable

    //
    // Call module-specific instance initialization functions here.
    //

    // **INPUT** Initialize the input module.
    if (!InitInput(MyHwnd))
    {
        return FALSE;
    }

    return TRUE;                // We succeeded...
}

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: input.c
//
//  PURPOSE: Show windows input: mouse, keyboard, control(scroll), and timer
//
//  FUNCTIONS:
//    WndProc - Processes messages for the main window.
//    MsgCommand - Handle the WM_COMMAND messages for the main window.
//    MsgCreate - Set the timer for five-second intervals.
//    MsgDestroy - Kills the timer and posts the quit message.
//    MsgMouseMove - Display mouse move message and its parameters.
//    MsgLButtonDown -
//      Display left mouse button down message and its parameters.
//    MsgLButtonUp - Display left mouse button up message and its parameters.
//    MsgLButtonDoubleClick -
//      Display left mouse button double click message and its parameters.
//    MsgRButtonDown -
//      Display right mouse button down message and its parameters.
//    MsgRButtonUp - Display right mouse button up message and its parameters.
//    MsgRButtonDoubleClick -
//      Display right mouse button double click message and its parameters.
//    MsgKeyDown - Display key down message and its parameters.
//    MsgKeyUp - Display key up message and its parameters.
//    MsgChar - Display character recieved message and its parameters.
//    MsgTimer - Display timer message and a current time.
//    MsgScroll - Display scrollbar events and position.
//    MsgPaint - Draw the strings for current messages.
//    InitInput - Set up the rectangles for dispay of each type of message.
//
//   COMMENTS:
//    Message dispatch table -
//      For every message to be handled by the main window procedure
//      place the message number and handler function pointer in
//      rgmsd (the message dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Globals.h Contains the definitions of the structures and dispatch.c
//      contains the functions that use these structures.
//

static UINT idTimer;                //  timer ID
static int  nTimerCount = 0;        //  current timer count

#define TIMERID ((UINT) 't')




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant on message number)
//    lparam   - additional information (dependant on message number)
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispMessage() function with the main window's message dispatch
//    information (msdiMain) and the message specific information.
//
LOCAL
LRESULT CALLBACK WndProc
    (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiMain, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages
//
//  PARAMETERS:
//    hwnd - The window handle
//    uMessage - WM_COMMAND (unused)
//    GET_WM_COMMAND_ID(wparam,lparam) - The command number
//
//  RETURN VALUE:
//    Depends on the command.
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LRESULT lRet = 0;
    // Processing WM_COMMAND
    // Message packing of wparam and lparam have changed for Win32,
    // so use the GET_WM_COMMAND macro to unpack the command
    WORD cmd=GET_WM_COMMAND_ID(wparam,lparam);

    _TRACE( 1)
        fprintf(stderr,"WM_COMMAND MSG\n");
    _TREND

    if(cmd==IDM_SIGNAL_COMMAND)//signals simulation
        {
        raise(lparam);//Not HWND in this case!
        }
        else
        if(cmd>=MINUSERCOMMAND)
            {
            if(cmd==IDM_EXIT)
                InputChar=EOF; //Send EOF to symshell
                else
                InputChar=cmd; //Send cmd value to symshell
            }
            else
                lRet = DefWindowProc(hwnd, uMessage, wparam, lparam);

    return lRet;
}

// Handler for WM_SYSCOMMAND
LOCAL
LRESULT MsgSysCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LRESULT lRet = 0;

    // Message packing of wparam and lparam have changed for Win32,
    // so use the GET_WM_COMMAND macro to unpack the command
    _TRACE(1)
        fprintf(stderr,"WM_SYSCOMMAND MSG\n");
    _TREND

    switch (GET_WM_COMMAND_ID(wparam,lparam))
    {
        //
        // Add cases here for application specific command messages.
        //

        case SC_CLOSE:
            _TRACE( 1)
                fprintf(stderr,"   SC_CLOSE SYS COMMAND\n");
            _TREND
            InputChar=EOF; //Send EOF to symshell
            break;
//      case SC_SIZE:
//        		InputChar='\0';
// 				USE DEFAULT SUPPORT!   don't break; or move this statement
        default:
            lRet = DefWindowProc(hwnd, uMessage, wparam, lparam);
            break;
    }

    return lRet;
}


//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Set the timer for five-second intervals
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_CREATE      (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    // Set the timer for five-second intervals
//    idTimer =  SetTimer(hwnd, TIMERID, 5000, NULL);
     _TRACE( 1)
        fprintf(stderr,"CREATE MSG\n");
     _TREND

     return 0;
}

LOCAL
LRESULT MsgClose(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
     _TRACE( 1)
        fprintf(stderr,"CLOSE WND. MSG\n");
    _TREND

    InputChar=EOF; //Send EOF to symshell
     return 0;
}

//
//  FUNCTION: MsgGetMinMaxInfo(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Sets minimum window size.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Address of MINMAXINFO structure
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgGetMinMaxInfo(HWND   hwnd,
                         UINT   uMessage,
                         WPARAM wparam,
                         LPARAM lparam)
{
    _TRACE(1)
        fprintf(stderr,
              "MsgGetMinMaxInfo: %x, %d, %d\n",
              (unsigned int)wparam, LOWORD(lparam), HIWORD(lparam)
                );
    _TREND

/*
    static int     cxMin=0;
    static int     cyMin=0;
    MINMAXINFO FAR *lpmmi;

    lpmmi = (MINMAXINFO FAR *)lparam;

     if (cxMin == 0 && cyMin == 0)
    {
        HDC        hdc;
        int        i;
        TEXTMETRIC tm;
        SIZE       size;


        hdc = GetDC(hwnd);

        // find minimum allowable window size by finding widest cell label string
        for (i = 0; i < 16; i ++)
        {
                GetTextExtentPoint(hdc, aszLabel[i], lstrlen(aszLabel[i]), &size);
            cxMin = cyMin = max(cxMin, size.cx);
        }

        // add one-character-wide margin
        GetTextMetrics(hdc, &tm);
        cxMin += 2 * tm.tmAveCharWidth;
        cyMin += 2 * tm.tmAveCharWidth;

        ReleaseDC(hwnd, hdc);
    }
    lpmmi->ptMinTrackSize.x = 4 * cxMin;
    lpmmi->ptMinTrackSize.y = 4 * cyMin;
*/
    return 0;
}

//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Kills the timer and posts the quit message.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_DESTROY (Unused)
//    wparam    - Extra data (Unused)
//    lparam    - Extra data (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    _TRACE( 1)
        fprintf(stderr,"DESTROY MSG\n");
    _TREND
//    KillTimer(hwnd, idTimer);       // Stops the timer
    PostQuitMessage(0);

    return 0;
}


//
//  FUNCTION: MsgLButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display left mouse button down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_LBUTTONDOWN (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgLButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WORD xPos = LOWORD(lparam);  // horizontal position of cursor
    WORD yPos = HIWORD(lparam);  // vertical position of cursor

    if(is_mouse)
        {
       _TRACE(1)
        fprintf(stderr,
              "WM_LBUTTONDOWN: %x, %d, %d\n",
              wparam, LOWORD(lparam), HIWORD(lparam)
                );
       _TREND
           InputXpos=xPos;
            InputYpos=yPos;
            InputClick=1;
            MouseInput=1;
           InputChar='\b';
        }
    return 0;
}

//WM_XBUTTONDOWN?
//
//  FUNCTION: MsgXButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display left mouse button down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_XBUTTONDOWN (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgXButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WORD xPos = LOWORD(lparam);  // horizontal position of cursor
    WORD yPos = HIWORD(lparam);  // vertical position of cursor

    if(is_mouse)
        {
       _TRACE( 1)
        fprintf(stderr,
              "WM_XBUTTONDOWN: %x, %d, %d\n",
              wparam, LOWORD(lparam), HIWORD(lparam)
                );
       _TREND
           InputXpos=xPos;
            InputYpos=yPos;
            InputClick=4;
            MouseInput=1;
           InputChar='\b';
        }
    return 0;
}

//
//  FUNCTION: MsgLButtonDoubleClick(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display left mouse button double click message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_LBUTTONDOBLECLICK (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgLButtonDoubleClick(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WORD xPos = LOWORD(lparam);  // horizontal position of cursor
    WORD yPos = HIWORD(lparam);  // vertical position of cursor
        //InputChar='\xff';
    if(is_mouse)
        {
    _TRACE( 1)
         fprintf(stderr,
              "WM_LBUTTONDBLCLK: %x, %d, %d\n",
              wparam, LOWORD(lparam), HIWORD(lparam)
                );
    _TREND
           InputXpos=xPos;
           InputYpos=yPos;
           InputClick=4;
           MouseInput=101;
           InputChar='\b';
        }

    return 0;
}


//
//  FUNCTION: MsgRButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display right mouse button down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_RBUTTONDOWN (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgRButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    WORD xPos = LOWORD(lparam);  // horizontal position of cursor
    WORD yPos = HIWORD(lparam);  // vertical position of cursor
    if(is_mouse)
        {
            _TRACE( 1)
                fprintf(stderr,
                  "WM_RBUTTONDOWN: %x, %d, %d\n",
                wparam, LOWORD(lparam), HIWORD(lparam)
                );
            _TREND
            InputXpos=xPos;
            InputYpos=yPos;
            InputClick=2;
            MouseInput=1;
            InputChar='\b';
        }
    return 0;
}


//
//  FUNCTION: MsgRButtonDoubleClick(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display right mouse button double click message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_RBUTTONDOUBLECLICK (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgRButtonDoubleClick(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    _TRACE( 1)
         fprintf(stderr,
            "WM_RBUTTONDBLCLK: %x, %d, %d\n",
            wparam, LOWORD(lparam), HIWORD(lparam)
        );
    _TREND
    return 0;
}


//
//  FUNCTION: MsgKeyDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display key down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_KEYDOWN (Unused)
//    wparam    - Virtual Key Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgKeyDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    _TRACE(1)
        fprintf(stderr,
            "WM_KEYDOWN: %x, %x, %x\n",
            wparam, LOWORD(lparam), HIWORD(lparam)
        );
    _TREND

    return 0;
}


//
//  FUNCTION: MsgKeyUp(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display key up message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_KEYUP (Unused)
//    wparam    - Virtual Key Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgKeyUp(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    _TRACE( 1)
        fprintf(stderr,
            "WM_KEYUP: %x, %x, %x\n",
            wparam, LOWORD(lparam), HIWORD(lparam)
        );
    _TREND
    //	 InputChar='\0';
    return 0;
}


//
//  FUNCTION: MsgChar(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display character recieved message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_CHAR (Unused)
//    wparam    - Character Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgChar(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    _TRACE( 1)
        fprintf(stderr,
            "WM_CHAR: %c, %x, %x\n",
            wparam, LOWORD(lparam), HIWORD(lparam)
                );
    _TREND
    InputChar=wparam;
    return 0;
}


//
//  FUNCTION: MsgTimer(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display timer message and a current time.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_TIMER (Unused)
//    wparam    - The timer identifier
//    lparam    - NULL           (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
LOCAL
LRESULT MsgTimer(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    /*
        if ( wparam == TIMERID ) {
            wsprintf(
                TimerText,
                "WM_TIMER: %d seconds",
                nTimerCount += 5
            );
        }
    */
    _TRACE( 1)
        fprintf(stderr,
            "WM_TIMER: %c, %x, %x\n",
            wparam, LOWORD(lparam), HIWORD(lparam)
                );
    _TREND
    return 0;
}

//
//  FUNCTION: MsgPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Draw the strings for current messages.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_PAINT (Unused)
//    wparam    - Extra data (Unused)
//    lparam    - Extra data (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
static struct {LONG left,top,right,bottom;} forrepaint;
static int repaint_flag=0;

LOCAL
LRESULT MsgPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
 //   RECT rect;
    HDC  TempHdc;
    LONG left,top,right,bottom;

    _TRACE( 1)
            fprintf(stderr," REPAINT MSG ");
    _TREND

    TempHdc = BeginPaint(hwnd, &ps);

    left=ps.rcPaint.left;
    top=ps.rcPaint.top;
    right=ps.rcPaint.right;
    bottom=ps.rcPaint.bottom;

    if(animate && VirtualScreen && MbHdc )//Animujemy i jest juz bitmapa
        {
        BitBlt(	TempHdc,
                left,top,
                right-left,//W_width*mulx,
                bottom-top,//W_height*muly,
                MbHdc,
                left,top,
                SRCCOPY);

        _TRACE(1)
            fprintf(stderr,"-> BITBLT ");
        _TREND
        }
        else   //Nie ma tej treœci na bitmapie
        {
            _TRACE(1)
                fprintf(stderr,"-> FORCE REPAINT ");
            _TREND

            if(repaint_flag==1)//Jeszcze aplikacja nie odczytala
            {
                if(forrepaint.left>left) forrepaint.left=left;
                if(forrepaint.top>top) forrepaint.top=top;
                if(forrepaint.right<right) forrepaint.right=right;
                if(forrepaint.bottom<bottom) forrepaint.bottom=bottom;
            }
            else
            {
                forrepaint.left=left;
                forrepaint.top=top;
                forrepaint.right=right;
                forrepaint.bottom=bottom;
                repaint_flag=1;
            }

            InputChar='\r';//Tylko udaje ze cos wypelnia.
                //Naprawde zleca aplikacji odbudowanie

            _TRACE( 1)
                fprintf(stderr," %ld %ld %ld %ld ",
                    forrepaint.left,
                    forrepaint.top,
                    forrepaint.right,
                    forrepaint.bottom);
            _TREND
        }

    EndPaint(hwnd, &ps);
    _TRACE(1)
        fprintf(stderr," * \n");
    _TREND
    return 0;
}

int  repaint_area(ssh_coordinate* x, ssh_coordinate* y,
                  ssh_natural* width, ssh_natural* height)/* Podaje obszar, który ma byæ odnowiony i zwraca 0 */
                                                        /* Jeœli zwraca -1 to brak danych lub brak implementacji! Odrysowaæ ca³oœæ. */
                                                        /* Jeœli zwraca -2 to znaczy, ¿e dane ju¿ zosta³y odczytane. Nale¿y zignorowaæ. */
{
    if(repaint_flag==1)
        {
        _TRACE(2)
            fprintf(stderr," Repaint_area() from %d,%d - %dx%d pixels\n",*x,*y,*width,*height);
        _TREND

        *x=forrepaint.left/mulx;
        *y=forrepaint.top/muly;
        if(mulx>1)
            *width=(forrepaint.right-forrepaint.left+(mulx-1))/mulx;
            else
            *width=forrepaint.right-forrepaint.left;

        if(muly>1)
            *height=(forrepaint.bottom-forrepaint.top+(muly-1))/muly;
            else
            *height=forrepaint.bottom-forrepaint.top;
        repaint_flag=0;
        return 0;
        }
        else
        return -1; //Nie by³o nic odczytania (?)
}

LOCAL
LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  //WORD	fwSizeType = wparam;      // resizing flag
  WORD	nWidth = LOWORD(lparam);  // width of client area
  WORD	nHeight = HIWORD(lparam); // height of client area
  WORD  n_mulx;//=0,  //Assigned in next if instruction!
  WORD  n_muly;//=0;  //Assigned in next if instruction!

  if(!Flexible)
        {
        n_mulx=(nWidth-ini_col*raw_char_width())/ini_width;
        n_muly=(nHeight-ini_row*raw_char_height())/ini_height;
        if(n_mulx<1) n_mulx=1;
        if(n_muly<1) n_muly=1;
        }
        else
        {
        n_mulx=1;n_muly=1;
        }

     _TRACE(3)//trace_level & 1)||(trace_level & 2))
        fprintf(stderr,"MSG RESIZE to %u %u \n",nWidth,nHeight);
     _TREND

      if(Flexible || n_mulx!=mulx || n_muly!=muly)// Rozmiar faktycznie sie zmienil
          {
          if(!Flexible)
            {
            W_width=ini_width+(ini_col*raw_char_width())/n_mulx;
            W_height=ini_height+(ini_row*raw_char_height())/n_muly;
            mulx=n_mulx;muly=n_muly;
            _TRACE(2)
                fprintf(stderr,"RES. MULT. X=%u Y=%u ",mulx,muly);
            _TREND
            }
            else
            {
            W_width=nWidth;
            W_height=nHeight;
            mulx=n_mulx;muly=n_muly;
            _TRACE(2)
                fprintf(stderr,"RES. FLEXIBLE ");
            _TREND
            }

          FreeResources();//DC , Pens , Brushes
          assert(MbHdc==0);
          assert(VirtualScreen==NULL);
          if(animate)
                {
                _TRACE( 2)
                    fprintf(stderr,"-->FORCE REPAINT");
                _TREND
                InputChar='\r';
                }
          }

      _TRACE( 2)
        fprintf(stderr,"* \n");
      _TREND

      return 0;
}

// Main window message table definition.
static MSD rgmsd[] =
{
    {WM_COMMAND,        MsgCommand},
    {WM_SYSCOMMAND,		MsgSysCommand},
    {WM_CREATE,         MsgCreate},
    {WM_CLOSE,				MsgClose},    //Send EOF to symshell
//    {WM_GETMINMAXINFO, MsgGetMinMaxInfo},
    {WM_SIZE,				MsgSize},	  //resize client area
    {WM_DESTROY,        MsgDestroy},
//    {WM_MOUSEMOVE,      MsgMouseMove},
    {WM_LBUTTONDOWN,    MsgLButtonDown},
//    {WM_LBUTTONUP,      MsgLButtonUp},
    {WM_LBUTTONDBLCLK,  MsgLButtonDoubleClick},   
    {WM_RBUTTONDOWN,    MsgRButtonDown},
//    {WM_RBUTTONUP,      MsgRButtonUp},
    {WM_RBUTTONDBLCLK,  MsgRButtonDoubleClick},
    {WM_XBUTTONDOWN,  MsgXButtonDown},
//    {WM_KEYDOWN,        MsgKeyDown},
//    {WM_KEYUP,          MsgKeyUp},
    {WM_CHAR,           MsgChar},
//    {WM_TIMER,          MsgTimer},
//    {WM_HSCROLL,        MsgScroll},
//    {WM_VSCROLL,        MsgScroll},
    {WM_PAINT,          MsgPaint}
};

MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};

//
//  FUNCTION: InitInput(HWND)
//
//  PURPOSE: Set up the rectangles for dispay of each type of message
//
//  PARAMETERS:
//    hwnd   - Window handle
//
//  RETURN VALUE:
//    Always returns TRUE - Sucess
//
//  COMMENTS:
//
//
LOCAL
BOOL InitInput(HWND hwnd)
{
    /*HDC hdc;
    TEXTMETRIC tm;
    RECT rect;
    int  dyLine;
    */
    _TRACE(1)
        fprintf(stderr," InitInput() ");
    _TREND

   return TRUE;
}

//
//	Blok tworzenia zrzutu ekranu
//  z przyk³adów Microsoftu
//
static jmp_buf dump_jumper; /* JUMP bufor na wypadek b³êdu */

void errhandler(const char* text,HWND hwnd,int Errnum)
{
    MessageBoxA(hwnd,text,"Window capture error",MB_ICONERROR);
    if(errno==0 && Errnum>0) //Ma ustawiæ Errno
        errno=Errnum;

    longjmp(dump_jumper,Errnum);

    //Tu nigdy nie powinien wejœæ!
    exit(Errnum);//Debug
}

LOCAL
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp) {
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    /* Retrieve the bitmap's color format, width, and height. */

    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
        errhandler("GetObject", hwnd, EINVAL );


    /* Convert the color format to a count of bits. */

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);

    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else
        cClrBits = 32;

    /*
     * Allocate memory for the BITMAPINFO structure. (This structure
     * contains a BITMAPINFOHEADER structure and an array of RGBQUAD data
     * structures.)
     */

    if (cClrBits != 24)
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER) +
                    sizeof(RGBQUAD) * (2^cClrBits));

    /*
     * There is no RGBQUAD array for the 24-bit-per-pixel format.
     */

    else
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
                    sizeof(BITMAPINFOHEADER));



    /* Initialize the fields in the BITMAPINFO structure. */

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = 2^cClrBits;


    /* If the bitmap is not compressed, set the BI_RGB flag. */

    pbmi->bmiHeader.biCompression = BI_RGB;

    /*
     * Compute the number of bytes in the array of color
     * indices and store the result in biSizeImage.
     */

    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8
                                  * pbmi->bmiHeader.biHeight
                                  * cClrBits;

    /*
     * Set biClrImportant to 0, indicating that all of the
     * device colors are important.
     */

    pbmi->bmiHeader.biClrImportant = 0;

    return pbmi;

}


//The following example code defines a function that initializes the remaining structures, retrieves the array of palette indices, opens the file, copies the data, and closes the file.
static LPBYTE lpBits=NULL;              /* memory pointer */
LOCAL
void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
                  HBITMAP hBMP, HDC hDC)
 {

    HANDLE hf;                  /* file handle */
    BITMAPFILEHEADER hdr;       /* bitmap file-header */
    PBITMAPINFOHEADER pbih;     /* bitmap info-header */
 //   DWORD dwTotal;              /* total count of bytes */
    DWORD cb;                   /* incremental count of bytes */
    BYTE *hp;                   /* byte pointer */
    DWORD dwTmp;


    pbih = (PBITMAPINFOHEADER) pbi;
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits)	
    {
         fprintf(stderr,"GlobalAlloc ENOMEM");
         errhandler("GlobalAlloc", hwnd, ENOMEM);
    }
    /*
     * Retrieve the color table (RGBQUAD array) and the bits
     * (array of palette indices) from the DIB.
     */

    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight,
                   lpBits, pbi, DIB_RGB_COLORS))
    {
        fprintf(stderr,"GetDIBits EINVAL");
        errhandler("GetDIBits", hwnd , EINVAL );//errno
    }

    /* Create the .BMP file. */

    hf = CreateFile(pszFile,
                   GENERIC_READ | GENERIC_WRITE,
                   (DWORD) 0,
                   (LPSECURITY_ATTRIBUTES) NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   (HANDLE) NULL);

    if (hf == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr,"CreateFile EBADF");
        errhandler("CreateFile", hwnd , EBADF );
    }

    hdr.bfType = 0x4d42;        /* 0x42 = "B" 0x4d = "M" */

    /* Compute the size of the entire file. */

    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
                 pbih->biSize + pbih->biClrUsed
                 * sizeof(RGBQUAD) + pbih->biSizeImage);

    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    /* Compute the offset to the array of color indices. */

    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
                    pbih->biSize + pbih->biClrUsed
                    * sizeof (RGBQUAD);

    /* Copy the BITMAPFILEHEADER into the .BMP file. */

    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
       (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
       errhandler("WriteFile", hwnd , EBADF );

    /* Copy the BITMAPINFOHEADER and RGBQUAD array into the file. */

    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
                  + pbih->biClrUsed * sizeof (RGBQUAD),
                  (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
       errhandler("WriteFile", hwnd , EBADF );

    /* Copy the array of color indices into the .BMP file. */
    cb = pbih->biSizeImage;
    //dwTotal = cb

    hp = lpBits;
    while (cb > MAXWRITE)  {
            if (!WriteFile(hf, (LPSTR) hp, (int) MAXWRITE,
                          (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
                errhandler("WriteFile", hwnd , EBADF );
            cb-= MAXWRITE;
            hp += MAXWRITE;
    }
    if (!WriteFile(hf, (LPSTR) hp, (int) cb,
         (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
           errhandler("WriteFile", hwnd , EBADF );

    /* Close the .BMP file. */

    if (!CloseHandle(hf))
    {
        fprintf(stderr,"CloseHandle EINVAL");
        errhandler("CloseHandle", hwnd, EINVAL );
    }
    /* Free memory. */

    GlobalFree((HGLOBAL)lpBits);
    lpBits=NULL;
}


/* Zrzuca ekran na plik o ekstensji zale¿nej od platformy. -1 jeœli sie nie uda. */
int  dump_screen(const char* Filename)
{
    HDC hdcScreen = GetMyHdc();
    HBITMAP hbmScreen=0;
    HDC hdcCompatible=0;
    PBITMAPINFO info;
    char bufor[2048];
    //TODO - problemy z za ma³ym rozmiarem obrazka!
    if(strlen(Filename)+6 > 2048)
    {
        fprintf(stderr,"Filename \"%s\" is too long!!!",Filename);
        return -1;
    }

    sprintf(bufor,"%s.BMP",Filename);

    _TRACE( 2 )
        fprintf(stderr,"DUMP SCREEN TO FILE %s ", bufor);
    _TREND

    errno=0; /* Kasowanie œmieci w kodach b³êdów */
    if(setjmp(dump_jumper)!=0) //Tu wskakuje gdy awaria
    {
        if(hbmScreen)
            DeleteObject(hbmScreen);
        if(hdcCompatible )
            DeleteDC( hdcCompatible );
        if(lpBits)
        {
            GlobalFree((HGLOBAL)lpBits);
            lpBits=NULL;
        }
        if(errno==0)
            errno=ENOEXEC;

        _TRACE(2)
            fprintf(stderr,"WRITE FAILED \n");
        _TREND
        return -1; //B£¥D!
    }

    /* GRABBING WINDOW */
    hdcCompatible = CreateCompatibleDC(hdcScreen);

    /* Create a compatible bitmap for hdcScreen. */

    hbmScreen = CreateCompatibleBitmap(hdcScreen, W_width*mulx, W_height*muly );
    // GetDeviceCaps(hdcScreen, HORZRES),
    // GetDeviceCaps(hdcScreen, VERTRES));

    if (hbmScreen == 0)
        errhandler("Create Compatible Bitmap (hdcScreen)", MyHwnd, ENOMEM );

    /* Select the bitmaps into the compatible DC. */

    if (!SelectObject(hdcCompatible, hbmScreen))
        errhandler("Compatible Bitmap Selection", MyHwnd , -1);

    /*
        * Copy color data for the entire display into a
        * bitmap that is selected into a compatible DC.
    */

    if (!BitBlt(hdcCompatible,
        0,0,
        W_width*mulx, W_height*muly ,
        hdcScreen,
        0,0,
        SRCCOPY))
        errhandler("Screen to Compat Blt Failed", MyHwnd, -1);

    /* SAVING */
    info=CreateBitmapInfoStruct(MyHwnd,hbmScreen);
    CreateBMPFile(MyHwnd,bufor,info,hbmScreen,hdcCompatible);

    /* FREEING */
    if(hbmScreen)
        DeleteObject(hbmScreen);
    if(hdcCompatible )
        DeleteDC( hdcCompatible );
    errno=ENOEXEC;

    _TRACE( 2)
        fprintf(stderr,"Write OK \n");
    _TREND

    return 0; //Czy b³¹d?
}

int title_with_pid=1;

/* Przekazanie parametrów wywo³ania */
void shell_setup(const char* title,int iargc,const char* iargv[])
{
    int i;
    int largc=iargc;
    const char** largv=iargv;
    if(largv && largv[0])
        progname=largv[0];
        else
        progname="SYMSHELL TESTING PROGRAM";

    for(i=1;i<largc;i++)
    {
        if(strcmp(largv[i],"-logo")==0)
        {
            symshell_about(title);
        }
        else
        if(strncmp(largv[i],"-help",5)==0)
            {
            printf("SYMSHELL for MS Windows. %s\n",Copyright);
            printf("Supported swithes:\n"
                   "\t -mapped[+/-] \n"
                   "\t -buffered[+/-]\n"
                   "\t -traceenv[1|2|4]\n"
                   "\t -mouse[+/-]\n"
                   "\t -gray[+/-]\n"
                   "\t -flex[+/-]\n"
                   "\t -font=[?/!/ini file]\n"
                   "\t -delay=[ms]\n"
                   "\t -width=[pix]\n"
                   "\t -height=[pix]\n"
                   "\t -pid\n"
                   "\t -help\n"
                   "\t -logo\n"
                   );
                    exit(-1);//2020-11-17 WB
            }
        else
        if(strncmp(largv[i],"-delay=",7)==0)
            {
            DelayTime=atoi(largv[i]+7);
            printf("Delay time after any refreshing of gr. window set to %d ms\n",DelayTime);
            }
        else
        if(strncmp(largv[i],"-width=",7)==0)
            {
            ForceWidth=atoi(largv[i]+7);
            printf("Width of gr. window forced to %d pix\n",ForceWidth);
            }
        else
        if(strncmp(largv[i],"-height=",8)==0)
            {
            ForceHeight=atoi(largv[i]+8);
            printf("Height of gr. window forced to %d pix\n",ForceHeight);
            }
        else
        if(strncmp(largv[i],"-gray",5)==0)
            {
            UseGrayScale=(largv[i][5]=='+')?1:0;
                printf("Gray scale is %s\n",(UseGrayScale?"ON":"OFF"));
            }
        else
        if(strncmp(largv[i],"-flex",5)==0)
            {
            Flexible=(largv[i][5]=='+')?1:0;
            printf("Window sizing is %s\n",(Flexible?"FLEXIBLE":"DISCRETE"));
            }
            else
        if(strncmp(largv[i],"-pid",4)==0)
            {
            title_with_pid=(largv[i][4]=='+')?1:0;
            printf("PID in Window title is %s\n",(title_with_pid?"ON":"OFF"));
            }
        else
        if(strncmp(largv[i],"-font=",6)==0)
            {
            UserFontStr=largv[i]+6;
            printf("User is forcing font selection to %s\n",UserFontStr);
            }
        else
        if(strncmp(largv[i],"-mapped",7)==0)
            {
            is_buffered=(largv[i][7]=='+')?1:0;
                printf("Double mapping is %s\n",(is_buffered?"ON":"OFF"));
            if(!is_buffered)	/* Jak nie ma bitmapy to nie mozna */
                animate=0; /* animowac */
            }
        else
        if(strncmp(largv[i],"-mouse",6)==0)
            {
            is_mouse=(largv[i][6]=='+')?1:0;
            printf("Mouse is %s\n",(is_mouse?"ON":"OFF"));
            }
        else
        if(strncmp(largv[i],"-buffered",9)==0)
            {
            animate=(largv[i][9]=='+')?1:0;
                printf("Buffered is %s\n",(animate?"ON":"OFF"));
            //if(animate)	/* Musi byc w³¹czona bitmapa buforuj¹ca */
            //	is_buffered=1;/* ¿eby mo¿na by³o na nia pisaæ */
                is_buffered=animate; /*Albo,albo.NIe jak w Xwindow */
            }
       /*
        else
        if(strncmp(largv[i],"-bestfont",9)==0)
            {
            ResizeFont=(largv[i][9]=='+')?1:0;
                printf("Search best font is %s\n",(ResizeFont?"ON":"OFF"));
            }
       */
        else
        if(strncmp(largv[i],"-traceevt",9)==0)
            {
            trace_level=atol(largv[i]+9);
                printf("Trace level is %d\n",trace_level);
            }
        else
        if(strcmp(largv[i],"-ogol")==0)
        {
            symshell_about(title);
            fflush(stdout);
        }
    }

    if(animate)	/* Musi byc w³¹czona bitmapa buforuj¹ca */
        is_buffered=1;/* ¿eby mo¿na by³o na ni¹ pisaæ */

    #if defined(_MSC_VER)
    #define snprintf _snprintf
    #define getpid   _getpid
    #endif

    if(title_with_pid)
        snprintf(window_name,sizeof(window_name),"%u: %s", getpid() ,title);
    else
        strncpy(window_name,title,sizeof(window_name)); // (window_name = title;

    strncpy(icon_name,title,sizeof(icon_name)); //icon_name = title;

    //fprintf(stdout,"\n");
    fflush(stdout);
}

/* ******************************************************************/
/*                                                   2026           */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiów Spo³ecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://borkowsk.iss.uw.edu.pl                       */
/*        MAIL: wborkowski@uw.edu.pl                                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/

