static char* Copyright=	"SYMSHELL LIBRARY(c)1997-2007 ;\n"
						"By Wojciech Borkowski, Warsaw University\n("
						__DATE__" - compilation timestamp)\n";
//Partially based on Borland and Microsoft examples
/*****************************************************************************************
* Implementacja najprostrzego interface'u wizualizacyjnego dla MS Windows 32.
* U¿ywano w: Borland 5.0, MS Visual C++ 4.0 , 5.0, 6.0 i nowszych
*
* Wersja dla Borland Developer Studio 2006
*
* Ostatnia modyfikacja/Last modification: 28.03.2008
UWAGA ProcessMsg u¿ywa teraz zamiast NULL uchwytu okna pobierajac komunikat - to powinno 
dzia³aæ lepiej, ale pewnoœci nie ma.
******************************************************************************************/
//Define EXTERN_WB_ABOUT and function of following prototype for redefine usage of ShellAbout
//int wb_about(const char* window_name);//Z biblioteki albo dostarczona z programem

extern int WB_error_enter_before_clean;/* For controling closing graphics window on error*/   

#include <windows.h>
#include <windowsx.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <errno.h>
#include <setjmp.h>

//#ifdef __MSVC__
//#pragma warning(disable:4068)
//#endif

#include "symshwin.h"			// prototypes specific to this application

#include "_sig_msg.h"			// for compatibility with wb_posix.

#include "symshell.h"

#define MY_WIN_STYLE		      (WS_OVERLAPPEDWINDOW/* | WS_HSCROLL | WS_VSCROLL*/)
#define MAXWRITE		0xfff0	  //Bezpieczny rozmiar bufora
#define MINUSERCOMMAND  IDM_EXIT  //*Komenda uzytkownika o najnizszym numerze */

/* FOR OTHER MODULES */
HINSTANCE	WB_Instance=0;
HINSTANCE	WB_PrevInstance=0;
HWND		WB_Hwnd=0;					//???
HWND		MyHwnd=0;						//Main window handle.

static const char* progname="WB SYMSHELL APPLICATION "__DATE__;
static const char* window_name="Windows WB SYMSHELL interface "__DATE__;
static const char* icon_name="Windows SYMSHELL "__DATE__;
static char szAppName[128]="SYMSHELL";   // The name of this application, if not in resources
static char szClassName[128]="CLASS_SYMSHELL";//The name of window class

static	HANDLE	hAccelTable;		// For read from resources
static	HDC	MyHdc=0;				//Current HDC
static	HDC	WnHdc=0;				//Window HDC
static	HDC	MbHdc=0;				//Memory bitmap HDC
static	HMENU	MainMenu=0;			//Menu handle loaded from resorce
static	HBITMAP	VirtualScreen=0;	//Memory bitmap CreateCompatible BitBlt
static	HFONT	UserFont=0;			//Handle to extra font
static	TEXTMETRIC	font_info;		//Struktura na in formacje o aktualnym foncie
static	MSG	msg;					//Struktura na aktualny komunikat


typedef struct 
{
	HPEN handle;
	int  size;	//Grubosci zaalokowanych piór
	int  style;	//Style dla zaalokowanych piór
}	piora;

#ifdef cplusplus
const unsigned PALETE_LENGHT=512;
#else
#define PALETE_LENGHT (512)
#endif


static COLORREF curent_pen_rgb=RGB(0,0,0);
//static COLORREF curent_brush_rgb=RGB(0,0,0);
static COLORREF colors[PALETE_LENGHT];	    //Tablice uchwytow do kolorow

static HBRUSH curent_brush=NULL;			//Aktualny uchwyt do pendzla
static HBRUSH free_style_brush=NULL;		//Alokowany pedzel dowolny
static HBRUSH brushes[PALETE_LENGHT];		//Tablice uchwytow do pedzli

static HPEN curent_pen=NULL;				//Aktualny uchwyt do pióra
static HPEN free_style_pen=NULL;			//Alokowane dowolne pióro
static piora	pens[PALETE_LENGHT];		//Tablice uchwytow do piór. 

static int trace_level=0;			//Maska poziomów œledzenia 1-msgs 2-grafika 4-alokacje/zwalnianie
//np:
//if(trace_level & 4)
//		fprintf(stderr," FREE RESORCES.\n");
static int NoResources=1;			// No resources attached to exe file
static int WindowClosed=1;			// Window destroyed/notopen flag for close_plot
static int curr_color=-1;
static int curr_fill=-1;
static int Background=0;
static int is_mouse=0;
static int animate=0;
static int is_buffered=0;
static int transparently=0;
static int Flexible=0;			//Czy dopuscic elestyczny rezizing okna

static int DelayTime=0;			//Delay time after gr. sync
static int LineWidth=1;
static int LineStyle=SSH_LINE_SOLID;
static int mulx=1;
static int muly=1;
static int W_width,W_height;
static int ini_width;		//MAXX+1 obszaru bitowego
static int ini_height;		//MAXY+1 obszaru bitowego
static int ForceHeight=0;	//Na rozmiary okna wymuszone parametrami wywo³ania
static int ForceWidth=0;	// -width=  i -height=
static int ini_col;			//Ile kolumn tekstu dodatkowo
static int ini_row;			//Ile wierszy tekstu dodatkowo
static int Is_finfo=0;
static const char* UserFontStr=NULL;
static int CharToGet=0;
static int UseGrayScale=0;	//Flaga uzycia skali szarosci

static HPEN GetMyPen(ssh_color color,int size,int style);
static HBRUSH GetMyBrush(ssh_color color);
static HFONT MyCreateFont( const char* fontstr );
static HDC GetVirtualScreen(void);
static HDC GetRealScreen(void);

#ifdef EXTERN_WB_ABOUT
extern int wb_about(const char* window_name);//Z biblioteki albo dostarczona z programem
#else
static int wb_about(const char* window_name)
{
char bufor[256];
extern HINSTANCE WB_Instance;
extern HWND		 WB_Hwnd;
HICON hIcon=0;

hIcon=LoadIcon(WB_Instance, MAKEINTRESOURCE(IDI_APPICON));
ShellAbout(WB_Hwnd,window_name,Copyright ,hIcon);

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
MessageBox(MyHwnd,bufor,"SYMSHELL GRAPHICS ERROR",MB_ICONSTOP);
return 'A';// abort();
}

static HDC GetRealScreen(void)
{
if(MyHdc==0)	
	{
	MyHdc=GetDC(MyHwnd);
	//Gdyby mialy byc inne fonty niz domyslny
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
	//Gdyby mialy byc inne fonty niz domyslny
	if(UserFont) //Not NULL
	{
		SelectObject(MbHdc,UserFont);
	}
	GetTextMetrics(MbHdc,&font_info);
	Is_finfo=1;
	}
return MbHdc;
}

int ReadFontSettings(const char* fontsettingsfile,LOGFONT* LogFont)
{
    if(LogFont==NULL) goto CATCHERROR;//Blad
    if(fontsettingsfile==NULL)  goto CATCHERROR;//Blad

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
        "FONT",         //LPCTSTR lpAppName,        // points to section name
        "lfFaceName",   //LPCTSTR lpKeyName,        // points to key name
        "",             //LPCTSTR lpDefault,        // points to default string
        LogFont->lfFaceName,//LPTSTR lpReturnedString,  // points to destination buffer
        LF_FACESIZE,  //DWORD nSize,              // size of destination buffer
        fontsettingsfile//LPCTSTR lpFileName        // points to initialization filename
        );
   return 1;
CATCHERROR:
   return 0;
}

void WriteFontSettings(const char* fontsettingsfile,LOGFONT* LogFont)
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
	int req_font_size=atoi(fontstr);//Jesli nie liczba to wychodzi i tak 0

	if(trace_level & 4)
		fprintf(stderr,"Create font %s.\n",fontstr);

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
        if( ReadFontSettings("./SYMSHELL.INI",cf.lpLogFont)==0) //Czytanie z domyslnego pliku
            ChooseFont(&cf); //Awaryjnie, jesli nie uda sie wczytac
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
            ChooseFont(&cf); //Awaryjnie, jesli nie uda sie wczytac
    }
	
    // Create a logical font based on the user's 
	// selection and return a handle identifying     // that font.  
	hfont = CreateFontIndirect(cf.lpLogFont);     
	return (hfont); 
} 

static HDC GetMyHdc(void)
/* DC caching */
{

if(!UserFont && UserFontStr) //If NULL but need not be, then create
	{
	UserFont=MyCreateFont(UserFontStr);
	if(!UserFont)
		{
		MessageBox(MyHwnd,UserFontStr,"Font allocation failed",MB_ICONSTOP);
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

    if(brushes[color]==0)//Trzeba alokowac pedzel
	{ 
		DeleteObject(brushes[color]);//???
        brushes[color]=CreateSolidBrush(colors[color]);
	}
    return curent_brush=brushes[color];
}

void	set_brush(ssh_color c)
/* Ustala aktualny kolor wypelnien za pomoca typu ssh_color */
{
	HDC hdc=GetMyHdc();
	HBRUSH MyBrush=GetMyBrush(c);
	SelectObject(hdc,MyBrush);
	curr_fill=-1;//Funkcje same ustawiajace brush musza to zrobic po uzyciu set_brush
}

void	set_brush_rgb(int r,int g,int b)
/* Ustala aktualny kolor wypelnien za pomoca skladowych RGB */
{
	COLORREF color=RGB(r,g,b);    
	if(free_style_brush!=NULL)
		DeleteObject(free_style_brush);
	free_style_brush=curent_brush=CreateSolidBrush(color);
	SelectObject(GetMyHdc(),free_style_brush);
	curr_fill=-1;//Funkcje same ustawiajace brush musza to zrobic po uzyciu set_brush
}

static HPEN GetMyPen(ssh_color color,int size,int style)
{
    assert(color<PALETE_LENGHT);

	if(size<=0) 
		size=1; // conajmniej grubosc 1
	
	if( pens[color].handle==0 ||
		pens[color].size!=size || 
		pens[color].style!=style)//Trzeba alokowac pedzel gdy zmienia sie rozmiar lub styl
	{
		if(pens[color].handle!=0) 
		{
			DeleteObject(pens[color].handle);//Wymaga usuwania 
			pens[color].handle=0;
		}
		
		if(trace_level & 4)
			fprintf(stderr,"Pen allocation c:%x s:%d style:%d.\n",color,size,style);
		
		{	//WLASCIWA ALOKACJA
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

void	set_pen(ssh_color c,int size,int style)
/* Ustala aktualny kolor linii za pomoca typu ssh_color */
{
	HDC hdc=GetMyHdc();
	
	curent_pen=GetMyPen(c,size,style);
    SelectObject(hdc,curent_pen);
	curr_color=-1;/* Funcje same ustawiajace pen sa zmuszone to zrobic po uzyciu set_pen */
	LineStyle=style;//???
	LineWidth=size;
}

void	set_pen_rgb(int r,int g,int b,int size,int style)
/* Ustala aktualny kolor linii za pomoca skladowych RGB */
{
	if(free_style_pen)
		DeleteObject(free_style_pen);
	
	if(trace_level & 4)
			fprintf(stderr,"Free style pen allocation rgb:%x%x%x s:%d style:%d.\n",r,g,b,size,style);
	//Tworzenie nowego piora
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
	//Od razu uzycie piora
	SelectObject(GetMyHdc(),free_style_pen);
	curr_color=-1;/* Funcje same ustawiajace pen sa zmuszone to zrobic po uzyciu set_pen */
	LineStyle=style;//???
	LineWidth=size;
}

void set_rgb(ssh_color color,int r,int g,int b)
/* Zmienia definicja koloru. Indeksy 0..255 */
{
    assert(color<PALETE_LENGHT);

	colors[color]=RGB(r,g,b);
	
	if(brushes[color]!=0)
	{
		if(curr_fill==color)
		{
			curr_fill=-1;/* Wymusza wymiane pedzla przy nastepnym rysowaniu*/
            if(MyHdc!=0)
				SelectObject(MyHdc,GetStockObject(NULL_BRUSH));/*Wymiata z kontekstu */
		}
		DeleteObject(brushes[color]);/* Teraz mozna juz zwolnic */
		brushes[color]=0;
	}

	if(pens[color].handle!=0)
	{
		if(curr_color==color)
      	{
			curr_color=-1;/* Wymusza wymiane piora przy nastepnym rysowaniu*/
            if(MyHdc!=0)
				SelectObject(MyHdc,GetStockObject(NULL_PEN));/*Wymiata z kontekstu */
		}
	
		DeleteObject(pens[color].handle);/* Teraz mozna juz zwolnic */
		pens[color].handle=0;
		pens[color].size=0;
		pens[color].style=0;
	}
}

void set_gray(ssh_color shade,int intensity)
/* Zmiania definicje odcienia szarosci. Indeksy 256..511 */
{
    assert(255<shade && shade<PALETE_LENGHT);
    set_rgb(shade,intensity,intensity,intensity);
}

/*
static HPEN Get1Pen(int color)
{
if(pens[color]==0)//Trztzeba alokowac pedzel
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
	
	if(trace_level & 4)
		fprintf(stderr," FREE RESORCES.\n");
	
	if(MbHdc)		  //Zwalnia wirtualny kontekst jesli jest
	{ 
		if(MbHdc==MyHdc) 
			MyHdc=0; 
		DeleteDC(MbHdc); 
		MbHdc=0;
	}
	
	if(VirtualScreen!=0) //Zwalnia ekran wirtualny jesli jest
	{ 
		DeleteObject(VirtualScreen); 
		VirtualScreen=0; 
	}
	
	if(MyHdc!=0)		//Zwalnia kontekst okna jesli jest
	{ 
		int ret=ReleaseDC(MyHwnd,MyHdc);            //Czy to aby dziala
                                                    assert(ret==1);
		MyHdc=0;   
	}
	
	for(k=0;k<PALETE_LENGHT;k++)
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

static void SetScale(void)
{
#ifndef M_PI
const double M_PI=3.141595;
#endif
extern void set_rgb(ssh_color color,int r,int g,int b);

if(UseGrayScale)//Uzywa skali szarosci tam gdzie normalnie sa kolory
  {
	int k;
	for(k=0;k<255;k++)
	{
		long wal=k;
		//fprintf(stderr,"%u %ul\n",k,wal);
		set_rgb(k,wal,wal,wal); //Color part
		set_rgb(256+k,wal,wal,wal);//Gray scale part
	}
	if(trace_level & 4)
		fprintf(stderr,"%s\n","SetScale (0-255 Gray) completed");
  }
  else
  {
  int k;
  for(k=0;k<255;k++)
	{
		  long wal1,wal2,wal3;
		  double kat=(M_PI*2)*k/255.;
		  
		  wal1=(long)(255*sin(kat*1.22));
		  if(wal1<0) wal1=0;
		  
		  wal2=(long)(255*(-cos(kat*0.46)));
		  if(wal2<0) wal2=0;
		  
		  wal3=(long)(255*(-cos(kat*0.9)));
		  if(wal3<0) wal3=0;
		  
		  set_rgb(k,wal1,wal2,wal3);
		  
		  /*
		  wal1=(long)(255*sin(kat*1.25));
		  
		   if(wal1<0) wal1=0;
		   wal2=(long)(255*(-sin(kat*0.85)));
		   if(wal2<0) wal2=0;
		   wal3=(long)(255*(-cos(kat*1.1)));
		   if(wal3<0) wal3=0;
		  */
	  }
	  //ALTERNATYWNIE?
	  {
	  unsigned k;
	  for(k=256;k<PALETE_LENGHT; k++)
			set_rgb(k,(unsigned char)k,(unsigned char)k,(unsigned char)k );
	  if(trace_level & 4)
		fprintf(stderr,"%s\n","SetScale (Colors: 0-255; Gray: 256--> %d) completed",PALETE_LENGHT);
	  }
  }

	set_rgb(255,255,255,255);

}



static int InitWindowClass(void)
{
// Other instances of app running?	
if (!WB_PrevInstance)
    {
		// Initialize shared things
		if(trace_level & 4)
			fprintf(stderr,"%s\n","Init Window Class");
			
		if (!InitApplication(WB_Instance))
		{
			if(trace_level & 4)
				fprintf(stderr,"%s\n","  FAILED!!!");
			return FALSE;               // Exits if unable to initialize
		}
	}
return TRUE;
}

static int InitMainWindow(void)
{
// Perform initializations that apply to a specific instance
if(trace_level & 4)
	fprintf(stderr,"%s\n","Init Main Window");
if(!InitInstance(WB_Instance))
	{
        return FALSE;
    }
if(!NoResources)//Jesli sa zasoby to staramy sie zaladowac akcelerator
	hAccelTable = LoadAccelerators(WB_Instance, szAppName);
return TRUE;
}


/* CONFIGURATION AND MENAGE SHELL - USE IN THIS ORDER! */
/*******************************************************/

void fix_size(int yes)
/* Ustala czy rozmiar okna moze byæ plynnie zmieniany */
{
	Flexible=!yes;
}

int fixed()				
/* Czy okno ma zafiksowana wielkosc */
{
	return !Flexible;
}

int line_width(int W)
{
	unsigned pom=LineWidth;
	LineWidth=W;
	curr_color=-1;//Wymusza realokacje jesli zostanie uzyty aktualny kolor
	if(MyHdc)
		SelectObject(MyHdc,GetStockObject(NULL_PEN));/*Wymiata z kontekstu */
	return pom;
}

int get_line_width()
{
	return LineWidth;
}

int line_style(int S)
{
	int pom=LineStyle;
	LineStyle=S;
	curr_color=-1;//Wymusza realokacje jesli zostanie uzyty aktualny kolor
	if(MyHdc)
		SelectObject(MyHdc,GetStockObject(NULL_PEN));/*Wymiata z kontekstu */
	return pom;
}

int		put_style(int S)
{//SSH_SOLID_PUT,SSH_XOR_PUT
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

int  mouse_activity(int yes)
/* Ustala czy mysz ma byc obslugiwana. Zwraca stan flagi */
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

void set_background(ssh_color color)
/* Ustala index koloru do czyszczenia itp */
{
    assert(color<PALETE_LENGHT);
    Background=color;
}

ssh_color background()
{
    return Background;
}

void buffering_setup(int _n)
/* Przelaczanie buforowanie okna na serverze */
{
    if(_n)
        animate=1;
    else
        animate=0;
    if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
        is_buffered=1;/* zeby mozna bylo na nia pisac */
}

int buffered()
{
    return is_buffered;
}

/* GETTING SCREEN PARAMETERS */
/*---------------------------*/
int  screen_height()
/*ostateczne rozmiary okna po przeliczeniach z */
{
return W_height;
}

int  screen_width()
/*do pozycjonowania na marginesach */
{
return W_width;
}

int  raw_char_height()
/* Aktualne rozmiary znaku  */
{
if(!Is_finfo)
	GetMyHdc();//Wymusza przeczytanie informacji o foncie
return  font_info.tmHeight;
}

int  raw_char_width()
/* potrzebne do pozycjonowania tekstu */
{
if(!Is_finfo)
	GetMyHdc();//Wymusza przeczytanie informacji o foncie
return  font_info.tmMaxCharWidth;
}

int  char_height(char znak)
/* Aktualne rozmiary znaku  */
{
int pom=raw_char_height()/muly;
return pom;
}

int  char_width(char znak)
/* potrzebne do pozycjonowania tekstu */
{
int pom=raw_char_width()/mulx;
return pom;
}

int  string_height(const char* str)
/* Aktualne rozmiary lancucha */
{
SIZE sizes;
if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
		return sizes.cy/muly;
		else
		return -1;
}

int  string_width(const char* str)
/* ...potrzebne do jego pozycjonowania */
{
SIZE sizes;
if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
		return sizes.cx/mulx;
		else
		return -1;
}

static void __cdecl special_close_plot() 
//¯eby mo¿na by³o swobodnie manipulowaæ sposobem wywo³ania iinnych funkcji
{
	close_plot();
}

int init_plot(int a,int b,int ca, int cb)
/* typowa dla platformy inicjacja grafiki/semigrafiki */
/* a,b to wymagane rozmiary ekranu w pixelach */
/* ca,cb - ile dodatkowych lini i kolumn na tekst (w znakach!) */
{
int ScreenRealH=0,ScreenRealW=0;
RECT rect;
if(WB_Instance==0)
			WB_Instance=GetModuleHandle(NULL);
ini_width=a;//MAXX+1 obszaru pixelowego
ini_height=b;//MAXY+1 obszaru pixelowego
ini_col=ca;//Ile kolumn tekstu dodatkowo
ini_row=cb;//Ile wierszy tekstu dodatkowo
SetScale();
W_width=a;W_height=b;// Zeby cos bylo

if(!InitWindowClass()||
	!InitMainWindow())
	{
	assert("Window initialisation"==NULL);
	return /*FALSE*/0;
	}

atexit(special_close_plot);
//ShowWindow(MyHwnd, SW_SHOW); // DEBUG

// Resize
GetRealScreen();//Zeby zaladowac niezbedna informacje o czcionce
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

//Trzeba policzyc obszar na menu jesli jest takowe
{
HMENU MyMenu=GetMenu(MyHwnd);
AdjustWindowRect(&rect,MY_WIN_STYLE,MyMenu!=NULL);
}

//Teraz trzeba uaktualnic rozmiary okna
{
char buf[128];
//Liczymy rozmiary ze wzgledu na zadany obszar roboczy oraz ewentualne menu
int real_W_height=abs(rect.top-rect.bottom);
int real_W_width=abs(rect.left-rect.right);
//Ale nie moze byc wieksze niz rozmiar ekranu!
//if(ForceHeight==0)//I tak nie mo¿na powiêkszyæ poza ekran - takie okno nie postaje...
if(real_W_height>ScreenRealH)
		real_W_height=ScreenRealH;
//if(ForceWidth==0)
if(real_W_width>ScreenRealW)
		real_W_width=ScreenRealW;

//Ustawiamy wg tego co obliczylo AdjustWindowRect()
SetWindowPos(  MyHwnd,
					HWND_TOP,0,0,
					real_W_width,real_W_height,
					SWP_NOMOVE | SWP_NOCOPYBITS);


//O ile okno jest mniejsze od ekranu
if(real_W_height<=ScreenRealH && real_W_width<=ScreenRealW)
{
    int cur_Cl_height,cur_Cl_width;
    //Sprawdzenie czy wyszlo, bo poprzednia funkcja nie uwzglednia zawijania menu
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

//Zapisujemy co wyszlo!
WritePrivateProfileString("WINDOW","Height",_ltoa(real_W_height,buf,10),"./SYMSHELL.INI");
WritePrivateProfileString("WINDOW","Width",_ltoa(real_W_width,buf,10),"./SYMSHELL.INI");
}


// Make the window visible; update its client area
ShowWindow(MyHwnd, SW_SHOW); // Show the window
WindowClosed=0;// WIndow open flag for close_plot
//if(!is_mouse)
//      SetCursor(LoadCursor(NULL, IDC_NO));
FreeResources(); //Bo moga sie zaladowac konteksty dla zlego ekranu

PostMessage(MyHwnd,WM_PAINT,0,0); // Sends WM_PAINT message
if(is_mouse)
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      else
      SetCursor(LoadCursor(NULL, IDC_NO));

return /*TRUE*/1;
}

void flush_plot()
/* uzgodnienie zawartosci ekranu  */
{
	if(trace_level & 2)
		fprintf(stderr," FLUSH PLOT ");
	
    if(MyHdc!=0)//Byl uzywany. Zwalnia tylko RelaseResources
    {
        if(animate && VirtualScreen && MyHdc==MbHdc )//Animujemy i jest juz bitmapa
        {
			HDC ScreenDc=GetDC(MyHwnd);
			if(trace_level & 2)
                fprintf(stderr,"-> BITBLT ");
            BitBlt(ScreenDc,0,0,W_width*mulx,W_height*muly,MyHdc,0,0,SRCCOPY);
            ReleaseDC(MyHwnd,ScreenDc);
        }
		if(trace_level & 2)
            fprintf(stderr,"-> RELEASE DC ");
        ReleaseDC(MyHwnd,MyHdc);
		curr_color=curr_fill=-1;
        MyHdc=0;//!!!
    }

	if(DelayTime>0)
		Sleep(DelayTime);//Uœpiene programu na pewien czas

    if(trace_level & 2)
		fprintf(stderr," * \n");
}

void	delay_ms(unsigned ms)
/* Wymuszenie oczekiwania przez pewn¹ liczbê ms */
{
	Sleep(ms);//Uœpiene programu na pewien czas
}

/* GETTING INPUT */
//zmienne do komunikacji ProcesMsg z procedurami obslugi komunikatow
static int InputChar=0;
static int InputXpos;
static int InputYpos;
static int InputClick;
static int MouseInput=0; //Flaga sygnalizuj¹ca, ¿e sa nieodczytane dane myszowe

static int ProcessMsg(int peek)
//Przerabia wejscie i zwraca znak:
// EOF jesli koniec, 0 jesli nic do wziecia lub inny
{
BOOL msg_avail=0;
int ret;

if(trace_level & 1)
	fprintf(stderr,"* PrcsMsg(p=%d) ",peek);

InputChar='\0';
// Acquire and dispatch messages until a WM_QUIT message is received.
JESZCZE_RAZ:
if(peek)
{
	if(trace_level & 1)
		fprintf(stderr,"[");
		
	msg_avail=PeekMessage(&msg,/*NULL*/MyHwnd,0,0,PM_REMOVE);
		
	if(trace_level & 1)
		fprintf(stderr,"%d]",msg_avail);
}
else
{
	if(trace_level & 1)
		fprintf(stderr,"{");
		
	msg_avail=GetMessage(&msg,/*NULL*/MyHwnd, 0, 0);
		
	if(trace_level & 1)
		fprintf(stderr,"%d}",msg_avail);
		
	if(msg_avail==FALSE && msg.hwnd==MyHwnd)
	{
		if(trace_level & 1)
			fprintf(stderr,"msg_avail=%d!",msg_avail);
		WindowClosed=1;// WIndow destroyed?
		InputChar='\0';
		return EOF;
	}
}

if(msg_avail==-1) /* ??? BOOL i -1 ???? */
{
	if(trace_level & 1)
		fprintf(stderr,"msg_avail=%d",msg_avail);
	if(error_proc()=='A') 
		abort();
 }
 
if(msg.hwnd!=MyHwnd)
	{	
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	if(trace_level & 1)
		fprintf(stderr,"* PrcsMsg GOTO \"ONES MORE\" ");
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

static int first_to_read=0;

int  input_ready()
/* (nie)zalezna od platformy funkcja sprawdzajaca czy jest wejscie */
{
	int input;
	if(trace_level & 1)
		fprintf(stderr,"* input_ready() ");
	
	if(CharToGet!=0||first_to_read!=0)//Nieodebrano
		return TRUE;

	input=ProcessMsg(1);//PEEK! Trzeba sprawdzic nowe komunikaty
	
	if(input!='\0')
	{CharToGet=input;return TRUE;}
	else
	{CharToGet='\0';return FALSE;}
}

int  set_char(int c)	
/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
{
	if(trace_level & 1)
		fprintf(stderr,"* set_char(%c) ",c);
		
	if(first_to_read!=0)//Jeszcze nieodebrano
		return 0;
	first_to_read=c;
	return 1;
}

int  get_char()
/* odczytywanie znakow sterowania, z oczekiwaniem jesli brak */
{
	int input;
	if(trace_level & 1)
		fprintf(stderr,"* get_char() ");
		
    if(first_to_read!=0) //Sprawdza czy nie ma zwrotow
    {
        input=first_to_read;
        first_to_read=0;
        return input;
    }
    
    if(CharToGet!=0)//Zwraca znak odczytany przez input_ready()
    {
        input=CharToGet;
        CharToGet='\0';
		return input;
    }

    while((input=ProcessMsg(0))=='\0');//sam musi poczekac na znak
    
	return input;
}

int  get_mouse_event(int* xpos,int* ypos,int* click)
/* Odczytuje ostatnie zdazenie myszy */
{
	if(trace_level & 1)
		fprintf(stderr,"* get_mouse_event() ");
		
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
void close_plot(void)
/* zamkniecie grafiki/semigrafiki */
{
	int ret=0;//Dla GetMessage
	
	if(trace_level & 2)
		fprintf(stderr," CLOSE PLOT ");

	if(!WindowClosed) // Jeszcze nie zamkniete wczeœniej & Valid HANDLE?
		{
		WindowClosed=1; //Zabezpieczenie przed powtórnym wywolaniem tej funkcji

		if(WB_error_enter_before_clean)
			{
			char* kom="Press OK to close the window";
			/* width,height of Window at this moment */
			//print(ini_width/2-(strlen(kom)*char_width('X'))/2,ini_height/2,kom);
			//      FILE
			//fprintf(stderr,"(See at window: %s )",window_name);
			if(IsWindow(MyHwnd)!=0) //IsWindow
				{
				if(MessageBox(MyHwnd,kom,window_name,MB_ICONQUESTION)==0)
					MessageBox(NULL,"Your window is probably closed",window_name,MB_ICONQUESTION);;
				}
				else
				MessageBox(NULL,"Your window already closed",window_name,MB_ICONQUESTION);
			//get_char();
			WB_error_enter_before_clean=0;
			}

		FreeResources();
		if(UserFont)
			{//Alokowane raz na program
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



/* PRINTING */
/************/
static char straznik1=0x77;
static char bufor[2048];
static char straznik2=0x77;
static int ox,oy;

void printbw(int x,int y,const char* format,...)       // int x,int y,const char* format,...
/* ---//--- wyprowadzenie tekstu na ekran */
{
HDC MyHdc;
RECT rc;
size_t len;//Bedzie przypisane dlugoscia bufora
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

  /* Print string in window */
  /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    MyHdc=GetMyHdc();
    font_height = font_info.tmHeight;
    font_width = font_info.tmAveCharWidth;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

//    ox=x;oy=y;
	rc.left=x;rc.top=y;
	rc.right=x+font_width*(len+1);
	rc.bottom=y+font_height;
	//FillRect (MyHdc, &rc, GetMyBrush(255));

    SetTextColor(MyHdc,colors[0]);
    SetBkColor(MyHdc,colors[255]);

//if(!animate)
    TextOut(MyHdc,x,y,bufor,len);
//if(is_buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}

void printc(int x,int y,
    ssh_color fore,ssh_color back,
    const char* format,...)
{
HDC MyHdc;
//RECT rc;
size_t len;//Bedzie przypisane dlugoscia bufora
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
                assert(fore<PALETE_LENGHT);
                assert(back<PALETE_LENGHT);
    MyHdc=GetMyHdc();
	//font_height = font_info.tmHeight;
	//font_width = font_info.tmAveCharWidth;
	x*=mulx; /* Multiplicaton of coordinates */
	y*=muly; /* if window is bigger */

	/* Output text, centered on each line */
	//rc.left=x;
	//rc.top=y;
	//rc.right=x+font_width*(len+1);
	//rc.bottom=y+font_height;
	//FillRect (MyHdc, &rc, GetMyBrush(back));

	SetTextColor(MyHdc,colors[fore]);
    SetBkColor(MyHdc,colors[back]);
//if(!animate)
    TextOut(MyHdc,x,y,bufor,len); // SetBkMode(hdc, TRANSPARENT)???
//if(is_buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}


int  print_transparently(int yes)
/* Wlacza mozliwosc drukowania tekstu bez zamazywania t³a. Zwraca stan poprzedni */
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
void plot(int x,int y,ssh_color color)
/* wyswietlenie punktu na ekranie */
{
    assert(color<PALETE_LENGHT);    

x*=mulx; /* Multiplicaton of coordinates */
y*=muly; /* if window is bigger */
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

void plot_rgb(int x,int y,int r,int g,int b)
/* wyswietlenie punktu na ekranie */
{
COLORREF color;
color=RGB(r,g,b);

x*=mulx; /* Multiplicaton of coordinates */
y*=muly; /* if window is bigger */
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
			return;//granica, albo poza dozwolonym obszarem ekranu
	SetPixelV(hdc,x,y,R_fill);
	//Seeding
	_fill_seed(hdc,x+1,y,R_fill,R_border);
	_fill_seed(hdc,x-1,y,R_fill,R_border);
	_fill_seed(hdc,x,y+1,R_fill,R_border);
	_fill_seed(hdc,x,y-1,R_fill,R_border);
}

void fill_flood(int x,int y,ssh_color fill,ssh_color border)
//Wypelnia powodziowo lub algorytmem siania
{
    COLORREF R_fill,R_border;

                    assert(fill<PALETE_LENGHT);
                    assert(border<PALETE_LENGHT);
	 
    R_fill=colors[fill];
    R_border=colors[border];
	x*=mulx; /* Multiplicaton of coordinates */
	y*=muly; /* if window is bigger */
	_fill_seed(GetMyHdc(),x,y,R_fill,R_border);
}

void fill_flood_rgb(int x,int y,
				int rf,int gf,int bf,int rb,int gb,int bb)
//Wypelnia powodziowo lub algorytmem siania
{
    COLORREF R_fill,R_border;              
	 
    R_fill=RGB(rf,gf,gf);
    R_border=RGB(rb,gb,gb);
	x*=mulx; /* Multiplicaton of coordinates */
	y*=muly; /* if window is bigger */
	_fill_seed(GetMyHdc(),x,y,R_fill,R_border);
}

void fill_rect(int x1,int y1,int x2,int y2,ssh_color color)
{
RECT rc;
            assert(color<PALETE_LENGHT);

x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
y1*=muly;y2*=muly;  /* if window is bigger */
rc.left=x1;rc.top=y1;
rc.right=x2;
rc.bottom=y2;
FillRect (GetMyHdc(), &rc, GetMyBrush(color));/* Bez dolnego wiersza i prawej kolumny!!! */
}


void fill_rect_d(int x1,int y1,int x2,int y2)
/* Wyswietla prostokat w kolorach domyslnych*/
{
RECT rc;          
x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
y1*=muly;y2*=muly;  /* if window is bigger */
rc.left=x1;rc.top=y1;
rc.right=x2;
rc.bottom=y2;
FillRect (GetMyHdc(), &rc,curent_brush);/* Bez dolnego wiersza i prawej kolumny!!! */
}

void line(int x1,int y1,int x2,int y2,ssh_color color)
/* wyswietlenie lini */
{
    POINT points[2];        assert(color<PALETE_LENGHT);
    plot(x2,y2,color);
    plot(x1,y1,color);
    
    x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
    y1*=muly;y2*=muly;  /* if window is bigger */
    
	if(curr_color!=color /*???*/)
    {
        HPEN MyPen=GetMyPen(color,LineWidth,LineStyle);
        SelectObject(GetMyHdc(),MyPen);
    }
    
    points[0].x=x1;points[0].y=y1;
    points[1].x=x2;points[1].y=y2;
    Polyline(GetMyHdc(),points,2);
}

void line_d(int x1,int y1,int x2,int y2)
/* Wyswietlenie lini w kolorze domyslnym */
{
    POINT points[2];       
    plot_rgb(x2,y2,GetRValue(curent_pen_rgb),GetGValue(curent_pen_rgb),GetBValue(curent_pen_rgb));
    plot_rgb(x1,y1,GetRValue(curent_pen_rgb),GetGValue(curent_pen_rgb),GetBValue(curent_pen_rgb));
    
    x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
    y1*=muly;y2*=muly;  /* if window is bigger */
      
    points[0].x=x1;points[0].y=y1;
    points[1].x=x2;points[1].y=y2;
    Polyline(GetMyHdc(),points,2);
}

void circle(int x,int y,int r,ssh_color color)
/* Wyswietlenie okregu w kolorze c */
{
    int r1,r2;      assert(color<PALETE_LENGHT);
    
    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */
	if(curr_color!=color /*???*/)
    {
        HPEN MyPen=GetMyPen(color,LineWidth,LineStyle);
        SelectObject(GetMyHdc(),MyPen);
    }
    Arc(GetMyHdc(),x-r1,y-r2,x+r1,y+r2,x,y+r1,x,y+r1);
}

void circle_d(int x,int y,int r)
/* Wyswietlenie okregu w kolorze domyslnym */
{
    int r1,r2;    
    
    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */
   
    Arc(GetMyHdc(),x-r1,y-r2,x+r1,y+r2,x,y+r1,x,y+r1);
}

void fill_circle(int x,int y,int r,ssh_color color)
/* KOlo w kolorze color */
{
    int r1,r2;      assert(color<PALETE_LENGHT);
    
    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */
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
    Ellipse(GetMyHdc(),x-r1,y-r2,x+r1,y+r2);
}

void fill_circle_d(int x,int y,int r)
/* Wyswietlenie kola w kolorach domyslnych*/				
{
    int r1,r2;     
    
    x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
    r1=r*mulx;r2=r*muly;  /* if window is bigger */
    
    Ellipse(GetMyHdc(),x-r1,y-r2,x+r1,y+r2);
}

void fill_poly(int vx,int vy,
					const ssh_point points[],int number,
					ssh_color color)
/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
{
static POINT _LocalTable[10];
POINT* LocalPoints=_LocalTable;
int i;

if(number<=2)
		return; //Nie da sie rysowac wielokata o dwu punktach lub mniej

if(number>10) //Jest za duzy.Alokacja
	LocalPoints=calloc(number,sizeof(POINT));

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

if(number>10) //Byl duzy
	free(LocalPoints);
}

void fill_poly_d(int vx,int vy, const ssh_point points[],int number)
/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
{
static POINT _LocalTable[10];
POINT* LocalPoints=_LocalTable;
int i;

if(number<=2)
		return; //Nie da sie rysowac wielokata o dwu punktach lub mniej

if(number>10) //Jest za duzy.Alokacja
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

if(number>10) //Byl duzy
	free(LocalPoints);
}


void clear_screen()
/* Czysci ekran przed zmiana zawartosci */
{
RECT rc;
if(trace_level & 2)
	fprintf(stderr,"CLEAR SCREEN\n");
GetClientRect(MyHwnd, &rc);
rc.right++; //Niweluje dziwny sposob wypelniania FillRect
rc.bottom++;//pomijajacy dolny wiersz i skrajnie prawa kolumne
FillRect (GetMyHdc(), &rc, GetMyBrush(Background));
}


// MESSAGE SUPPORT COPIED FROM BORLAND SAMPLE
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
//    structure.  In either case, return the value recieved from the
//    message or default function.
//

static LRESULT DispMessage(LPMSDI lpmsdi,
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

static LRESULT DispDefault(EDWP   edwp,
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



static BOOL InitApplication(HINSTANCE hInstance)
{
    #ifdef __WIN16__
    WNDCLASS  wc;
    #else
    WNDCLASSEX wc;
    #endif
	static int AppInitialised=0;
	if(AppInitialised==1)
		return TRUE; //Juz zostala zainicjalizowana

    // Load the application name  strings.
    if(LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName))==0)		
		NoResources=1;//Nie podlaczono zasobow
		else
        {
		    NoResources=0;//Sa zasoby podlaczono zasobow
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
	wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // Class style(s).
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
			sprintf(szClassName,"SSHx%xCLASS",hInstance);
		}
		else
		{
			sprintf(szClassName,"SSH_CLASS_%s",szAppName);
		}

	wc.lpszClassName = szClassName; // Name to register as

	// Register the window class and return FALSE if unsuccesful.
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
	//Module specific application initialization functions or so...
	//
	AppInitialised=1;
	return TRUE;

FAIL:
	{
	DWORD err=GetLastError();
	char buf[1024];
	sprintf(buf,"Can't registry window class! code:%ld",err);
	MessageBox(0,buf,"Unexpected error",MB_ICONSTOP);
    return FALSE; 
	}
}


static BOOL InitInstance(HINSTANCE hInstance)
{

    // Create a main window for this application instance.
    MyHwnd = CreateWindow(szClassName,           // See RegisterClass() call.
                        window_name,           // Text for window title bar.
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
    // **TODO** Call module specific instance initialization functions here.
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
//    {WM_LBUTTONDBLCLK,  MsgLButtonDoubleClick},
    {WM_RBUTTONDOWN,    MsgRButtonDown},
//    {WM_RBUTTONUP,      MsgRButtonUp},
//    {WM_RBUTTONDBLCLK,  MsgRButtonDoubleClick},
//    {WM_KEYDOWN,        MsgKeyDown},
//    {WM_KEYUP,          MsgKeyUp},
    {WM_CHAR,           MsgChar},
//    {WM_TIMER,          MsgTimer},
//    {WM_HSCROLL,        MsgScroll},
//    {WM_VSCROLL,        MsgScroll},
    {WM_PAINT,          MsgPaint}
};

static MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};


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

static LRESULT CALLBACK WndProc
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

static LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	LRESULT lRet = 0;
	// Processing WM_COMMAND
    // Message packing of wparam and lparam have changed for Win32,
    // so use the GET_WM_COMMAND macro to unpack the command
	WORD cmd=GET_WM_COMMAND_ID(wparam,lparam);

	if(trace_level & 1)
		fprintf(stderr,"WM_COMMAND MSG\n");
	
	if(cmd==IDM_SIGNAL_COMMAND)//signals symulation
		{
		raise(lparam);//Not HWND in this case!
		}
		else
		if(cmd>=MINUSERCOMMAND)
			{
			if(cmd==IDM_EXIT)
				InputChar=EOF; //Send EOF to symshell
				else
				InputChar=cmd; //Send cmd value to symmshell

			}
			else
				lRet = DefWindowProc(hwnd, uMessage, wparam, lparam);

    return lRet;
}

// Handler for WM_SYSCOMMAND
static LRESULT MsgSysCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LRESULT lRet = 0;

    // Message packing of wparam and lparam have changed for Win32,
	// so use the GET_WM_COMMAND macro to unpack the commnad
	if(trace_level & 1)
		fprintf(stderr,"WM_SYSCOMMAND MSG\n");

	switch (GET_WM_COMMAND_ID(wparam,lparam))
    {
        //
        // **TODO** Add cases here for application specific command messages.
        //

		case SC_CLOSE:
			if(trace_level & 1)
				fprintf(stderr,"   SC_CLOSE SYS COMMAND\n");
			InputChar=EOF; //Send EOF to symshell
			break;
//      case SC_SIZE:
//        		InputChar='\0';
// 				USE DEFAULT SUPPORT!   don't break; or move this steatment
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

static LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    // Set the timer for five-second intervals
//    idTimer =  SetTimer(hwnd, TIMERID, 5000, NULL);
	 if(trace_level & 1)
		fprintf(stderr,"CREATE MSG\n");
	 return 0;
}



static LRESULT MsgClose(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	 if(trace_level & 1)
		fprintf(stderr,"CLOSE WND. MSG\n");
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


static LRESULT MsgGetMinMaxInfo(HWND   hwnd,
						 UINT   uMessage,
                         WPARAM wparam,
                         LPARAM lparam)
{
if(trace_level & 1)
	fprintf(stderr,
		  "MsgGetMinMaxInfo: %x, %d, %d\n",
		  wparam, LOWORD(lparam), HIWORD(lparam)
			);
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


static LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	if(trace_level & 1)
		fprintf(stderr,"DESTROY MSG\n");
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


static LRESULT MsgLButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
WORD xPos = LOWORD(lparam);  // horizontal position of cursor
WORD yPos = HIWORD(lparam);  // vertical position of cursor

if(is_mouse)
	{
   if(trace_level & 1)
	fprintf(stderr,
		  "WM_LBUTTONDOWN: %x, %d, %d\n",
		  wparam, LOWORD(lparam), HIWORD(lparam)
			);
	   InputXpos=xPos;
		InputYpos=yPos;
		InputClick=1;
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


static LRESULT MsgLButtonDoubleClick
	 (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace_level & 1)
	 fprintf(stderr,
		  "WM_LBUTTONDBLCLK: %x, %d, %d\n",
		  wparam, LOWORD(lparam), HIWORD(lparam)
	);
	//InputChar='\xff';
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


static LRESULT MsgRButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
WORD xPos = LOWORD(lparam);  // horizontal position of cursor
WORD yPos = HIWORD(lparam);  // vertical position of cursor
if(is_mouse)
	{
		if(trace_level & 1)
			fprintf(stderr,
			  "WM_RBUTTONDOWN: %x, %d, %d\n",
			wparam, LOWORD(lparam), HIWORD(lparam)
			);

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


static LRESULT MsgRButtonDoubleClick
	 (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace_level & 1)
	 fprintf(stderr,
        "WM_RBUTTONDBLCLK: %x, %d, %d\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );
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


static LRESULT MsgKeyDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace_level & 1)
    fprintf(stderr,
        "WM_KEYDOWN: %x, %x, %x\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );

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


static LRESULT MsgKeyUp(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace_level & 1)
    fprintf(stderr,
        "WM_KEYUP: %x, %x, %x\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );
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


static LRESULT MsgChar(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace_level & 1)
	fprintf(stderr,
		"WM_CHAR: %c, %x, %x\n",
		wparam, LOWORD(lparam), HIWORD(lparam)
			);
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


static LRESULT MsgTimer(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
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
if(trace_level & 1)
	fprintf(stderr,
		"WM_TIMER: %c, %x, %x\n",
		wparam, LOWORD(lparam), HIWORD(lparam)
			);
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


static LRESULT MsgPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
 //   RECT rect;
	HDC  TempHdc;
	LONG left,top,right,bottom;

	if(trace_level & 1)
		fprintf(stderr," REPAINT MSG ");

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
		
		if(trace_level & 1)
			fprintf(stderr,"-> BITBLT ");
		}
		else   //Nie ma tej treœci na bitmapie
		{
			if(trace_level & 1)
				fprintf(stderr,"-> FORCE REPAINT ");	
		
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

			if(trace_level & 1)
			{
				fprintf(stderr," %ld %ld %ld %ld ",
					forrepaint.left,
					forrepaint.top,
					forrepaint.right,
					forrepaint.bottom);
			}
		}

	EndPaint(hwnd, &ps);
	if(trace_level & 1)
		fprintf(stderr," * \n");
	return 0;
}

int repaint_area(int* x,int* y,int* width,int* height)
{
if(repaint_flag==1)
	{
	if(trace_level & 2)
		fprintf(stderr," Repaint_area() from %d,%d - %dx%d pixels\n",x,y,width,height);
		
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
	return -1;//Nie bylo nic odczytania (?)
}


static LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  //WORD	fwSizeType = wparam;      // resizing flag
  WORD	nWidth = LOWORD(lparam);  // width of client area
  WORD	nHeight = HIWORD(lparam); // height of client area
  WORD  n_mulx=0,n_muly=0;

  if((trace_level & 1)||(trace_level & 2))
	fprintf(stderr,"MSG RESIZE to %u %u \n",nWidth,nHeight);
  
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


  if(Flexible || n_mulx!=mulx || n_muly!=muly)// Rozmiar faktycznie sie zmienil
  	  {
	  if(!Flexible)
		{
		W_width=ini_width+(ini_col*raw_char_width())/n_mulx;
		W_height=ini_height+(ini_row*raw_char_height())/n_muly;
		mulx=n_mulx;muly=n_muly; 
		if(trace_level & 2)
			fprintf(stderr,"RES. MULT. X=%u Y=%u ",mulx,muly);
		}
		else
		{
		W_width=nWidth;
		W_height=nHeight;
		mulx=n_mulx;muly=n_muly; 
		if(trace_level & 2)
			fprintf(stderr,"RES. FLEXIBLE ");
		}
	  
	  FreeResources();//DC , Pens , Brushes
	  assert(MbHdc==0);
	  assert(VirtualScreen==NULL);
	  if(animate)
			{
			if(trace_level & 2)
				fprintf(stderr,"-->FORCE REPAINT");
			InputChar='\r';
			}
	  }

  if(trace_level & 2)
	fprintf(stderr,"\n");

  return 0;
}



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

static BOOL InitInput(HWND hwnd)
{
    /*HDC hdc;
    TEXTMETRIC tm;
    RECT rect;
    int  dyLine;
*/
	if(trace_level & 1)
		fprintf(stderr," InitInput() ");
		
   return TRUE;
}

//
//	Blok tworzenia zrzutu ekranu
//  z przykladow Microsoftu
//
static jmp_buf dump_jumper;/* JUMP bufor na wypadek bledu */

void errhandler(const char* text,HWND hwnd,int Errnum)
{
MessageBox(hwnd,text,"Window capture error",MB_ICONERROR);
if(errno==0 && Errnum>0)//Ma ustawic Errno
	errno=Errnum;
longjmp(dump_jumper,Errnum);
//Tu nigdy nie powinien wejsc
exit(Errnum);//Debug
}

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
         errhandler("GlobalAlloc", hwnd, ENOMEM); 
 
    /* 
     * Retrieve the color table (RGBQUAD array) and the bits 
     * (array of palette indices) from the DIB. 
     */ 
 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, 
                   lpBits, pbi, DIB_RGB_COLORS)) 
        errhandler("GetDIBits", hwnd , EINVAL );//errno 
 
    /* Create the .BMP file. */ 
 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                   (LPSECURITY_ATTRIBUTES) NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
 
    if (hf == INVALID_HANDLE_VALUE) 
        errhandler("CreateFile", hwnd , EBADF ); 
 
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
           errhandler("CloseHandle", hwnd, EINVAL ); 
 
    /* Free memory. */

    GlobalFree((HGLOBAL)lpBits);
	lpBits=NULL;
}
 

int  dump_screen(const char* Filename)
/* Zrzuca ekran na plik o extensji zaleznej od platformy. -1 jesli sie nie uda. */
{
    HDC hdcScreen = GetMyHdc();
    HBITMAP hbmScreen=0;
    HDC hdcCompatible=0;
    PBITMAPINFO info;
    char bufor[1024];
    
    assert(strlen(Filename)+5<1024);
    sprintf(bufor,"%s.BMP",Filename);
    
	if((trace_level & 2))
        fprintf(stderr,"DUMP SCREEN TO FILE %s ", bufor);
    
    errno=0; /* Kasowanie smieci */
    if(setjmp(dump_jumper)!=0) //Awaria
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
		if(trace_level & 2)
			fprintf(stderr,"WRITE FAILED \n");
		return -1;//B£¥D!
    }
    
    /* GRABBING WINDOW */
	hdcCompatible = CreateCompatibleDC(hdcScreen); 
    
    /* Create a compatible bitmap for hdcScreen. */ 
    
    hbmScreen = CreateCompatibleBitmap(hdcScreen, W_width, W_height );
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
        W_width, W_height, 
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
	if(trace_level & 2)
		fprintf(stderr,"Write OK \n");
	return 0; //Czy blad?
}

void shell_setup(const char* title,int iargc,char* iargv[])
/* Przekazanie parametrow wywolania */
{
int i;
int largc=iargc;
char** largv=iargv;
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
			   "\t -help\n"
			   "\t -logo\n"
			   );
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
			printf("Buffered is %s\n",(is_buffered?"ON":"OFF"));
		//if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
		//	is_buffered=1;/* zeby mozna bylo na nia pisac */
			is_buffered=animate;/*Albo,albo.NIe jak w Xwindow */
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
	}
	fflush(stdout);
}

if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
	is_buffered=1;/* zeby mozna bylo na nia pisac */

window_name = title;
icon_name = title;
fflush(stdout);
}

/********************************************************************/
/*			          WBRTM  light version 2009                     */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/


