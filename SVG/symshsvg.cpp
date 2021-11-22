/** SYMSHELL - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++     **/
/*************************************************************************/
/** Simplest graphics interface implemented for SVG vector graphic       */
/** designed by W.Borkowski from University of Warsaw                    */
/**                                                                      */
/** https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI              */
/** https://github.com/borkowsk                                          */
/** SVG IMPLEMENTATION in file 'symshsvg.cpp'                            */
/** File changed massivelly: 17.11.2020                                  */
/*************************************************************************/
/**               SYMSHELLLIGHT  version 2021-11-22                     **/
/*************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cmath>
#include <cstdio>

#ifndef MAX_PATH
#define MAX_PATH 4000
#endif

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#define _getpid   getpid
#endif

#include "symshell.h"
#define HIDE_WB_PTR_IO	1     //I/O NIEPOTRZEBNY
#include "wb_ptr.hpp"

using namespace std;
using namespace wbrtm;

/// Wewnętrzne śledzenie wywołań
//////////////////////////////////////////////////
#if defined( _MSC_VER )
//#define STR_HELPER(x) #x
//#define STR(x) STR_HELPER(x)
//#define _FUNCTION_NAME_  ("SYMSHSVG_" STR( __LINE__ ) )
#define _FUNCTION_NAME_  __FUNCTION__ 
#else
#define _FUNCTION_NAME_  __func__ //C11
#endif

/// Zmienne eksportowane na zewnątrz
///////////////////////////////////////////////////////
unsigned long     PID=_getpid();
int         ssh_trace_level = 1;           //Maska poziomów śledzenia 1-msgs 2-grafika 3-grafika detaliczna 4-alokacje/zwalnianie
double      INITIAL_LENGH_RATIO = 0.001;   //Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu)
double      MAXIMAL_LENGH_RATIO = 0.999;   //Ile maksymalnie rekordów jest dopuszczalnych?//(mnożone przez liczbę pikseli ekranu)

const char* GrFileOutputByExtension = "svg";//Tym można ustalać format pliku wyjściowego.
                                            // Jak extension nieznane, to wyrzuca strumień obiektowy '.str'
const char* GrTmpOutputDirectory = "./";    // Ścieżka, gdzie ma wrzucać zrzuty tymczasowe

unsigned    GrReloadInterval = 1000;        //Co ile czasu skrypt w pliku SVG wymusza przeładowanie. Jak 0 to w ogóle nie ma skryptu.
bool        GrMouseActive = false;          //Myszy domyślnie nie ma, ale inny moduł może ją symulować przez linkowanie do tych zmiennych globalnych
int         GrCharMessage = -2;             //Nie ma też klawiatury, ale  ------------------------------//------------------------------------------
int         GrMouseX = -1;
int         GrMouseY = -1;
int         GrMouseC = -1;

//Jakby się chrzaniło, to inny moduł może to zmienić lub od/za blokować
int         GrPrintTransparently = 0;
int         GrLineWidth = 1;
int         GrLineStyle = SSH_LINE_SOLID;

ssh_rgb     GrPenColor = { 255,255,255};//,255 };
ssh_rgb     GrBrushColor = { 205,205,205};//,255 };

unsigned long    _ssh_window=0;  /* Dummy handler for check and external use */

const char* SEP = "\t";

/// Zmienne 'static' czyli bez dostępu z zewnątrz modułu
//////////////////////////////////////////////////////////
static const char*  ScreenTitle = "SSHSVG"; //Nazwa "okna" czyli domyślnego pliku generowanego przez flush_plot()
static char ScreenHeader[1024]="SSH SVG WINDOW";//Domyślny nagłówek pliku

static ssh_color    curr_background = 0;
static unsigned     GrScreenHi = 0;
static unsigned     GrScreenWi = 0;
static unsigned     GrFontHi = 14;
static unsigned     GrFontWi = 6;
static ssh_rgb      palette[512];
static int          UseGrayScale = 0;  //Flaga użycia skali szarości - np. do wydruków
                                       //Ustawiana jako parametr wywołania programu
                                       //podobnie jak opcje śledzenia i buforowania,
                                       //ale dla skali kolorów to jedyny sposób na
                                       //włączenie
static bool         GrClosed = true;   //Czy grafika już/jeszcze ZAMKNIĘTA?

/// IMPLEMENTACJA
//////////////////////

enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };

struct Empty { unsigned type : 4; unsigned mode : 4; };
struct Point { unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rb : 8;  unsigned gb : 8; unsigned bb : 8; };
struct LineTo{ unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned w : 8; };
struct Line  { unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x1 : 16; unsigned y1 : 16; unsigned x2 : 16; unsigned y2 : 16; unsigned w : 8; };
struct Circle{ unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rx : 16; unsigned ry : 16; unsigned rf : 8; unsigned gf : 8; unsigned bf : 8; unsigned w : 8;};
struct Rect  { unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x1 : 16; unsigned y1 : 16; unsigned x2 : 16; unsigned y2 : 16; unsigned rf : 8; unsigned gf : 8; unsigned bf : 8; unsigned w : 8;};
struct Text  { unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rf : 8; unsigned gf : 8; unsigned bf : 8; char* txt; };
struct Poly  { unsigned type : 4; unsigned mode : 4; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned rf : 8; unsigned gf : 8; unsigned bf : 8; unsigned w : 8; ssh_point* points; unsigned size; };

union GrOperation //Struktura do przechowywania operacji rysowania
{
    struct Empty  empty;
    struct Point  point;
    struct LineTo lineTo;
    struct Line   line;
    struct Circle circle;
    struct Rect   rect;
    struct Text   text;
    struct Poly   poly;

    GrOperation()
    {
        memset(this, 0, sizeof(GrOperation));//Wypełnianie zerami
                                                assert(empty.type == GrType::Empty);
                                                assert(text.txt == NULL);//Powinno być oczywiste, ale...
                                                assert(poly.points == NULL);
    }

    GrOperation(GrOperation& From)
    {
        memcpy(this,&From, sizeof(GrOperation));
        if (empty.type == GrType::Text || empty.type == GrType::Poly)
            memset(&From, 0, sizeof(GrOperation));//Wypełnianie zerami
    }

    void clean()//Czyści stare operacje, np. gdy uznano, że efekt i tak jest zasłonięty, albo clear_screen(), albo end.
    {
        if (empty.type == GrType::Text && text.txt!=NULL )
            delete text.txt;
        else if (empty.type == GrType::Poly && poly.points!=NULL)
            delete [] poly.points;
        empty.type = GrType::Empty;
    }

    ~GrOperation() //Jeżeli jest zapisany obiekt z danymi dynamicznymi to trzeba zwolnić
    {
        clean();
    }
};

static wb_dynarray<GrOperation> GrList; //Lista operacji rysowania
static int GrListPosition = -1;         //Aktualna pozycja na liście
static int maxN=-1;                     //from MAXIMAL_LENGTH_RATIO;


static GrOperation&  NextGrListEntry()
{
    if (++GrListPosition < GrList.get_size())
    {
        return GrList[GrListPosition];//Zwraca dostęp do kolejnej operacji. Potem można wpisywać informacje
    }
    else //NIE MA MIEJSCA!!!
    {
        size_t N = GrList.get_size() * 2;
        if(N>maxN) N=maxN;//Nie więcej niż maxN

        if(N<maxN)//Jeszcze można powiększyć
        {
            //Przy powiększaniu nie chcemy użyć "expand", bo to by wywoływało destruktory i kopiowanie!
            size_t oldsize;
            GrOperation* RawPtr = GrList.give_dynamic_ptr_val(oldsize);         assert(GrList.get_size() == 0);

            GrList.alloc(N);//Nowy bufor w powiększonym rozmiarze
            memcpy(GrList.get_ptr_val(), RawPtr, oldsize*sizeof(GrOperation));//Przekopiowanie realnej zawartości
            for (size_t i = 0; i < oldsize; i++)
                RawPtr[i].empty.type = GrType::Empty;//Wirtualne wyczyszczenie starego
            delete [] RawPtr; //Zwalnianie bez wywoływania możliwych istotnych destruktorów dla Text i Poly
        }
        else //Już nie można bardziej powiększyć bufora! Kasujemy pół najstarszej zawartości i przesuwamy
        {
            GrOperation* RawPtr = GrList.get_ptr_val();
            GrListPosition/=2;
            for(size_t i=0; i < GrListPosition;i++)
                RawPtr[i].clean();//Zwalniamy ewentualne składniki dynamiczne
            memmove(RawPtr,RawPtr+GrListPosition,GrListPosition*sizeof(GrOperation));
            size_t size=GrList.get_size();
            for(size_t i=GrListPosition;i<size;i++)
                RawPtr[i].empty.type = GrType::Empty;//Wirtualne wyczyszczenie zduplikowanej zawartości
        }

        return GrList[GrListPosition];//Zwraca dostęp do kolejnej operacji, czyli pierwszej za starej listy
    }
}

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */

void set_title(const char* window_name)
{
    strncpy(ScreenHeader,window_name,1023);
}

void shell_setup(const char* title,int iargc,const char* iargv[]) /* Obsługa parametrów wywołania programu */
{
    if(ssh_trace_level>0) //shell_setup
    {
        cout << "SVG: " << _FUNCTION_NAME_ << SEP;
        cout << title << SEP << iargc << endl;
        for (int i = 0; i < iargc; i++)
            cout << iargv[i] << SEP;
        cout << endl<< "SVG: struct size:" << SEP << sizeof(GrOperation) << endl;  //assert(sizeof(GrOperation) == 16);???
    }

    ScreenTitle = title;

    for (int i = 1; i < iargc; i++)
        if (iargv[i][0] == '-')
        {
            if (iargv[i][1] == 'F' || iargv[i][1] == 'e' || iargv[i][1] == 'f')
                GrFileOutputByExtension = iargv[i] + 2;
            else
                if (iargv[i][1] == 'D' || iargv[i][1] == 'd')
                    GrTmpOutputDirectory = iargv[i] + 2;
                else
                    if (iargv[i][1] == 'C' || iargv[i][1] == 'a' || iargv[i][1] == 'c')
                        GrCharMessage = atoi(iargv[i] + 2);
                    else
                        if (iargv[i][1] == 'R' || iargv[i][1] == 'r')
                            GrReloadInterval = atoi(iargv[i] + 2);
                        else
                            if (iargv[i][1] == 'B' || iargv[i][1] == 'b')
                                MAXIMAL_LENGH_RATIO = atof(iargv[i] + 2);
                            else
                                if (iargv[i][1] == 'T' || iargv[i][1] == 't')
                                    ssh_trace_level = atoi(iargv[i] + 2);
                                else
                                {
                                    cout<<"SVG graphics parameters:"<<endl;
                                    cout<<"-Fext - output format (svg or str)"<<endl;
                                    cout<<"-Dpath - output directory"<<endl;
                                    cout<<"-Cchar - single input character"<<endl;
                                    cout<<"-Rms - reload interval in SVG embeded script"<<endl;
                                    cout<<"-Bratio - maximal size of buffor as ratio of number of pixels"<<endl;
                                    cout<<"-Tint - trace level"<<endl;
                                    exit(-1);
                                    //cout<<"-x"<<endl;
                                }
        }
}

static void SetScale();  //Gdzieś tam jest funkcja ustalająca domyślną paletę kolorów indeksowanych

/// \brief ssh_stat init_plot() - początek pracy okna/ekranu graficznego (tu wirtualnego)
/// \param a
/// \param b
/// \param ca
/// \param cb
/// \return ssh_stat
/// \info inicjacja grafiki/semigrafiki
ssh_stat init_plot(ssh_natural  a, ssh_natural   b,                 /* ile pikseli mam mieć okno */
                   ssh_natural ca, ssh_natural  cb                  /* Ile linii i kolumn znaków marginesu */
               )
{
    //a to szerokość okna, b to wysokość
    //ca to miejsce na dodatkowe kolumny tekstu
    //cb to miejsce na dodatkowe wiersze tekstu
    //... liczone wg. rozmiaru używanego fontu (nie przewidziano używania różnych rozmiarów tekstu)
    atexit(close_plot);

    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP; //init_plot
    if(ssh_trace_level>1) cout << a << SEP << b << SEP << ca << SEP << cb << endl;

    SetScale(); //Ustawienie kolorów

    GrScreenWi = a + GrFontWi* ca;  assert(a>0);
    GrScreenHi = b + GrFontHi* cb;  assert(b>0);

    unsigned N = (unsigned)( ((a + ca)*(b + cb))*INITIAL_LENGH_RATIO );//Potem jest sprawdzane, czy nie za małe, warning zbędny
    maxN=(int)( ((a + ca)*(b + cb))*MAXIMAL_LENGH_RATIO );				//Potem jest sprawdzane, czy nie za małe, warning zbędny

    if(N<1 || maxN<1 || N>maxN )//N lub maxN, mniejsze od 1 to ewidentny błąd
    {
        cerr<<__FUNCTION__<<": WARNING! : "
            <<"((a + ca)*(b + cb))*INITIAL_LENGH_RATIO="
            <<"(("<<a<<" + "<<ca<<")*("<<b<<" + "<<cb<<"))*"<<INITIAL_LENGH_RATIO
            <<"="<<N<<endl;

        N=100;

        if(maxN<N)
            maxN=10000;
    }

    N+=N%2;maxN+=maxN%2;//Likwidacja ewentualnej nieparzystości, ważna przy przesuwaniu

    GrList.alloc(N);//Tu nie może być zero

    GrClosed = false;

    //_ssh_window=0;//&GrList;//???

    cerr<<GrScreenWi<<"x"<<GrScreenHi<<"-N:"<<N<<":"<<maxN<<endl;

    return 1;//OK
}

/// /brief void close_plot() - koniec pracy
/// zamkniecie grafiki/semigrafiki - tu grafiki wirtualnej zapisywanej do pliku
/// Automatycznie instalowana w atexit()
void close_plot()
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP; //close_plot
    if (GrClosed) return;

    if(ssh_trace_level>1) cout << "List lenght:"<<SEP<<GrList.get_size()<<SEP<<"USED:"<<SEP<< GrListPosition<<  endl;
    GrClosed = true;
    _ssh_window=0;
}

/// \brief void buffering_setup()
/// \param Yes or No
/// \info Przełączanie buforowanie okna. Może nie zadziałać wywołane po inicjacji
/// W grafikach rastrowych zawartość pojawia się na ekranie albo od razu, albo po wywołaniu "flush_plot"
/// Chodzi o lepszą jakość animacji. Jednak przy debuggingu lepiej widzieć w trakcie rysowania.
/// W module SVG aktualnie nie robi nic poza ewentualnym wyświetleniem na konsolę śladu użycia.
void buffering_setup(int Yes)
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;       //buffering_setup
    if(ssh_trace_level>1) cout << Yes <<SEP<<"IGNORED!"<< endl; //Nie ma sensu, bo nic nie jest wyświetlane w trakcie rysowania
}

/// Czy symulować niezmienność rozmiarów okna?
/// W takim trybie zmiana wielkości okna powiększa piksele o całkowitą wielokrotność
/// W tym module SVG aktualnie nie robi nic poza ewentualnym wyświetleniem na konsolę śladu użycia.
/// Jak plik graficzny to funkcja nie ma znaczenia, bo go raczej nie zwiększamy spoza programu.
void fix_size(int Yes)
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fix_size
    if(ssh_trace_level>1) cout << Yes << SEP << "IGNORED!" << endl;//Nie ma sensu, bo zapis i tak jest wektorowy i nie ma okna
}

/// \brief void delay_ms() - zawieszenie wykonania programu na pewną liczbę milisekund
/// \param ms
/// Wymuszenie oczekiwania przez pewną liczbę ms. -
/// W module SVG nie ma sensu, bo taki program raczej działa w tle!
/// Jednak trochę robimy, bo mogło chodzić o szanse na przełączenie wątków
/// albo procesów.
void delay_ms(unsigned ms)
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */

    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//delay_ms
    if(ssh_trace_level>1) cout << ms << SEP << "usec->msec*10!" << endl;

    usleep(ms*10);
}

/// \brief void delay_us() - zawieszenie wykonania programu na pewną liczbę milisekund
/// \param us
/// Wymuszenie oczekiwania przez pewną liczbę ms. -
/// W module SVG nie ma sensu, bo taki program raczej działa w tle!
/// Jednak trochę robimy, bo mogło chodzić o szanse na przełączenie wątków
/// albo procesów.
void delay_us(unsigned us)
{
    extern int usleep(useconds_t usec);/* takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. */

    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//delay_us
    if(ssh_trace_level>1) cout << us << SEP << "ALWAYS 1 us!" << endl;

    usleep(1);
}

/** OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO /TU WIRTUALNEGO/
 ************************************************************** */

/// Ustala czy mysz ma byc obsługiwana. Zwraca poprzedni stan tego ustawienia
int mouse_activity(ssh_mode Yes)
{
    if(ssh_trace_level>0) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//mouse_activity
    if(ssh_trace_level>0) cout << Yes << endl;
    int old = GrMouseActive;
    //GrMouseActive = (Yes?true:false);//Bo kretyński warning TODO DEBUG
    return old;
}

/// Ustala index koloru do czyszczenia tła.
/// Domyślne tło okna. Może nie zadziałać po inicjacji.
void set_background(ssh_color c)
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_background
    if(ssh_trace_level>1) cout << (ssh_color)c << endl;
    curr_background = c;
}

/** Operacje przestawiania wlasnosci pracy okna graficznego
 * ********************************************************* */

/// Czyści ekran lub ekran wirtualny. Zależnie czy jest buforowanie czy nie.
/// Tu nawet nie ma ekranu wirtualnego.
/// CZYSZCZONA JEST LISTA.
void clear_screen()
{
    if(ssh_trace_level) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//clear_screen
    if(ssh_trace_level) cout << endl;
    for (size_t i = 0; i < GrList.get_size(); i++)
        GrList[i].clean();
    GrListPosition = -1; //Pusto
}

/// Czyszczenie optymalizujące. Specjalnie dla tego modułu. W innych nie robi nic.
/// Cały ekran/okno zostanie wirtualnie "wymazany"
int invalidate_screen()
{
    if(ssh_trace_level) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;
    clear_screen();
    if(ssh_trace_level) cout << endl;
    return 1;
}

/// \brief ssh_mode     print_transparently()
/// \param Yes
/// \return ssh_mode - stary tryb drukowania
/// \info Włącza drukowanie tekstu bez zamazywania tła/lub z. Zwraca ustawienie poprzednie.
ssh_mode     print_transparently(ssh_mode Yes)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//print_transparently
    if(ssh_trace_level>2) cout << Yes << endl;
	int old = GrPrintTransparently;
    GrPrintTransparently = Yes;
	return old;
}

/// Ustala szerokosc linii. Moze byc kosztowne (?). Zwraca ustawienie poprzednie.
ssh_natural     line_width(ssh_natural width)

{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//line_width
    if(ssh_trace_level>2) cout << width << endl;
	int old = GrLineWidth;
	GrLineWidth = width;
    if(GrLineWidth<=0) GrLineWidth=1;
	return old;
}

/// Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED
/// Zwraca ustawienie poprzednie.
ssh_mode    line_style(ssh_mode Style)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//line_style
    if(ssh_trace_level>2) cout << Style << endl;
    int old = GrLineStyle;
    GrLineStyle = Style;
    return  GrLineStyle;    //Zwraca poprzedni stan
}

/// Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT
/// Zwraca ustawienie poprzednie.
/// W module SVG nie robi nic.
ssh_mode    put_style(ssh_mode Style)
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//put_style
    if(ssh_trace_level>1) cout << Style << SEP << "IGNORED!" << endl;
    return  SSH_SOLID_PUT;  //Zwraca poprzedni stan
}

/// \brief set_rgb() - Zmienia definicje pojedynczego koloru indeksowanego
/// \param color - indeks koloru
/// \param r
/// \param g
/// \param b
/// Zmienia definicje koloru indeksowanego o indeksie 'color' w tabeli (palecie) kolorów.
/// Dozwolone indeksy 0..255 bo powyżej jest skala szarości.
/// Do logowania użycia korzysta z maski poziomów śledzenia: 1-msgs 2-grafika 4-alokacje/zwalnianie
/// extern int ssh_trace_level = 0;
void set_rgb(ssh_color color,ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    if ( (ssh_trace_level & 4) !=0)//alokacja/zwalnianie (4) //set_rgb
	{
		cout <<"SVG: " << _FUNCTION_NAME_ << SEP;
		cout << (ssh_color)color << SEP << r << SEP << g << SEP << b << SEP << endl;
	}
	palette[color] = RGB(r,g,b);
}

/// \brief void set_gray()
/// \param shade
/// \param intensity
/// Zmienia definicje pojedynczego odcienia szarości w palecie szarości.
/// Indeksy 0-255 mapowane na 256..511
/// Do logowania użycia korzysta z maski poziomów śledzenia: 1-msgs 2-grafika 4-alokacje/zwalnianie
/// extern int ssh_trace_level = 0;
void set_gray(ssh_color shade,ssh_intensity intensity)
{
    shade%=256;
    if ( (ssh_trace_level & 4) !=0)//alokacja/zwalnianie (4)
    {
        cout <<"SVG: " << _FUNCTION_NAME_ << SEP;
        cout << (ssh_color)shade << SEP << intensity << endl;
    }
    palette[256+shade] = RGB(intensity,intensity,intensity);
}

/// \brief void set_pen() - Ustala aktualny kolor linii za pomoca typu ssh_color
/// \param c
/// \param width
/// \param Style
/// Ustala aktualny kolor linii za pomocą typu ssh_color (koloru indeksowanego)
/// ssh_color c - jest jak dotąd zawsze traktowany jako indeks do tabeli kolorów*
/// w których pierwsze 256 ustala się wg. jakiejś skali,
/// a następne 256 są odcieniami szarości, domyślnie od czarnego do białego
/// Kolory indeksowane w symshwin i symshx11 kozystają z cache'owania systemowych pisaków,
/// Żeby było szybciej. A przynajmniej kiedyś dawało to wyraźny zysk.
/// *ssh_color - to int32 więc ma miejsce na tryb RGB, ale nigdy nie zostało to zaimplementowane
void set_pen(ssh_color c,ssh_natural width, ssh_mode Style)

{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_pen
    if(ssh_trace_level>2) cout << (ssh_color)c <<SEP<<width<<SEP<<  Style << endl;
    GrLineWidth = width;
    if(GrLineWidth<=0) GrLineWidth=1;
    GrLineStyle = Style;
    GrPenColor = palette[c];
}

/// \brief void set_pen_rgb() - Ustala aktualny kolor linii za pomocą składowych RGB
/// \param r
/// \param g
/// \param b
/// \param width
/// \param Style
/// Ustala aktualny kolor linii za pomocą składowych RGB, oraz od razu styl, żeby nie mnożyć wywołań
/// UWAGA DO IMPLEMENTACJI:
/// Jeżeli kolory indeksowane korzystają z cache'owania tego samego pisaka to
/// należy ustalić kolor aktualny na pusty np. curr_color=-1;
void set_pen_rgb(ssh_intensity r,ssh_intensity g, ssh_intensity b,
                 ssh_natural width,ssh_mode Style)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_pen_rgb
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b << SEP << width << SEP << Style << endl;
    GrLineWidth = width;
    if(GrLineWidth<=0) GrLineWidth=1;
    GrLineStyle = Style;
    GrPenColor.r = r & 0xff;
    GrPenColor.g = g & 0xff;
    GrPenColor.b = b & 0xff;
}

/// \brief void set_pen_rgba() - Ustala aktualny kolor linii za pomocą składowych RGBA
/// \param r
/// \param g
/// \param b
/// \param a
/// \param width
/// \param style
/// Aktualnie składowa 'a' jest IGNOROWANA!
void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a,
                  ssh_natural width,ssh_mode style)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_pen_rgba
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b << SEP << width << SEP << style << endl;
    GrLineWidth = width;
    if(GrLineWidth<=0) GrLineWidth=1;
    GrLineStyle = style;
    GrPenColor.r = r & 0xff;
    GrPenColor.g = g & 0xff;
    GrPenColor.b = b & 0xff;
  //GrPenColor.a = a & 0xff;//TODO!
}

/// \brief void set_brush() - Ustala aktualny kolor wypełnień za pomocą typu ssh_color
/// \param c
/// ssh_color jest jak dotąd zawsze traktowany jako indeks do tabeli, ale ma miejsce na tryb RGB
/// W modułach Win i X11 kolory indeksowane korzystają z cache'owania systemowych pędzli,
/// ale w SVG musi działać samo.
void set_brush(ssh_color c)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_brush
    if(ssh_trace_level>2) cout << (ssh_color)c << endl;
    GrBrushColor = palette[c];
}

/// \brief void set_brush_rgb() - Ustala aktualny kolor wypełnień za pomocą składowych RGB
/// \param r
/// \param g
/// \param b
/// UWAGA DO IMPLEMENTACJI:
/// Jeżeli kolory indeksowane korzystają z cache'owanie tego samego pędzla to
/// należy ustalić kolor aktualny na pusty np. curr_fill=-1;
/// W implementacji SVG musi działać samo.
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_brush_rgb
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b <<  endl;
    GrBrushColor.r = r & 0xff;
    GrBrushColor.g = g & 0xff;
    GrBrushColor.b = b & 0xff;
}

/// Ustala aktualny kolor wypelnień za pomocą składowych RGBA
/// Składowa 'a' jest na razie ignorowana.
void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,
                   ssh_intensity a)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//set_brush_rgba
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b <<  endl;
    GrBrushColor.r = r & 0xff;
    GrBrushColor.g = g & 0xff;
    GrBrushColor.b = b & 0xff;
    //GrBrushColor.a = a & 0xff;//TODO!
}

/** ODCZYTYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO
 * **************************************************** */

/// Zwraca 1 jeśli okno graficzne (lub wirtualne) jest buforowane
/// W przypadku implementacji SVG zawsze zwraca 1
ssh_mode  buffered()
{
    if(ssh_trace_level>0) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << "return yes==1";//buffered
    if(ssh_trace_level>0) cout << endl;
    return 1;
}

/// Czy okno ma zafiksowana wielkość?
/// W przypadku implementacji SVG zawsze zwraca 1
ssh_mode fixed()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << "return yes===1";//fixed
    if(ssh_trace_level>2) cout << endl;
    return 1;
}

/// Aktualny kolor tla...
ssh_color background()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP <<"return "<< curr_background;//background
    if(ssh_trace_level>2) cout << endl;
    return curr_background;
}

/// Aktualna grubość linii
ssh_natural get_line_width()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << "return " << GrLineWidth;//get_line_width
    if(ssh_trace_level>2) cout << endl;
    return GrLineWidth;
}

/// Aktualny kolor linii jako ssh_color (indeks)
/// W przypadku implementacji SVG zawsze zwraca -1024 (out of table!)
ssh_color get_pen()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//get_pen
    if(ssh_trace_level>2) cout <<"IGNORED"<< endl;
    return -1024;
}

/// Aktualny kolor wypełnień jako ssh_color
/// W przypadku implementacji SVG zawsze zwraca 0 (czarny)
/// Dlaczego nie -1024 (out of table!) ? TODO ?
ssh_color get_brush()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//get_brush
    if(ssh_trace_level>2) cout <<"IGNORED"<< endl;
    return 0;
}

/// Aktualne rozmiary pionowe okna z init_plot po przeliczeniach
/// ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora
ssh_natural screen_height()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << "return " << GrScreenHi;//screen_height
    if(ssh_trace_level>2) cout << endl;
    return GrScreenHi;
}

/// Aktualne rozmiary poziome okna z init_plot po przeliczeniach
/// ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora
ssh_natural screen_width()
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << "return " << GrScreenWi;//screen_width
    if(ssh_trace_level>2) cout << endl;
    return GrScreenWi;
}

/// Aktualne rozmiary znaku - wysokość
/// ...potrzebne do pozycjonowania tekstu
ssh_natural char_height(char znak)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << znak << SEP << "return " << GrFontHi;//char_height
    if(ssh_trace_level>2) cout << endl;
    return GrFontHi;
}

/// Aktualne rozmiary znaku - szerokość
/// ...potrzebne do pozycjonowania tekstu
ssh_natural char_width(char znak)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP <<znak<<SEP<< "return " << GrFontWi;//char_width
    if(ssh_trace_level>2) cout << endl;
    return GrFontWi;
}

/// Aktualne rozmiary wyświetlania całego łańcucha znaków - wysokość
/// Tu zazwyczaj może być to samo co char_height.
ssh_natural string_height(const char* str)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP ;//string_height
    if(ssh_trace_level>2) cout << str <<SEP << "return " << GrFontHi<< endl;
    return GrFontHi;
}

/// Aktualne rozmiary wyświetlania całego łańcucha znaków - szerokość
/// W najgorszym razie odpowiednia wielokrotność char_width
/// ...potrzebne do jego pozycjonowania */
ssh_natural string_width(const char* str)
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//string_width
    unsigned len = strlen(str);
    if(ssh_trace_level>2) cout << str << SEP << "return " << GrFontWi*len << endl;
    return GrFontWi*len;
}

//**  DO TEGO MIEJSCA POPRAWIONE **
//*********************************

/* WYPROWADZANIE TEKSTU */
void printc(int x, int y,
            ssh_color fore, ssh_color back,
            const char* format, ...)
/* Drukuje w kolorach uzytkownika wybranych z palety*/
{
    extern int  GrPrintTransparently;// = 0;
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << SEP;//printc
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << fore << SEP << back << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.r = palette[back].r;
    Op.text.g = palette[back].g;
    Op.text.b = palette[back].b;
    Op.text.rf = palette[fore].r;
    Op.text.gf = palette[fore].g;
    Op.text.bf = palette[fore].b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);
    va_end(marker);              /* Reset variable arguments.      */
    //if(Op.text.txt != NULL) //Tu niepotrzebne - nowy obiekt powinien być sklirowany
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}

void printbw(int x,int y,const char* format,...)
/* Drukuje czarno na białym*/
{
    extern int  GrPrintTransparently;// = 0;
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << SEP;//printbw
    if(ssh_trace_level>2) cout << x << SEP << y <<SEP<< format << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;

    Op.text.r = 255;
    Op.text.g = 255;
    Op.text.b = 255;
    Op.text.rf = 0;
    Op.text.gf = 0;
    Op.text.bf = 0;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);						assert(strlen(target) < 2046);
    va_end(marker);              /* Reset variable arguments.      */
    //Op.text.txt.take(clone_str(target));
    //if(Op.text.txt != NULL)
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}

void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...)
{
    extern int  GrPrintTransparently;// = 0;
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << SEP;//print_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.rf = GrPenColor.r;
    Op.text.gf = GrPenColor.g;
    Op.text.bf = GrPenColor.b;
    Op.text.r = GrBrushColor.r;
    Op.text.g = GrBrushColor.g;
    Op.text.b = GrBrushColor.b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);			assert(strlen(target) < 2046);
    va_end(marker);              /* Reset variable arguments.      */
    //Op.text.txt.take(clone_str(target));
    //if(Op.text.txt != NULL)
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}



void print_rgb(int x, int y,
               unsigned r, unsigned g, unsigned b,
               ssh_color back,
               const char* format, ...)
/* Drukuje z możliwością ustawienia tuszu poprzez RGB */
{
    extern int  GrPrintTransparently;// = 0;
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << SEP;//print_rgb
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r<<','<<g<<','<<b << SEP << back << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.rf = r;
    Op.text.gf = g;
    Op.text.bf = b;
    Op.text.r = palette[back].r;
    Op.text.g = palette[back].g;
    Op.text.b = palette[back].b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);			assert(strlen(target) < 2046);
    va_end(marker);              /* Reset variable arguments.      */
    //Op.text.txt.take(clone_str(target));
    //if(Op.text.txt != NULL)
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}

/* PUNKTOWANIE  */

void plot_d(ssh_coordinate x, ssh_coordinate y)
/* Wyswietlenie punktu na ekranie */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP<<SEP;//plot
    if(ssh_trace_level>2) cout << x << SEP << y << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02;//Plot
    Op.point.r = GrPenColor.r;
    Op.point.g = GrPenColor.g;
    Op.point.b = GrPenColor.b;
    Op.point.x = x;
    Op.point.y = y;
}

void plot(int x,int y, ssh_color c)
/* Wyswietlenie punktu na ekranie */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP<<SEP;//plot
    if(ssh_trace_level>2) cout << x << SEP << y << SEP << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02;//Plot
    Op.point.r = palette[c].r;
    Op.point.g = palette[c].g;
    Op.point.b = palette[c].b;
    Op.point.x = x;
    Op.point.y = y;
}

void plot_rgb(ssh_coordinate x, ssh_coordinate y, int r, int g, int b)
/* Wyswietlenie punktu na ekranie w kolorze true-color.
Jak inny tryb okna to efekt mo�e by� dziwny - ale ju� niemal nie ma ekran�w nie-true-color*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//plot_rgb
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP << g << SEP << b << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02;//Plot
    Op.point.r = r;
    Op.point.g = g;
    Op.point.b = b;
    Op.point.x = x;
    Op.point.y = y;
}

void fill_flood(ssh_coordinate x, ssh_coordinate y, ssh_color fill, ssh_color border)
/* Wypełnia powodziowo lub algorytmem siania w kolorze indeksowanym*/
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_flood
    if(ssh_trace_level>1) cout << x << SEP << y << SEP
                               <<(ssh_color)fill<< SEP << (ssh_color)border << endl;

    GrOperation& Op = NextGrListEntry();
    //struct Point { unsigned type : 3; unsigned mode : 5; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rb : 8;  unsigned gb : 8; unsigned bb : 8; } point;
    Op.empty.type = GrType::Point;
    Op.point.mode = 0x1;//FILL
    Op.point.r = palette[fill].r;
    Op.point.g = palette[fill].g;
    Op.point.b = palette[fill].b;
    Op.point.x = x;
    Op.point.y = y;
    Op.point.rb = palette[border].r;
    Op.point.gb = palette[border].g;
    Op.point.bb = palette[border].b;
}

void fill_flood_rgb(int x,int y,
                    int rf,int gf,int bf,int rb,int gb,int bb)
/* Wypelnia powodziowo lub algorytmem siania */
/* w kolorze RGB */
/* Ale SVG chyba tego nie ma? */
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_flood_rgb
    if(ssh_trace_level>1) cout << x << SEP << y << SEP
                               << rf << SEP << gf << SEP << bf << SEP
                               << rb << SEP << gb << SEP << bb << endl;

    GrOperation& Op = NextGrListEntry();//???????
    //struct Point { unsigned type : 3; unsigned mode : 5; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rb : 8;  unsigned gb : 8; unsigned bb : 8; } point;
    Op.empty.type = GrType::Point;
    Op.point.mode = 0x1;//FILL
    Op.point.r = rf;
    Op.point.g = gf;
    Op.point.b = bf;
    Op.point.x = x;
    Op.point.y = y;
    Op.point.rb = rb;
    Op.point.gb = gb;
    Op.point.bb = bb;
}

/* RYSOWANIE  */

void line_d(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2)
/* Wyswietlenie lini w kolorze domyslnym - tak�e rgb */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP<< SEP;//line_d
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Line;
    //struct Line   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned w:8;} line;
    Op.line.mode = GrLineStyle;
    Op.line.w = GrLineWidth;
    Op.line.x1 = x1;
    Op.line.x2 = x2;
    Op.line.y1 = y1;
    Op.line.y2 = y2;
    Op.line.r = GrPenColor.r;
    Op.line.g = GrPenColor.g;
    Op.line.b = GrPenColor.b;
}

void line(int x1,int y1,int x2,int y2,ssh_color c)
/* Wyswietlenie lini w kolorze c z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP<< SEP;//line
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Line;
    //struct Line   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned w:8;} line;
    Op.line.mode = GrLineStyle;
    Op.line.w = GrLineWidth;
    Op.line.x1 = x1;
    Op.line.x2 = x2;
    Op.line.y1 = y1;
    Op.line.y2 = y2;
    Op.line.r = palette[c].r;
    Op.line.g = palette[c].g;
    Op.line.b = palette[c].b;
}

void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
/* Wyswietlenie okregu w kolorze domyslnym - także rgb */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << endl;
    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0;//NO FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = GrPenColor.r;
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
}

void ellipse_d(ssh_coordinate x,ssh_coordinate y, ssh_natural a, ssh_natural b)
/* Wyswietlenie elipsy w kolorze domyslnym - także rgb */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;
    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0;//NO FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = GrPenColor.r;
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
}

void ellipse(ssh_coordinate x,ssh_coordinate y, ssh_natural a, ssh_natural b, ssh_color c)
/* Wyswietlenie elipsy w kolorze c */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;
    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0;//NO FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = palette[c].r;
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
}

void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c)
/* Wyswietlenie okregu w kolorze c  z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP<< SEP;//circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0;//NO FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = palette[c].r;
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
}

void fill_circle_d(int x,int y,int r)
/* Wyswietlenie kola w kolorach domyslnych - także rgb */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1;//FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = GrPenColor.r;//Prawdopodobnie b�dzie ignorowane
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
    Op.circle.rf = GrBrushColor.r;
    Op.circle.gf = GrBrushColor.g;
    Op.circle.bf = GrBrushColor.b;
}

void fill_ellipse_d(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b)
/* Wypełnienie elipsy w kolorach domyslnych - także rgb */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1;//FILL
    Op.circle.w = GrLineWidth;
    Op.circle.r = GrPenColor.r;//Prawdopodobnie b�dzie ignorowane
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
    Op.circle.rf = GrBrushColor.r;
    Op.circle.gf = GrBrushColor.g;
    Op.circle.bf = GrBrushColor.b;
}

void fill_circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
                 ssh_color c)
/* Wyswietlenie kola w kolorze c  z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1;//FILL
    Op.circle.w = 0;// GrLineWidth; TAK NIEKONSEKWENTNIE SI� ZACHOWUJE ORYGINA� BITMAPOWY.
    Op.circle.r = palette[c].r;//Mo�e b�dzie ignorowane
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
    Op.circle.rf = palette[c].r;
    Op.circle.gf = palette[c].g;
    Op.circle.bf = palette[c].b;
}

void fill_ellipse(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b, ssh_color c)
/* Wyswietlenie elipsy w kolorze c  z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1;//FILL
    Op.circle.w = 0;// GrLineWidth; TAK NIEKONSEKWENTNIE SI� ZACHOWUJE ORYGINA� BITMAPOWY.
    Op.circle.r = palette[c].r;//Mo�e b�dzie ignorowane
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
    Op.circle.rf = palette[c].r;
    Op.circle.gf = palette[c].g;
    Op.circle.bf = palette[c].b;
}

void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,            /*rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop)
{}

void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
           ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{}

void earc_d(ssh_coordinate x,ssh_coordinate y,                         /*rysuje łuk eliptyczny */
            ssh_natural a,ssh_natural b,                               /* o półosiach a i b */
            ssh_radian start,ssh_radian stop)
{}

void earc(ssh_coordinate x,ssh_coordinate y,
          ssh_natural a,ssh_natural b,
          ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{}

void fill_arc_d(ssh_coordinate x, ssh_coordinate y, ssh_natural r,       /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start, ssh_radian stop, ssh_bool pie)         /* początek i koniec łuku */
{}

void fill_arc(ssh_coordinate x, ssh_coordinate y, ssh_natural r,         /* wirtualny środek i promień łuku */
              ssh_radian start, ssh_radian stop, ssh_bool pie, ssh_color c)            /* w kolorze c */
{}

void fill_earc_d(ssh_coordinate x, ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a, ssh_natural b,                          /* o półosiach a i b */
                 ssh_radian start, ssh_radian stop, ssh_bool pie)                     /* początek i koniec łuku */
{}

void fill_earc(ssh_coordinate x, ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a, ssh_natural b,                            /* o półosiach a i b */
               ssh_radian start, ssh_radian stop, ssh_bool pie, ssh_color c)           /* w kolorze c */
{}


void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,                /* Wypełnienie prostokata */
                   ssh_coordinate x2,ssh_coordinate y2,                /* rozciągniętego między rogami x1y1 a x2y2 */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b)    /* w kolorze rbg okreslonym składowymi koloru */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_rect_d
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1;//FILL
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = r;//Moze bedzie ignorowane? TODO check!
    Op.rect.g = g;
    Op.rect.b = b;
    Op.rect.rf = r;//Kolor wypelnienia
    Op.rect.gf = g;
    Op.rect.bf = b;
}

void fill_rect_d(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2)
/* Wyswietla prostokat w kolorach domyslnych*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_rect_d
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1;//FILL
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = GrPenColor.r;//Prawdopodobnie będzie ignorowane
    Op.rect.g = GrPenColor.g;
    Op.rect.b = GrPenColor.b;
    Op.rect.rf = GrBrushColor.r;//Kolor wypełnienia
    Op.rect.gf = GrBrushColor.g;
    Op.rect.bf = GrBrushColor.b;
}

void fill_rect(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2, ssh_color c)
/* Wyswietla prostokat w kolorze c  z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_rect
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1;//FILL
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = palette[c].r;//Prawdopodobnie bedzie ignorowane
    Op.rect.g = palette[c].g;
    Op.rect.b = palette[c].b;
    Op.rect.rf = palette[c].r;//Kolor wypelnienia
    Op.rect.gf = palette[c].g;
    Op.rect.bf = palette[c].b;
}

void fill_poly_d(ssh_coordinate vx, ssh_coordinate vy,
                 const ssh_point points[], int number)
/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_poly_d
    if(ssh_trace_level>2) cout << vx << SEP << vy << SEP
                               << number << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Poly;
    //struct Poly   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned rf : 8; unsigned gf : 8;unsigned bf :8; wb_dynarray<ssh_point> points; } poly;
    Op.poly.mode = 0x1;//FILL
    Op.poly.r = GrPenColor.r;//Prawdopodobnie b�dzie ignorowane
    Op.poly.g = GrPenColor.g;
    Op.poly.b = GrPenColor.b;
    Op.poly.rf = GrBrushColor.r;//Kolor wype�nienia
    Op.poly.gf = GrBrushColor.g;
    Op.poly.bf = GrBrushColor.b;
    //if(Op.poly.points!=NULL)
    //	delete [] Op.poly.points;
    Op.poly.points= new ssh_point[number];
    Op.poly.size=number;
    for (unsigned i = 0; i < number; i++)
    {
        Op.poly.points[i].x = points[i].x + vx;
        Op.poly.points[i].y = points[i].y + vy;
    }
}

void fill_poly(ssh_coordinate vx, ssh_coordinate vy,
               const ssh_point points[], int number,
               ssh_color c)
/* Wypelnia wielokat przesuniety o vx,vy w kolorze c  z palety*/
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//fill_poly
    if(ssh_trace_level>2) cout << vx << SEP << vy << SEP
                               << number << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry();//enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Poly;
    //struct Poly   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned rf : 8; unsigned gf : 8;unsigned bf :8; wb_dynarray<ssh_point> points; } poly;
    Op.poly.mode = 0x1;//FILL
    Op.poly.r = palette[c].r;//Prawdopodobnie b�dzie ignorowane
    Op.poly.g = palette[c].g;
    Op.poly.b = palette[c].b;
    Op.poly.rf = palette[c].r;//Kolor wype�nienia
    Op.poly.gf = palette[c].g;
    Op.poly.bf = palette[c].b;
    //Op.poly.points.alloc(number);
    //if(Op.poly.points!=NULL)
    //	delete [] Op.poly.points;
    Op.poly.points= new ssh_point[number];
    Op.poly.size=number;
    for (unsigned i = 0; i < number; i++)
    {
        Op.poly.points[i].x = points[i].x + vx;
        Op.poly.points[i].y = points[i].y + vy;
    }
}

/* POBIERANIE ZNAKOW Z KLAWIATURY i ZDAZEN OKIENNYCH (w tym z MENU) */

/* Normalnie są to znaki skierowane do okna graficznego i nie związane ze
strumieniem wejściowym. W przypadku implementacji na pliku graficznym
mozna by tu zrobić nieblokujące standardowe wejście. Ale chyba bardziej
elastyczny byłby "named pipe" o nazwie zaleznej od PID i nazwy pliku wykonywalnego */
ssh_mode input_ready()
/* Funkcja sprawdzajaca czy jest cos do wziecia z wejscia */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ <<SEP<< GrCharMessage << endl;//input_ready
    if (GrCharMessage >= -1)
        return SSH_YES;
    else
        return SSH_NO;
}

ssh_msg get_char()
/* Funkcja odczytywania znakow sterowania i zdarzen */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << GrCharMessage << endl;//get_char
    if (GrCharMessage >= -1)
    {
        int c = GrCharMessage;
        GrCharMessage = -2;
        return c;
    }
    else
        return 0;//Jak nie ma czego zwrócić to zwraca neutralne 0. Nigdy nie staje na tej funkcji, jak przy zwykłym oknie
    //return -1;//-1 oznacza koniec wejścia - np. kliknięcie w "zamykacz okna" - co się tu normalnie nie zdarza
}

ssh_stat set_char(ssh_msg c)
/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
/* Pewne jest tylko odeslanie jednego znaku. */
{
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << c << endl;//set_char
    if (GrCharMessage < -1)
    {
        GrCharMessage = c;
        return 1; //udało się
    }
    else
        return 0;//Nie udało się bo poprzednie nie zostało odczytane
}

ssh_stat get_mouse_event(ssh_coordinate *xpos, ssh_coordinate *ypos, ssh_coordinate *click)
/* Funkcja odczytujaca ostatnie zdazenie myszy */
/* Można odczytać kiedykolwiek, ale sens ma tylko gdy get_char() zwróciło '\b' */
{
    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << endl;//get_mouse_event
    if (GrMouseActive)
    {
        *xpos = GrMouseX;
        *ypos = GrMouseY;
        *click = GrMouseC;
        return 1;
    }
    else
        return 0;
}


ssh_stat repaint_area(int* x,int* y,unsigned* width,unsigned* height)
/* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
/* Jesli zwraca -1 to brak danych lub brak implementacji ! Odrysowac trzeba całosc. */
/* Jesli zwraca -2 to znaczy ze dane juz były wcześniej odczytane. Nalezy zignorowac. */
{
    if(ssh_trace_level>0) cout <<"SVG: " << _FUNCTION_NAME_ << endl;//repaint_area
    //Nie powinien by� u�ywany, ale je�li ju� to musi to prosi o odrysowanie ca�o�ci bo tak bezpieczniej
    *x = 0;
    *y = 0;
    *width = GrScreenWi;
    *height = GrScreenHi;
    return -1;
}

ssh_rgb get_rgb_from(ssh_color c)
/* Jakie są ustawienia RGB konkretnego kolorku w palecie */
{
	ssh_rgb pom;
    if(ssh_trace_level>2) cout <<"SVG: " << _FUNCTION_NAME_ << SEP << (ssh_color)c << endl;//get_rgb_from
	pom = palette[c];
	return pom;
}

static void SetScale(void)
//Wewnetrzna implementacja termicznej skali kolorów
//Czyli wypełnienie tabeli rgb dla kolorów indeksowanych
{
#ifndef M_PI
    const double M_PI=3.141595;
#endif

    if(ssh_trace_level>1) cout <<"SVG: " << _FUNCTION_NAME_ << endl;

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

        if(ssh_trace_level & 4)
           cout <<"SVG: " << _FUNCTION_NAME_ << SEP <<"SetScale (0-255 Gray) completed"<< endl;
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
        //else ?ALTERNATYWNIE? TODO CHECK!
        {
            unsigned k;
            for(k=256;k<PALETE_LENGHT; k++)
                set_rgb(k,(unsigned char)k,(unsigned char)k,(unsigned char)k );
            if(ssh_trace_level & 4)
               cout <<"SVG: " << _FUNCTION_NAME_ << SEP <<"SetScale (Colors: 0-255; Gray: 256-->" << PALETE_LENGHT << ") completed" << endl;
        }
    }

    set_rgb(255,255,255,255); //Zazwyczaj oczekuje ze kolor 255 to biały, albo chociaż jasny
}

//NAJWAŻNIEJSZE FUNKCJE - ZAPIS INFORMACJI DO PLIKU W FORMACIE WEKTOROWYM
////////////////////////////////////////////////////////////////////////////////

extern const char* GrFileOutputByExtension;// = "str";//Tym można sterować format pliku wyjściwego. Jak format nieznany to wyrzuca strumień obiektowy .str

int writeSTR(ostream& o)
{
	o << "#otx file - objects as text" << endl;
	o << "#enum  GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };" << endl;
	ssh_rgb bac = get_rgb_from( get_background() );
	o << "BACKGROUND=( " << unsigned(bac.r) << ',' << unsigned(bac.g) << ',' << unsigned(bac.b) << " )" << endl;
	o << "GrOpt*[" << GrListPosition + 1 << "] {" << endl;
	for (unsigned i = 0; i <= GrListPosition;i++)
		switch (GrList[i].empty.type)
		{
		case GrType::Empty:	break;//NIE ROBI NIC!
		case GrType::Point: {
			o << "Point" << "\t{\t";
			struct Point &pr = (GrList[i].point);
			o << pr.x << "; " << pr.y << "; 0x"<<hex<<pr.mode <<dec<< "; ";
			if (pr.mode != 0)//MODE == 0 MEANS MoveTo
				o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";//COLOR
			if (pr.mode == 1)//FLOOD FILL TO BORDER
				o << "(" << pr.rb<< ',' << pr.gb<< ',' << pr.bb<< "); ";
			o << " }" << endl;
		}; break;
		case GrType::LineTo: {
			o << "#LineTo" << "\t{\t";
			struct LineTo &pr = (GrList[i].lineTo);
			o << "NOT IMPLEMENTED!";
			o << " }" << endl;
		};  break;
		case GrType::Line: {
			o << "Line" << "\t{\t";
			struct Line &pr = (GrList[i].line);
			o << pr.x1 << "; " << pr.y1 << "; " << pr.x2 << "; " << pr.y2 << "; "<< pr.w << "; 0x"  <<hex<< pr.mode<<dec<< "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			o << " }" << endl;
		};  break;
		case GrType::Circle: {
			o << "Circle" << "\t{\t";
			struct Circle &pr = (GrList[i].circle);
			o << pr.x << "; " << pr.y << "; " << pr.rx << "; " << pr.ry << "; " << pr.w << "; 0x" << hex << pr.mode << dec << "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		}; break;
		case GrType::Rect: {
			o << "Rect" << "\t{\t";
			struct Rect &pr = (GrList[i].rect);
			o << pr.x1 << "; " << pr.y1 << "; " << pr.x2 << "; " << pr.y2 << "; " << pr.w << "; 0x" << hex << pr.mode << dec << "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 0x1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		}; break;
		case GrType::Text: {
			o << "Text" << "\t{\t";
			struct Text &pr = (GrList[i].text);
			o << pr.x << "; " << pr.y << "; 0x" << hex << pr.mode << dec << "; "<<endl;
			o << "\t\t\"" << pr.txt << endl;
			o << "\t\t(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";//FOREGROUND
			if(pr.mode!= 0x1 )//WITH FILLED BACKGROUND
				o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";//T�O
			o << " }" << endl;
		}; break;
		case GrType::Poly: {
			o << "Poly" << "\t{\t";
			struct Poly &pr = (GrList[i].poly);
			o << pr.w << "; 0x" << hex << pr.mode << dec << "; " << endl;
			o << "\t\tint2d[" << pr.size << "] {";
			for (unsigned j = 0; j < pr.size; j++)
				o << " (" << pr.points[j].x << ',' << pr.points[j].y << ")";
			o << " }"<<endl;
			o << "\t\t(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 0x1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		}; break;
		default:
			o << "#unknown type!!! " << GrList[i].empty.type << endl;
			break;
		}
	o << "};\t#End of GrOpt list" << endl;
	return 0;
}

int writeSVG(ostream& o)
{
    //unsigned GrReloadInterval = 1000; //Co ile czasu skrypt w pliku SVG wymusza prze�adowanie. Jak 0 to w og�le nie ma skryptu.
	int curX = 0, curY = 0; //Do MoveTo i LineTo
	o << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
		 "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";

	o << "<svg "
			"xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
			"xmlns:cc=\"http://creativecommons.org/ns#\" "
			"xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" "
			"xmlns:svg=\"http://www.w3.org/2000/svg\" "
			"xmlns=\"http://www.w3.org/2000/svg\" "
			"version=\"1.1\" ";

	if(GrReloadInterval>0)
		o << "onload=\"init(evt)\" ";//Sam skrypt mo�e dopiero na ko�cu?

	o <<" x=\"0px\" ";
	o <<"y=\"0px\" ";
	o <<" width=\"" << GrScreenWi     << "px\" ";
	o <<" height=\"" << GrScreenHi + 22 <<"px\" >"<<endl;//Troch� miejsca na wirtualnym ekranie na copyright

	//if(GrReloadInterval>0)
	//{
	//o<<"<META HTTP-EQUIV=\"Refresh\" CONTENT=\""<<int(GrReloadInterval/1000)<<"\">\n" //??? Tak to dzia�a w HTMLu, ale w SVG nie bardzo
	//}
	if(GrReloadInterval>0)
	{
		o <<
		"<script type=\"text/ecmascript\"><![CDATA[ "
		"function init(evt){ "
		"setTimeout(function(){ "
       //location.href='http://.... .pl'; //gdyby mia� �adowa� co� innego
        "location.reload(1); "
		" }, "<< GrReloadInterval <<" ); "
		"}  ]]></script> "<<endl;
	}

	//T�O
	ssh_rgb bac = get_rgb_from(get_background());
	o << "<rect x=\"0px\" y=\"0px\" width=\"" << GrScreenWi << "px\" height=\"" << GrScreenHi << "px\" rx=\"0\" style=\"fill:"
		//<<"rgb(128,0,128)"<<" "
	  <<"rgb(" << unsigned(bac.r) << ',' << unsigned(bac.g) << ',' << unsigned(bac.b) << "); "
      <<"stroke:#000000; stroke-width:0px;\" />" << endl;

    o << "<text style=\"fill:red;\" x=\""<< 0 <<"\" y=\""<< GrScreenHi + 12 <<"\">This is SVG from "<<ScreenHeader<<" "<<ScreenTitle<<" </text>"<<endl;
	for (unsigned i = 0; i <= GrListPosition; i++)
		switch (GrList[i].empty.type)
		{
		case GrType::Empty:	break;//NIE ROBI NIC!
		case GrType::Point: {
			struct Point &pr = (GrList[i].point);
			if (pr.mode == 0)//MoveTo
			{
				curX = pr.x;
				curY = pr.y;
			}
			else
				if (pr.mode == 1)//FLOOD FILL TO BORDER
				{
					// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/flood-color
					// o << " rgb(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";//COLOR
					// o << " rgb(" << pr.rb << ',' << pr.gb << ',' << pr.bb << "); ";//BORDER
				}
				else
				{
					o << "<rect ";
					o << "x=\"" << pr.x << "px\" y=\"" << pr.y << "\" width=\"1px\" height=\"1px\" stroke=\"0px\" ";// << hex << pr.mode << dec << "; ";
					if (pr.mode != 0)//MODE == 0 MEANS MoveTo
						o << "fill=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" ";//COLOR
					o << "/>" << endl;
				}
		}; break;
		case GrType::LineTo: {
			o << "#LineTo" << "\t{\t";
			struct LineTo &pr = (GrList[i].lineTo);
			o << "NOT IMPLEMENTED!";
			o << " }" << endl;
		};  break;
		case GrType::Line: {
			struct Line &pr = (GrList[i].line);
			//o << "<line x1 =\"0\" y1=\"0\" x2=\"100\" y2=\"50\" stroke=\"blue\" stroke-width=\"6\" />" << endl;
			o << "<line x1=\"" << pr.x1 << "px\" y1=\"" << pr.y1 << "px\" x2=\"" << pr.x2 << "px\" y2=\"" << pr.y2 << "px\" "
			  << "stroke-width=\"" << pr.w << "px\" ";
			o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" ";//COLOR
			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		};  break;
		case GrType::Circle: {
			struct Circle &pr = (GrList[i].circle);
			//o << "<circle cx=\"120\" cy=\"120\" r=\"80\" fill=\"red\" stroke=\"black\" stroke-width=\"5\" />" << endl;
			//o << "<ellipse cx=\"200\" cy=\"200\" rx=\"20\" ry=\"7\" fill=\"none\" stroke=\"black\" stroke-width=\"6\" />" << endl;
            if (pr.rx == pr.ry)//Koło - circle
				o << "<circle r=\"" << pr.ry << "px\" ";
			else
				o << "<ellipse rx=" << pr.rx << "px\" ry=\"" << pr.ry << "px\" ";

			o << "cx=\"" << pr.x << "px\" cy=\"" << pr.y << "px\" "
			  << "stroke-width=\"" << pr.w << "px\" ";
			if (pr.w>0)
				o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" ";//COLOR
			if (pr.mode == 0x1)//FILL
				o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" ";//FILL
			else
				o << "fill=\"none\" ";
			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		}; break;
		case GrType::Rect: {
			struct Rect &pr = (GrList[i].rect);
			o << "<rect ";// x = \"140\" y=\"120\" width=\"250\" height=\"250\" rx=\"40\"
			o << "x=\"" << pr.x1 << "px\" y=\"" << pr.y1 << "px\" width=\"" << pr.x2-pr.x1 << "px\" height=\"" << pr.y2-pr.y1 << "px\" "
			  << "stroke-width=\"" << pr.w << "px\" ";
			if (pr.w>0)
				o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" ";//COLOR
			if (pr.mode == 0x1)//FILL
				o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" ";//FILL
			else
				o << "fill=\"none\" ";
			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		}; break;
		case GrType::Text: {
			struct Text &pr = (GrList[i].text);
			int lenght = strlen(pr.txt);
			if (!pr.mode)//NOT TRANSPARENTLY
			{
				o << "<rect ";
				o << "x=\"" << pr.x << "px\" y=\"" << pr.y << "px\" width=\"" << lenght*GrFontWi << "px\" height=\"" << GrFontHi << "px\" "
					<< "stroke-width=\"" << 0 << "px\" ";
				o << "fill=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" />";//FILL
			}
			int realfont = (GrFontHi *4 )/ 5;
			o << "<text style=\"font-size:" << realfont << "px; fill: rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ");\" ";
			//o << "textLength=\""<< lenght*GrFontWi <<"px\" ";
			o << "lengthAdjust=\"spacingAndGlyphs\" ";
			o << "x = \"" << pr.x << "\" y=\"" << pr.y+ realfont << "\">"<< pr.txt <<"</text>" << endl;
		}; break;
		case GrType::Poly: {
			struct Poly &pr = (GrList[i].poly);
			//	o << "<polygon class =\"mystar\" fill=\"#3CB54A\"
			//points=\"134.973,14.204 143.295,31.066 161.903,33.77 148.438,46.896 151.617,65.43 134.973, 56.679, 118.329, 65.43 121.507, 46.896 108.042, 33.77 126.65, 31.066\" />" << endl;
			o << "<polygon ";
			if (pr.mode == 0x1)//FILL
				o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" ";//FILL
			else
				o << "fill=\"none\" ";
			o << "stroke-width=\"" << pr.w << "px\" ";
			if(pr.w>0)
				o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" ";//COLOR IF EXIST
			o << "points=\"";
			for (unsigned j = 0; j < pr.size; j++)
					o <<" "<< pr.points[j].x << ',' << pr.points[j].y <<" ";
			o << "\" />";
		}; break;
		default:
			o << "#unknown type!!! " << GrList[i].empty.type << endl;
			break;
		}


	o << "</svg>" << endl;

	return 0;
}

void flush_plot()
/* Ostateczne uzgodnienie zawartosci ekranu realnego z zawartoscia ekranu wirtualnego/tymczasowego w pami�ci */
{
    if(GrClosed)
    {
        cerr<<"SYMSHELL graphix not initialised"<<endl;
        return;
    }

    //GrTmpOutputDirectory ?
    static unsigned flush_counter = 0;//Zliczamy
    flush_counter++; //Ale nie uzywamy w nazwie pliku...
    if(ssh_trace_level>0) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;//flush_plot
    if(ssh_trace_level>0) cout <<'#'<< flush_counter <<SEP<< GrList.get_size() <<SEP<< GrListPosition << endl;
    wb_pchar name(MAX_PATH);
    name.prn("%s%s_%0u", GrTmpOutputDirectory,  ScreenTitle, PID );
    dump_screen(name.get()); //Zapisuje liste operacji graficznych do pliku w ustalonym formacie
}

ssh_stat	dump_screen(const char* Filename)
/* Zapisuje zawartosc ekranu do pliku graficznego w naturalnym formacie platformy */
/* Tutaj dostepne sa formaty wektorowe "stream", SVG, moze EXM ...*/
{
    if(ssh_trace_level>0) cout <<"SVG: " << _FUNCTION_NAME_ << SEP;
    if(ssh_trace_level>0) cout << Filename <<'.'<< GrFileOutputByExtension << endl;

    wb_pchar name(MAX_PATH);

    //Sposob zapisu zalezy od typu
    name.prn("%s.%s", Filename, "tmp" );
    ofstream Out( name.get() );

    if(!Out)
    {
        perror(Filename);
        return -1;
    }

    int ret = 0;
    if (strcmp(GrFileOutputByExtension, "svg") == 0
            || strcmp(GrFileOutputByExtension, "SVG") == 0)
    {
        ret = writeSVG(Out);
    }
    else
    {
        ret = writeSTR(Out);
    }
    if(ret)
        return ret;//Gdy błąd?
    Out.close();

    wb_pchar name2(MAX_PATH);
    //Sposob zapisu zalezy od typu
    name2.prn("%s.%s", Filename, GrFileOutputByExtension);

    remove(name2.get());//Na wypadek gdyby by�
    ret=rename(name.get(),name2.get());

    return ret;
}

/********************************************************************/
/*              SYMSHELLLIGHT  version 2021-11-22                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

