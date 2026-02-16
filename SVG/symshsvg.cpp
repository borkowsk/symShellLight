/** @file
 *  \brief                      SYMSHELL SVG IMPLEMENTATION
 *//* ******************************************************************************** */
/** \details    SYMSHELL IS A SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++
 **             ==================================================================
 **
 **             Simplest graphics interface implemented for SVG vector graphics.
 **             All graphic operations are buffered in a large list, and write
 **             into an SVG file when flush_screen() is called.
 **             Also dump_screen() produces an SVG file.
 **
 ** \note       This source file was changed massively: 17.11.2020, then 17.12.2021.
 **
 **
 ** \author     Designed by W. Borkowski from the University of Warsaw
 **
 ** \n          https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI
 ** \n          https://github.com/borkowsk
 **
 ** \library    SYMSHELLLIGHT  version 2026a
 ** 
/// @date 2026-02-16 (last modification)
 */
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cmath>
#include <cstdio>

#ifdef _MSC_VER
#include <chrono>
#include <thread>
/** takes microseconds, so you will have to multiply the input by 1000 in order to sleep in milliseconds. TODO: TEST IT! */
static inline int  usleep(__int64 usec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(usec));
    return 0; //?
}
#else
// extern int
#include <unistd.h>
#endif


#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
//#pragma warning(disable : 4996) //deprecated functions
//#pragma warning(disable : 4521) //multiple copy constructor
//#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#include <process.h>
#else
//#include <sys/types.h>
//#include <unistd.h>
#define _getpid   getpid
#endif

#include "symshell.h"
#define HIDE_WB_PTR_IO	1     //I/O NIEPOTRZEBNY
#include "wb_ptr.hpp"

using namespace std;
using namespace wbrtm;

/// Wewnętrzne śledzenie wywołań.
#if defined( _MSC_VER )
//#define STR_HELPER(x) #x
//#define STR(x) STR_HELPER(x)
//#define WB_FUNCTION_NAME_  ("SYMSHSVG_" STR( __LINE__ ) )
#define WB_FUNCTION_NAME_  __FUNCTION__
#else
#define WB_FUNCTION_NAME_  __func__ //C11
#endif

/* Zmienne eksportowane na zewnątrz
 * ********************************** */
extern "C" {
    [[maybe_unused]] const char *_ssh_grx_module_name="SVG";
}

/// Pid procesu. Przydaje się
unsigned long     PID=_getpid();

/// Maska poziomów śledzenia 1-msgs 2-grafika 3-grafika detaliczna 4-alokacje/zwalnianie.
int         ssh_trace_level = 0;

/// Jakiej długości inicjujemy tablice operacji graficznych (mnożone przez liczbę pikseli ekranu).
double      INITIAL_LENGTH_RATIO = 0.005;

/// Ile maksymalnie rekordów jest dopuszczalnych?//(mnożone przez liczbę pikseli ekranu).
double      MAXIMAL_LENGTH_RATIO = 0.999;

/// Rozszerzenie nazwy pliku wyjściowego. Jednocześnie określa format pliku wyjściowego.
/// Jak extension nieznane, to wyrzuca strumień obiektowy '.str'
const char* GrFileOutputByExtension = "svg";

/// Ścieżka, gdzie ma wrzucać zrzuty tymczasowe.
const char* GrTmpOutputDirectory = "./";

/// Co ile czasu skrypt w pliku SVG wymusza przeładowanie. Jak 0 to w ogóle nie ma skryptu.
unsigned    GrReloadInterval = 1000;

/// Myszy w SVG domyślnie nie ma, ale inny moduł może ją symulować przez linkowanie do tych zmiennych globalnych.
bool        GrMouseActive = false;
int         GrMouseX = -1; // Pozycja X symulowanej myszy
int         GrMouseY = -1; // Pozycja Y symulowanej myszy
int         GrMouseC = -1; // Klik symulowanej myszy (0,1,2)

/// Nie ma też klawiatury, ale inny moduł może ją symulować przez linkowanie do tych zmiennych globalnych.
int         GrCharMessage = -2;

/// "Dummy window" handler for check and external use.
[[maybe_unused]]  unsigned long    _ssh_window=0;

/// Separator wydruków.
const char* SEP = "\t";

/* Zmienne 'static' czyli bez dostępu z zewnątrz modułu
 * **************************************************** */

// Nazwa "okna" czyli domyślnego pliku generowanego przez flush_plot()
static const char*  ScreenTitle = "SSH_SVG";

// Domyślny nagłówek pliku
static char ScreenHeader[1024]="SSH SVG WINDOW";

// Ustawienia grafiki
static int          GrPrintTransparently = 0;

static ssh_natural  GrLineWidth = 1;
static ssh_mode     GrLineStyle = SSH_LINE_SOLID;

static ssh_rgb      GrPenColor = { 255,255,255}; //,255 };
static ssh_rgb      GrBrushColor = { 205,205,205}; //,255 };

static ssh_color    curr_background = 0;
static unsigned     GrScreenHi = 0;
static unsigned     GrScreenWi = 0;
static unsigned     GrFontHi = 14;
static unsigned     GrFontWi = 6;
static ssh_rgb      palette[512];

//  Flaga użycia skali szarości, np. do wydruków
static int          UseGrayScale = 0;  //Ustawiana jako parametr wywołania programu
                                       //podobnie jak opcje śledzenia i buforowania,
                                       //ale dla skali kolorów to jedyny sposób na
                                       //włączenie
//  Czy grafika już/jeszcze ZAMKNIĘTA?
static bool         GrClosed = true;

/* IMPLEMENTACJA
 * ************** */
namespace {
    enum GrType {
        Empty = 0, Point = 1, LineTo = 2, Line = 3, Circle = 4, Rect = 5, Text = 6, Poly = 7, Arc = 8
    }; // Typy rekordów

    struct Empty {
        unsigned type: 4;
        unsigned mode: 2;
        Empty():type(GrType::Empty),mode(0){}
    };
    struct Point {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2;
        unsigned x: 16;
        unsigned y: 16; // weight & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; /* main color */
        unsigned rb: 8;
        unsigned gb: 8;
        unsigned bb: 8; /* background color */ };
    struct LineTo {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2;
        unsigned x: 16;
        unsigned y: 16; // weight & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; /* main color */ };
    struct Line {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2;
        unsigned x1: 16;
        unsigned y1: 16; // weight & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned x2: 16;
        unsigned y2: 16; /* end point */ };
    struct Ellipse {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2;
        unsigned x: 16;
        unsigned y: 16; // weight & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned rx: 16;
        unsigned ry: 16; // the semi-axis of the ellipse
        unsigned rf: 8;
        unsigned gf: 8;
        unsigned bf: 8; /* secondary color */ };
    struct Arc {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2;
        unsigned x: 16;
        unsigned y: 16; // weight & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned rx: 16;
        unsigned ry: 16; // the semi-axis of the ellipse
        unsigned as: 16; // start angle
        unsigned ae: 16; // end angle
        unsigned rf: 8;
        unsigned gf: 8;
        unsigned bf: 8; /* secondary color */ };
    struct Rect {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2; // weight
        unsigned x1: 16;
        unsigned y1: 16; // x..y coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned x2: 16;
        unsigned y2: 16; // end point
        unsigned rf: 8;
        unsigned gf: 8;
        unsigned bf: 8; /* secondary color */ };
    struct Poly {
        unsigned type: 4;
        unsigned mode: 2;
        unsigned wi: 2; /*weight*/      unsigned si: 16; // array size
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned rf: 8;
        unsigned gf: 8;
        unsigned bf: 8; /* secondary color */
        ssh_point *points;
    };
    struct Text {
        unsigned type: 4;
        unsigned mode: 1;
        unsigned sc: 3;
        unsigned x: 16;
        unsigned y: 16; // height & coordinates
        unsigned r: 8;
        unsigned g: 8;
        unsigned b: 8; // main color
        unsigned rf: 8;
        unsigned gf: 8;
        unsigned bf: 8; /* secondary color */
        char *txt;
    };

/// Struktura do przechowywania operacji rysowania.
///\see \n "https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths"
    union GrOperation {
        struct Empty empty;
        struct Point point;
        struct LineTo lineTo;
        struct Line line;
        struct Ellipse circle;
        struct Arc arc;
        struct Rect rect;
        struct Poly poly;
        struct Text text;

        /// Konstruktor domyślny.
        GrOperation():empty() {
            memset((char*)this, 0, sizeof(GrOperation)); //Wypełnianie zerami
            //Powinno być oczywiste, ale...
            assert(text.txt == nullptr);
            assert(poly.points == nullptr);
        }

        /// Konstruktor kopiujący.
        GrOperation(GrOperation &From):empty() {
            memcpy((void *) this, (void*) &From, sizeof(GrOperation));
            if (empty.type == GrType::Text || empty.type == GrType::Poly) {
                memset((void*)(&From), 0, sizeof(GrOperation)); //Wypełnianie zerami
            }
        }

        /// Czyści stare operacje, np. gdy uznano, że efekt i tak jest zasłonięty, albo clear_screen(), albo end.
        void clean() {
            if (empty.type == GrType::Text && text.txt != nullptr)
                delete text.txt;
            else if (empty.type == GrType::Poly && poly.points != nullptr)
                delete[] poly.points;
            empty.type = GrType::Empty;
        }

        /// Destruktor. Jeżeli jest zapisany obiekt z danymi dynamicznymi to trzeba zwolnić
        ~GrOperation() {
            clean();
        }
    };

} //namespace

static wb_dynarray<GrOperation> GrList; // Lista operacji rysowania
static int GrListPosition = -1;         // Aktualna pozycja na liście
static int maxN=-1;                     // from MAXIMAL_LENGTH_RATIO;

// Funkcja implementacyjna. Zwraca dostęp do kolejnego "entry" tablicy operacji graficznych. W razie potrzeby alokuje.
static GrOperation&  NextGrListEntry_()
{
    if (++GrListPosition < GrList.get_size())
    {
        return GrList[GrListPosition]; //Zwraca dostęp do kolejnej operacji. Potem można wpisywać informacje
    }
    else //NIE MA MIEJSCA!!!
    {
        size_t N = GrList.get_size() * 2;
        if(N>maxN) N=maxN; //Nie więcej niż `maxN`

        if(N<maxN) //Jeszcze można powiększyć
        {
            //Przy powiększaniu nie chcemy użyć "expand", bo to by wywoływało destruktory i kopiowanie!
            size_t oldSize;
            GrOperation* RawPtr = GrList.give_dynamic_ptr_val(oldSize);               assert(GrList.get_size() == 0);

            GrList.alloc(N); //Nowy bufor w powiększonym rozmiarze
            memcpy((void*) GrList.get_ptr_val(), (void*) RawPtr, oldSize * sizeof(GrOperation)); //Przekopiowanie realnej zawartości
            for (size_t i = 0; i < oldSize; i++)
                RawPtr[i].empty.type = GrType::Empty; //Wirtualne wyczyszczenie starego
            delete [] RawPtr; //Zwalnianie bez wywoływania możliwych istotnych destruktorów dla Text i Poly
        }
        else //Już nie można bardziej powiększyć bufora! Kasujemy pół najstarszej zawartości i przesuwamy
        {
            GrOperation* RawPtr = GrList.get_ptr_val();
            GrListPosition/=2;
            for(size_t i=0; i < GrListPosition;i++)
                RawPtr[i].clean(); //Zwalniamy ewentualne składniki dynamiczne
            memmove((void*) RawPtr,(void*) (RawPtr+GrListPosition),GrListPosition*sizeof(GrOperation));
            size_t size=GrList.get_size();
            for(size_t i=GrListPosition;i<size;i++)
                RawPtr[i].empty.type = GrType::Empty; //Wirtualne wyczyszczenie zduplikowanej zawartości
        }

        return GrList[GrListPosition]; //Zwraca dostęp do kolejnej operacji, czyli pierwszej za starej listy
    }
}

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */

/// \brief Ustalanie innego tytułu okna niż nazwa aplikacji.
/// \param [in] window_name
[[maybe_unused]]
void set_title(const char* window_name)
{
    strncpy(ScreenHeader,window_name,1023);
}

/// \brief Obsługa parametrów wywołania programu.
/// \param [in] `title` nazwa okna i aplikacji
/// \param [in] `iArgc` liczba parametrów wywołania main()
/// \param [in] `iArgv` tablica parametrów wywołania z main()
[[maybe_unused]]
void shell_setup(const char* title, int iArgc, const char* iArgv[])
{
    if(ssh_trace_level>0) //shell_setup
    {
        cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;
        cout << title << SEP << iArgc << endl;
        for (int i = 0; i < iArgc; i++)
            cout << iArgv[i] << SEP;
        cout << endl<< "SVG: struct size:" << SEP << sizeof(GrOperation) << endl;  //assert(sizeof(GrOperation) == 16);???
    }

    ScreenTitle = title;

    for (int i = 1; i < iArgc; i++)
        if (iArgv[i][0] == '-')
        {
            if (iArgv[i][1] == 'F' || iArgv[i][1] == 'e' || iArgv[i][1] == 'f')
                GrFileOutputByExtension = iArgv[i] + 2;
            else
                if (iArgv[i][1] == 'D' || iArgv[i][1] == 'd')
                    GrTmpOutputDirectory = iArgv[i] + 2;
                else
                    if (iArgv[i][1] == 'C' || iArgv[i][1] == 'a' || iArgv[i][1] == 'c') {
                        //GrCharMessage = atoi(iArgv[i] + 2);
                        char** endPtr= nullptr; //Nie chce tego używać!
                        GrCharMessage = strtol(iArgv[i] + 2, endPtr, 10);
                        if(errno==ERANGE || errno==EINVAL)
                        {
                            cerr<<"ERROR! Invalid decimal value after `-C` or `-a`!"<<endl;
                            exit(-1);
                        }
                    }
                    else
                        if (iArgv[i][1] == 'R' || iArgv[i][1] == 'r') {
                            GrReloadInterval = atoi(iArgv[i] + 2);
                        }
                        else
                            if (iArgv[i][1] == 'B' || iArgv[i][1] == 'b') {
                                MAXIMAL_LENGTH_RATIO = atof(iArgv[i] + 2);
                            }
                            else
                                if (iArgv[i][1] == 'T' || iArgv[i][1] == 't') {
                                    ssh_trace_level = atoi(iArgv[i] + 2);
                                }
                                else
                                {
                                    cout<<"SVG graphics parameters:"<<endl;
                                    cout<<"-Fext - output format (svg or str)"<<endl;
                                    cout<<"-Dpath - output directory"<<endl;
                                    cout<<"-Cchar - single input character"<<endl;
                                    cout<<"-Rms - reload interval in an SVG embedded script"<<endl;
                                    cout<<"-Bratio - maximal size of buffer as a ratio of number of pixels"<<endl;
                                    cout<<"-Tint - trace level"<<endl;
                                    exit(-1);
                                    //cout<<"-x"<<endl;
                                }
        }
}

static void SetScale();  //Gdzieś tam jest funkcja ustalająca domyślną paletę kolorów indeksowanych

/// \brief inicjacja grafiki/semigrafiki — początek pracy okna/ekranu graficznego (lub wirtualnego).
/// \param a  to szerokość okna
/// \param b  to wysokość okna
/// \param ca  to miejsce na dodatkowe kolumny tekstu
/// \param cb  to miejsce na dodatkowe wiersze tekstu
/// \return (ssh_stat)1 - o ile wszystko poszło pomyślnie
/// \details
///     Parametry 'ca' i 'cb' są interpretowane wg. rozmiaru używanego fontu.
///     (na razie nie przewidziano używania różnych rozmiarów tekstu)
ssh_stat init_plot(ssh_natural  a, ssh_natural   b,                 /* ile pikseli mam mieć okno */
                   ssh_natural ca, ssh_natural  cb                  /* Ile linii i kolumn znaków marginesu */
               )
{
    atexit(close_plot);

    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //init_plot
    if(ssh_trace_level>1) cout << a << SEP << b << SEP << ca << SEP << cb << endl;

    SetScale(); //Ustawienie kolorów

    GrScreenWi = a + GrFontWi* ca;  assert(a>0);
    GrScreenHi = b + GrFontHi* cb;  assert(b>0);

    auto N = (unsigned)(((a + ca)*(b + cb)) * INITIAL_LENGTH_RATIO ); //Potem jest sprawdzane, czy nie za małe, warning zbędny
    maxN=(int)(((a + ca)*(b + cb)) * MAXIMAL_LENGTH_RATIO ); //Potem jest sprawdzane, czy nie za małe, warning zbędny

    if(N<1 || maxN<1 || N>maxN ) //N lub `maxN`, mniejsze od 1 to prawdopodobnie w inicjalizacji ekranu jest błąd
    {
        cerr << __FUNCTION__ << ": WARNING! : "
             << "((a + ca)*(b + cb))*INITIAL_LENGTH_RATIO="
             << "((" << a << " + " << ca << ")*(" << b << " + " << cb << "))*" << INITIAL_LENGTH_RATIO
             << "=" << N << endl;

        N=100; //Awaryjna poprawka

        if(maxN<N)
            maxN=10000;
    }

    N+=N%2;maxN+=maxN%2; //Likwidacja ewentualnej nieparzystości, ważna przy przesuwaniu

    GrList.alloc(N); //Tu nie może być zero

    GrClosed = false;

    _ssh_window=(unsigned long int)&GrList; //Czy to użyteczne to nie wiadomo, ale przynajmniej nie jest 0.
                                            //Bo 0 wskazywałoby, że inicjacja grafiki się nie powiodła.
    cerr<<GrScreenWi<<"x"<<GrScreenHi<<"-N:"<<N<<":"<<maxN<<endl;

    return 1; //OK
}

/// \brief Koniec pracy z grafiką.
///
/// Funkcja wykonuje zamknięcie grafiki/grafiki wirtualnej/semigrafiki.
/// Normalnie wypada ją wywołać, ale jest też automatycznie instalowana w `atexit`
/// \implementation  W module SVG grafiki wirtualnej zapisywanej do pliku zmienia tylko stan flagi
void close_plot()
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //close_plot
    if (GrClosed) return;

    if(ssh_trace_level>1) cout << "List length:"<<SEP<<GrList.get_size()<<SEP<<"USED:"<<SEP<< GrListPosition<<  endl;
    GrClosed = true;
    _ssh_window=0;
}

/// \brief Przełączanie buforowanie okna. Może nie zadziałać wywołane po inicjacji.
/// \param Yes (or No)
/// \details
/// W grafikach rastrowych zawartość pojawia się na ekranie albo od razu, albo po wywołaniu "flush_plot".
/// Chodzi o lepszą jakość animacji. Jednak przy debuggingu lepiej widzieć w trakcie rysowania.
/// \warning W module SVG aktualnie nie robi nic poza ewentualnym wyświetleniem na konsolę śladu użycia.
[[maybe_unused]]
void buffering_setup(int Yes)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;       //buffering_setup
    if(ssh_trace_level>1) cout << Yes <<SEP<<"IGNORED!"<< endl; //Nie ma sensu, bo nic nie jest wyświetlane w trakcie rysowania
}

/// \brief Czy symulować niezmienność rozmiarów okna?
/// \param Yes (or No)
///
/// W takim trybie zmiana wielkości okna powiększa piksele o całkowitą wielokrotność
/// \implementation W module SVG aktualnie nie robi nic poza ewentualnym wyświetleniem na konsolę śladu użycia.
/// Ponieważ zapisujemy tylko plik graficzny, to funkcja nie ma znaczenia
/// , bo raczej nie zwiększamy rozmiaru "ekranu" spoza programu (TODO ?)
[[maybe_unused]]
void fix_size(int Yes)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fix_size
    if(ssh_trace_level>1) cout << Yes << SEP << "IGNORED!" << endl; //Nie ma sensu, bo zapis i tak jest wektorowy i nie ma okna
}

/// \brief Zawieszenie wykonania programu na pewną liczbę milisekund.
/// \implementation
///     W module SVG nie ma sensu, bo taki program raczej działa w tle!
///     Jednak trochę robimy, bo mogło chodzić o szanse na przełączenie wątków albo procesów.
[[maybe_unused]]
void delay_ms(unsigned ms)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //delay_ms
    if(ssh_trace_level>1) cout << ms << SEP << "ALWAYS ONE `ms` == `10 us` !!!" << endl;

    usleep(10); //(ms*10)
}

/// \brief Zawieszenie wykonania programu na pewną liczbę mikrosekund.
/// \implementation
///     W module SVG nie ma to sensu, bo taki program raczej działa w tle!
///     Jednak trochę oczekiwania robimy, bo mogło chodzić o szanse na przełączenie wątków albo procesów.
[[maybe_unused]]
void delay_us(unsigned us)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //delay_us
    if(ssh_trace_level>1) cout << us << SEP << "ALWAYS ONE `us`!!!" << endl;

    usleep(1);
}

/* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO /TU WIRTUALNEGO/
 ************************************************************** */

/// \brief Ustala czy mysz ma byc obsługiwana.
/// \param Yes (or No)
/// \return
/// Zwraca poprzedni stan tego ustawienia
[[maybe_unused]]
int mouse_activity(ssh_mode Yes)
{
    if(ssh_trace_level>0) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //mouse_activity
    if(ssh_trace_level>0) cout << Yes << endl;
    int old = GrMouseActive;
    //GrMouseActive = (Yes?true:false); //Bo kretyński warning TODO DEBUG
    return old;
}

/// Ustala index koloru do czyszczenia tła.
/// Domyślne tło okna. Może nie zadziałać po inicjacji.
[[maybe_unused]]
void set_background(ssh_color c)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_background
    if(ssh_trace_level>1) cout << (ssh_color)c << endl;
    curr_background = c;
}

/* Operacje przestawiania własności pracy okna graficznego
 * ********************************************************* */

/// Czyści ekran lub ekran wirtualny. Zależnie czy jest buforowanie, czy nie.
/// Tu nawet nie ma ekranu wirtualnego.
/// CZYSZCZONA JEST LISTA!
[[maybe_unused]]
void clear_screen()
{
    if(ssh_trace_level) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //clear_screen
    if(ssh_trace_level) cout << endl;
    for (size_t i = 0; i < GrList.get_size(); i++)
        GrList[i].clean();
    GrListPosition = -1; //Pusto
}

/// Czyszczenie optymalizujące.
/// Cały ekran/okno zostanie wirtualnie "wymazany/e".
/// @note
///   Specjalnie dla tego modułu, bo tu jest bardzo tanie. W innych nie robi nic, bo czyszczenie jest tam kosztowne
///   , i zwykle staramy się go unikać, a tylko zamazywać nową treścią.
/// \return 1 always
[[maybe_unused]]
int invalidate_screen()
{
    if(ssh_trace_level) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;
    clear_screen();
    if(ssh_trace_level) cout << endl;
    return 1;
}

/// \brief Włącza drukowanie tekstu bez zamazywania tła/lub z.
/// \param Yes (`1` or `0`)
/// \return Zwraca ustawienie poprzednie trybu drukowania
[[maybe_unused]]
ssh_mode     print_transparently(ssh_mode Yes)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //print_transparently
    if(ssh_trace_level>2) cout << Yes << endl;
    int old = GrPrintTransparently;
    GrPrintTransparently = Yes;
    return old;
}

/// Ustala szerokość linii. Może byc kosztowne (?). Zwraca ustawienie poprzednie.
[[maybe_unused]]
ssh_natural     line_width(ssh_natural width)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //line_width
    if(ssh_trace_level>2) cout << width << endl;
    auto old = GrLineWidth;

    GrLineWidth = width;
    if(GrLineWidth<0)
        GrLineWidth=1;  // WHY NOT 0!!!
    if(GrLineWidth>3)
    {
        GrLineWidth=3; //SĄ NA TO TYLKO 2 bity w rekordzie!!!
        static int first_time=1;
        if(first_time==1)
        {
            cerr << "Warning! 'line_width' is currently limited to 3 in SVG SymShell!" << endl;
            first_time=0;
        }
    }
    return old;
}

/// \brief Ustala styl rysowania linii: `SSH_LINE_SOLID`, `SSH_LINE_DOTTED`, `SSH_LINE_DASHED`
/// \return Zwraca ustawienie poprzednie.
[[maybe_unused]]
ssh_mode    line_style(ssh_mode Style)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //line_style
    if(ssh_trace_level>2) cout << Style << endl;
    auto old = GrLineStyle;
    GrLineStyle = Style;
    return  GrLineStyle;    //Zwraca poprzedni stan
}

/// \brief Ustala stosunek nowego rysowania do starej zawartości ekranu.
/// \param Style to albo `SSH_SOLID_PUT`, albo `SSH_XOR_PUT`
/// \return Zwraca ustawienie poprzednie.
/// \note W module SVG nie robi nic.
[[maybe_unused]]
ssh_mode    put_style(ssh_mode Style)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //put_style
    if(ssh_trace_level>1) cout << Style << SEP << "IGNORED!" << endl;
    return  SSH_SOLID_PUT;  //Zwraca poprzedni stan
}

/// \brief Zmienia definicje pojedynczego koloru indeksowanego.
/// \param color  indeks koloru
/// \param r     składowa czerwona
/// \param g     składowa zielona
/// \param b     składowa niebieska
///
/// Zmienia definicje koloru indeksowanego o indeksie 'color' w tabeli (palecie) kolorów.
/// Dozwolone indeksy 0..255, bo powyżej jest skala szarości.
/// Do logowania użycia korzysta z maski poziomów śledzenia: 1-msgs 2-grafika 4-alokacje/zwalnianie
/// extern int ssh_trace_level = 0;
[[maybe_unused]]
void set_rgb(ssh_color color,ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    if ( (ssh_trace_level & 4) !=0)//alokacja/zwalnianie (4) //set_rgb
	{
		cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;
		cout << (ssh_color)color << SEP << r << SEP << g << SEP << b << SEP << endl;
	}
	palette[color] = RGB(r,g,b);
}

/// \brief Zmienia definicje pojedynczego odcienia szarości w palecie szarości.
/// \param shade - indeks odcienia szarości
/// \param intensity - intensywność
///
/// Indeksy 0-255 mapowane na 256..511
/// Do logowania użycia korzysta z maski poziomów śledzenia: 1-msgs 2-grafika 4-alokacje/zwalnianie
/// \see extern int ssh_trace_level = 0;
[[maybe_unused]]
void set_gray(ssh_color shade,ssh_intensity intensity)
{
    shade%=256;
    if ( (ssh_trace_level & 4) !=0)//alokacja/zwalnianie (4)
    {
        cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;
        cout << (ssh_color)shade << SEP << intensity << endl;
    }
    palette[256+shade] = RGB(intensity,intensity,intensity);
}

/// \brief Ustala aktualny kolor linii za pomocą indeksu do palety.
/// \param c
/// \param width
/// \param Style
///
/// Ustala aktualny kolor linii za pomocą typu ssh_color (koloru indeksowanego)
/// `ssh_color c` — jest jak dotąd zawsze traktowany jako indeks do tabeli kolorów[1]
/// w których:
/// - pierwsze 256 ustala się wg. jakiejś skali
/// - następne 256 są odcieniami szarości, domyślnie od czarnego do białego
/// Kolory indeksowane w implementacjach _SymShWin_ i _SymShX11_ korzystają z cache'owania systemowych pisaków
/// , żeby było szybciej. A przynajmniej kiedyś dawało to wyraźny zysk.
///
/// Typ `ssh_color` - to uint32 więc ma miejsce na tryb RGBA, ale nigdy nie zostało to zaimplementowane
[[maybe_unused]]
void set_pen(ssh_color c,ssh_natural line_width, ssh_mode Style)

{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_pen
    if(ssh_trace_level>2) cout << (ssh_color)c <<SEP<<line_width<<SEP<<  Style << endl;

    GrLineWidth = line_width;
    if(GrLineWidth<0)
        GrLineWidth=1;

    if(GrLineWidth>3)
    {
        GrLineWidth=3; //SĄ NA TO TYLKO 2 bity w rekordzie!!!
        static int first_time=1;
        if(first_time==1)
        {
            cerr << "Warning! 'line_width' in 'set_pen' is currently limited to 3 in SVG SymShell!" << endl;
            first_time=0;
        }
    }

    GrLineStyle = Style;
    GrPenColor = palette[c];
}

/// \brief Ustala aktualny kolor linii za pomocą składowych RGB.
/// \param r
/// \param g
/// \param b
/// \param width
/// \param Style
///
/// Poza kolorem linii za pomocą składowych RGB ustala też od razu styl, żeby nie mnożyć wywołań.
/// \internal
///     Jeżeli kolory indeksowane korzystają z cache'owania tego samego pisaka to
///     należy ustalić kolor aktualny na pusty np. curr_color=-1;
[[maybe_unused]]
void set_pen_rgb(ssh_intensity r,ssh_intensity g, ssh_intensity b,
                 ssh_natural line_width,ssh_mode Style)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_pen_rgb
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b << SEP << line_width << SEP << Style << endl;

    GrLineWidth = line_width;
    if(GrLineWidth<0)
        GrLineWidth=1;

    if(GrLineWidth>3)
    {
        GrLineWidth=3; //SĄ NA TO TYLKO 2 bity w rekordzie!!!
        static int first_time=1;
        if(first_time==1)
        {
            cerr << "Warning! 'line_width' in 'set_pen_rgb' is currently limited to 3 in SVG SymShell!" << endl;
            first_time=0;
        }
    }

    GrLineStyle = Style;
    GrPenColor.r = r & 0xff;
    GrPenColor.g = g & 0xff;
    GrPenColor.b = b & 0xff;
}

/// \brief Ustala aktualny kolor linii za pomocą składowych RGBA.
/// \param r
/// \param g
/// \param b
/// \param a
/// \param width
/// \param style
///
/// \note W module SVG składowa 'a' jest IGNOROWANA!
[[maybe_unused]]
void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a,
                  ssh_natural line_width,ssh_mode style)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_pen_rgba
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b << SEP << line_width << SEP << style << endl;

    GrLineWidth = line_width;
    if(GrLineWidth<0)
        GrLineWidth=1;

    if(GrLineWidth>3)
    {
        GrLineWidth=3; //SĄ NA TO TYLKO 2 bity w rekordzie!!!
        static int first_time=1;
        if(first_time==1)
        {
            cerr << "Warning! 'line_width' in 'set_pen_rgba' is currently limited to 3 in SVG SymShell!" << endl;
            first_time=0;
        }
    }

    GrLineStyle = style;
    GrPenColor.r = r & 0xff;
    GrPenColor.g = g & 0xff;
    GrPenColor.b = b & 0xff;
  //GrPenColor.a = a & 0xff; //TODO!
}

/// \brief Ustala aktualny kolor wypełnień za pomocą typu ssh_color
/// \param c
///
/// \details ssh_color jest jak dotąd zawsze traktowany jako indeks do tabeli, ale ma miejsce na tryb RGB
///     W modułach Win i X11 kolory indeksowane korzystają z cache'owania systemowych pędzli.
///     Jednak w SVG musi działać samo.
[[maybe_unused]]
void set_brush(ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_brush
    if(ssh_trace_level>2) cout << (ssh_color)c << endl;
    GrBrushColor = palette[c];
}

/// \brief Ustala aktualny kolor wypełnień za pomocą składowych RGB.
/// \param r
/// \param g
/// \param b
///
/// \internal
///     Jeżeli kolory indeksowane korzystają z cache'owanie tego samego pędzla
///   , to należy ustalić kolor aktualny na pusty np. `curr_fill = -1`.
///     W implementacji SVG musi działać samo.
[[maybe_unused]]
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_brush_rgb
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b <<  endl;
    GrBrushColor.r = r & 0xff;
    GrBrushColor.g = g & 0xff;
    GrBrushColor.b = b & 0xff;
}

/// Ustala aktualny kolor wypełnień za pomocą składowych RGBA.
/// Składowa 'a' jest na razie ignorowana.
[[maybe_unused]]
void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,
                   ssh_intensity a)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //set_brush_rgba
    if(ssh_trace_level>2) cout << r << SEP << g << SEP << b <<  endl;
    GrBrushColor.r = r & 0xff;
    GrBrushColor.g = g & 0xff;
    GrBrushColor.b = b & 0xff;
    //GrBrushColor.a = a & 0xff; //TODO!
}

/* ODCZYTYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO
 * **************************************************** */


/// \brief Zwraca `1`, jeśli okno graficzne (lub wirtualne) jest buforowane.
/// \return
/// W przypadku implementacji SVG zawsze zwraca 1
[[maybe_unused]]
ssh_mode  buffered()
{
    if(ssh_trace_level>0) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return yes==1"; //buffered
    if(ssh_trace_level>0) cout << endl;
    return 1;
}

/// \brief Sprawdza, czy okno ma zafiksowana wielkość?
/// W przypadku implementacji SVG zawsze zwraca 1
[[maybe_unused]]
ssh_mode fixed()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return yes===1"; //fixed
    if(ssh_trace_level>2) cout << endl;
    return 1;
}

/// Aktualny kolor tła.
[[maybe_unused]]
ssh_color background()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return " << curr_background; //background
    if(ssh_trace_level>2) cout << endl;
    return curr_background;
}

/// Aktualna grubość linii.
[[maybe_unused]]
ssh_natural get_line_width()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return " << GrLineWidth; //get_line_width
    if(ssh_trace_level>2) cout << endl;
    return GrLineWidth;
}

/// Aktualny kolor linii jako ssh_color (indeks).
/// \note W przypadku implementacji SVG zawsze zwraca -1024 (out of table!)
[[maybe_unused]]
ssh_color get_pen()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //get_pen
    if(ssh_trace_level>2) cout <<"IGNORED"<< endl;
    return -1024;
}

/// Aktualny kolor wypełnień jako `ssh_color`.
/// W przypadku implementacji SVG zawsze zwraca 0 (czarny)
/// Dlaczego nie -1024 (out of table!) ? TODO ?
[[maybe_unused]]
ssh_color get_brush()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //get_brush
    if(ssh_trace_level>2) cout <<"IGNORED"<< endl;
    return 0;
}

/// Aktualne rozmiary pionowe okna z init_plot po przeliczeniach...
/// Oraz ewentualnych zmianach uczynionych "ręcznie" przez operatora
[[maybe_unused]]
ssh_natural screen_height()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return " << GrScreenHi; //screen_height
    if(ssh_trace_level>2) cout << endl;
    return GrScreenHi;
}

/// Aktualne rozmiary poziome okna z init_plot po przeliczeniach...
/// Oraz ewentualnych zmianach uczynionych "ręcznie" przez operatora.
[[maybe_unused]]
ssh_natural screen_width()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "return " << GrScreenWi; //screen_width
    if(ssh_trace_level>2) cout << endl;
    return GrScreenWi;
}

/// Aktualne rozmiary znaku — wysokość.
/// Potrzebne do pozycjonowania tekstu.
[[maybe_unused]]
ssh_natural char_height(char znak)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << znak << SEP << "return " << GrFontHi; //char_height
    if(ssh_trace_level>2) cout << endl;
    return GrFontHi;
}

/// Aktualne rozmiary znaku — szerokość.
/// Potrzebne do pozycjonowania tekstu.
[[maybe_unused]]
ssh_natural char_width(char znak)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << znak << SEP << "return " << GrFontWi; //char_width
    if(ssh_trace_level>2) cout << endl;
    return GrFontWi;
}

/// Aktualne rozmiary wyświetlania całego łańcucha znaków — wysokość.
/// Tu zazwyczaj może być to samo co char_height.
[[maybe_unused]]
ssh_natural string_height(const char* str)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP ; //string_height
    if(ssh_trace_level>2) cout << str <<SEP << "return " << GrFontHi<< endl;
    return GrFontHi;
}

/// Aktualne rozmiary wyświetlania całego łańcucha znaków — szerokość.
/// W najgorszym razie odpowiednia wielokrotność `char_width`
/// Potrzebne do pozycjonowania.
[[maybe_unused]]
ssh_natural string_width(const char* str)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //string_width
    unsigned len = strlen(str);
    if(ssh_trace_level>2) cout << str << SEP << "return " << GrFontWi*len << endl;
    return GrFontWi*len;
}

/*     WYPROWADZANIE TEKSTU
 * ******************************* */

/// \brief Wyświetla tekst w kolorach użytkownika wybranych z palety.
/// \param x
/// \param y
/// \param fore
/// \param back
/// \param format
/// \param ...
[[maybe_unused]]
void printc(int x, int y,ssh_color fore, ssh_color back,const char* format, ...)
{
    //extern int GrPrintTransparently; // = 0;
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //printc
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << fore << SEP << back << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16;
    //                unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.r = palette[back].r;
    Op.text.g = palette[back].g;
    Op.text.b = palette[back].b;
    Op.text.rf = palette[fore].r;
    Op.text.gf = palette[fore].g;
    Op.text.bf = palette[fore].b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;                                                                  assert(format != nullptr);
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);
    va_end(marker);              /* Reset variable arguments.      */
    //if(Op.text.txt != NULL) //Tu niepotrzebne — nowy obiekt powinien być już wyczyszczony.
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}

/// \brief Drukuje czarno na białym.
/// \param x
/// \param y
/// \param format
/// \param ...
[[maybe_unused]]
void printbw(int x,int y,const char* format,...)
{
    // extern int GrPrintTransparently; // = 0;
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //printbw
    if(ssh_trace_level>2) cout << x << SEP << y <<SEP<< format << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16;
    //                unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;

    Op.text.r = 255;
    Op.text.g = 255;
    Op.text.b = 255;
    Op.text.rf = 0;
    Op.text.gf = 0;
    Op.text.bf = 0;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;                                                                  assert(format != nullptr);
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

/// \brief Drukuje w kolorach domyślnych pen & brush.
/// \param x
/// \param y
/// \param format
/// \param ...
[[maybe_unused]]
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...)
{
    //extern int GrPrintTransparently; // = 0;
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //print_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   {   unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8;
    //                  unsigned x  :16; unsigned y :16;
    //                  unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.rf = GrPenColor.r;
    Op.text.gf = GrPenColor.g;
    Op.text.bf = GrPenColor.b;
    Op.text.r = GrBrushColor.r;
    Op.text.g = GrBrushColor.g;
    Op.text.b = GrBrushColor.b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;                                                                  assert(format != nullptr);
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);			                                                  assert(strlen(target) < 2046);
    va_end(marker);              /* Reset variable arguments.      */
    //Op.text.txt.take(clone_str(target));
    //if(Op.text.txt != NULL)
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}


/// \brief Drukuje w kolorze RBG na tle z palety.
/// \param x
/// \param y
/// \param r
/// \param g
/// \param b
/// \param back
/// \param format
/// \param ...
[[maybe_unused]]
void print_rgb(int x, int y,unsigned r, unsigned g, unsigned b,ssh_color back,const char* format, ... )
{
    // extern int GrPrintTransparently; // = 0;
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //print_rgb
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r<<','<<g<<','<<b << SEP << back << SEP
                               << format << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Text;
    //struct Text   {   unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8;
    //                  unsigned x  :16; unsigned y :16;
    //                  unsigned rf :8;  unsigned gf :8; unsigned bf :8; wb_pchar txt; } text;
    Op.text.rf = r;
    Op.text.gf = g;
    Op.text.bf = b;
    Op.text.r = palette[back].r;
    Op.text.g = palette[back].g;
    Op.text.b = palette[back].b;
    Op.text.x = x;
    Op.text.y = y;
    Op.text.mode = GrPrintTransparently;                                                assert(format != nullptr);
    char target[2048];
    va_list marker;
    va_start(marker, format);     /* Initialize variable arguments. */
    vsprintf(target, format, marker);                                           assert(strlen(target) < 2046);
    va_end(marker);              /* Reset variable arguments.      */
    //Op.text.txt.take(clone_str(target));
    //if(Op.text.txt != NULL)
    //		delete Op.text.txt;
    if(ssh_trace_level>0) cout <<"SVG: "<<target<< endl;
    Op.text.txt=clone_str(target);
}

/*   PUNKTOWANIE
 * **************** */

// \brief Wyświetlenie punktu na ekranie (wirtualnym) w kolorze domyślnym pisaka.
[[maybe_unused]]
void plot_d(ssh_coordinate x, ssh_coordinate y)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //plot
    if(ssh_trace_level>2) cout << x << SEP << y << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02; //Plot
    Op.point.r = GrPenColor.r;
    Op.point.g = GrPenColor.g;
    Op.point.b = GrPenColor.b;
    Op.point.x = x;
    Op.point.y = y;
}

/// \brief Wyświetlenie punktu na ekranie w kolorze indeksowanym.
/// \param x
/// \param y
/// \param c : kolor z palety
void plot(int x,int y, ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //plot
    if(ssh_trace_level>2) cout << x << SEP << y << SEP << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02; //Plot
    Op.point.r = palette[c].r;
    Op.point.g = palette[c].g;
    Op.point.b = palette[c].b;
    Op.point.x = x;
    Op.point.y = y;
}

/// Wyświetlenie punktu na ekranie w kolorze true-color.
/// \param x
/// \param y
/// \param r czerwona składowa koloru
/// \param g zielona składowa koloru
/// \param b niebieska składowa koloru
///
void plot_rgb(ssh_coordinate x, ssh_coordinate y,                       /* Współrzędne */
              ssh_intensity r, ssh_intensity g, ssh_intensity b)

{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //plot_rgb
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP << g << SEP << b << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Point;
    //struct Point  { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; } point;
    Op.point.mode = 0x02; //Plot
    Op.point.r = r;
    Op.point.g = g;
    Op.point.b = b;
    Op.point.x = x;
    Op.point.y = y;
}

/// \brief Wypełnia powodziowo lub algorytmem siania w kolorze indeksowanym.
/// \param x współrzędna pozioma punktu startu
/// \param y współrzędna pionowa punktu startu
/// \param fill indeksowany kolor wypełnienia
/// \param border indeksowany kolor granicy wypełniania
///
/// Jednak SVG chyba tego nie ma? TODO CHECK ?
[[maybe_unused]]
void fill_flood(ssh_coordinate x, ssh_coordinate y, ssh_color fill, ssh_color border)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_flood
    if(ssh_trace_level>1) cout << x << SEP << y << SEP
                               <<(ssh_color)fill<< SEP << (ssh_color)border << endl;

    GrOperation& Op = NextGrListEntry_();
    //struct Point { unsigned type : 3; unsigned mode : 5; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rb : 8;  unsigned gb : 8; unsigned bb : 8; } point;
    Op.empty.type = GrType::Point;
    Op.point.mode = 0x1; //FILL
    Op.point.r = palette[fill].r;
    Op.point.g = palette[fill].g;
    Op.point.b = palette[fill].b;
    Op.point.x = x;
    Op.point.y = y;
    Op.point.rb = palette[border].r;
    Op.point.gb = palette[border].g;
    Op.point.bb = palette[border].b;
}

/// \brief Wypełnia powodziowo lub algorytmem siania w kolorze RGB.
/// \param x współrzędna pozioma punktu startu
/// \param y współrzędna pionowa punktu startu
/// \param rf czerwona składowa koloru wypełniania
/// \param gf zielona składowa koloru wypełniania
/// \param bf niebieska składowa koloru wypełniania
/// \param rb czerwona składowa koloru granicy
/// \param gb zielona składowa koloru granicy
/// \param bb niebieska składowa koloru granicy
///
/// Jednak SVG chyba tego nie ma? TODO CHECK ?
[[maybe_unused]]
void fill_flood_rgb(int x,int y,int rf,int gf,int bf,int rb,int gb,int bb)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_flood_rgb
    if(ssh_trace_level>1) cout << x << SEP << y << SEP
                               << rf << SEP << gf << SEP << bf << SEP
                               << rb << SEP << gb << SEP << bb << endl;

    GrOperation& Op = NextGrListEntry_(); //???????
    //struct Point { unsigned type : 3; unsigned mode : 5; unsigned r : 8; unsigned g : 8; unsigned b : 8; unsigned x : 16; unsigned y : 16; unsigned rb : 8;  unsigned gb : 8; unsigned bb : 8; } point;
    Op.empty.type = GrType::Point;
    Op.point.mode = 0x1; //FILL
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

/// \brief Wyświetlenie linii w aktualnym kolorze domyślnym — także RGB.
/// \param x1 pozioma współrzędna startowa
/// \param y1 pionowa współrzędna startowa
/// \param x2 pozioma współrzędna końcowa
/// \param y2 pionowa współrzędna końcowa
void line_d(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //`line_d`
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Line;
    //struct Line   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned w:8;} line;
    Op.line.mode = GrLineStyle;
    Op.line.wi = GrLineWidth;
    Op.line.x1 = x1;
    Op.line.x2 = x2;
    Op.line.y1 = y1;
    Op.line.y2 = y2;
    Op.line.r = GrPenColor.r;
    Op.line.g = GrPenColor.g;
    Op.line.b = GrPenColor.b;
}

/// \brief Wyświetlenie linii w kolorze indeksowanym z palety.
/// \param x1 pozioma współrzędna startowa
/// \param y1 pionowa współrzędna startowa
/// \param x2 pozioma współrzędna końcowa
/// \param y2 pionowa współrzędna końcowa
/// \param c indeks koloru z palety
void line(int x1,int y1,int x2,int y2,ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //line
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Line;
    //struct Line   { unsigned type :3; unsigned mode :5; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16; unsigned w:8;} line;
    Op.line.mode = GrLineStyle;
    Op.line.wi = GrLineWidth;
    Op.line.x1 = x1;
    Op.line.x2 = x2;
    Op.line.y1 = y1;
    Op.line.y2 = y2;
    Op.line.r = palette[c].r;
    Op.line.g = palette[c].g;
    Op.line.b = palette[c].b;
}

/// Wyświetlenie okręgu w kolorze ustawionego pisaka (także rgb).
/// \param x
/// \param y
/// \param r promień okręgu
void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << endl;
    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0; //NO FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = GrPenColor.r;
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
}

/// Wyświetlenie elipsy w kolorze ustawionego pisaka (także rgb).
/// \param x
/// \param y
/// \param a półoś pozioma
/// \param b półoś pionowa
[[maybe_unused]]
void ellipse_d(ssh_coordinate x,ssh_coordinate y, ssh_natural a, ssh_natural b)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;
    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0; //NO FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = GrPenColor.r;
    Op.circle.g = GrPenColor.g;
    Op.circle.b = GrPenColor.b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
}

/// Wyświetlenie elipsy w kolorze indeksowanym.
/// \param x
/// \param y
/// \param a półoś pozioma
/// \param b półoś pionowa
/// \param c kolor z palety
[[maybe_unused]]
void ellipse(ssh_coordinate x,ssh_coordinate y, ssh_natural a, ssh_natural b, ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;
    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0; //NO FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = palette[c].r;
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = a;
    Op.circle.ry = b;
}

/// Wyświetlenie okręgu w kolorze z palety.
/// \param x
/// \param y
/// \param r promień okręgu
/// \param c kolor z palety
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << SEP; //circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0; //NO FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = palette[c].r;
    Op.circle.g = palette[c].g;
    Op.circle.b = palette[c].b;
    Op.circle.x = x;
    Op.circle.y = y;
    Op.circle.rx = r;
    Op.circle.ry = r;
}

/// Wyświetlenie koła w kolorach domyślnych (pen & fill — także rgb).
/// \param x
/// \param y
/// \param r promień okręgu
void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1; //FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = GrPenColor.r; //Prawdopodobnie będzie ignorowane
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

/// Wypełnienie elipsy w kolorach domyślnych (pen & fill — także rgb).
/// \param x
/// \param y
/// \param a półoś pozioma
/// \param b półoś pionowa
__attribute__((unused)) [[maybe_unused]]
void fill_ellipse_d(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_circle_d
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1; //FILL
    Op.circle.wi = GrLineWidth;
    Op.circle.r = GrPenColor.r; //Prawdopodobnie będzie ignorowane
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

/// Wyświetlenie koła w kolorze c z palety
/// \param x
/// \param y
/// \param r promień okręgu
/// \param c kolor z palety
void fill_circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
                 ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << r << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1; //FILL
    Op.circle.wi = 0; // GrLineWidth; TAK NIEKONSEKWENTNIE SIĘ ZACHOWUJE ORYGINAŁ BITMAPOWY TODO?
    Op.circle.r = palette[c].r; //Może będzie ignorowane
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

/// Wypełnienie elipsy kolorem c z palety.
/// \param x
/// \param y
/// \param a półoś pozioma
/// \param b półoś pionowa
/// \param c kolor z palety
[[maybe_unused]]
void fill_ellipse(ssh_coordinate x, ssh_coordinate y, ssh_natural a, ssh_natural b, ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_circle
    if(ssh_trace_level>2) cout << x << SEP << y << SEP
                               << a << SEP << b << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Circle;
    //struct Circle { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8; unsigned x  :16; unsigned y :16; unsigned ra :16; unsigned rb :16; unsigned rf :8; unsigned gf :8; unsigned bf :8;} circle;
    Op.circle.mode = 0x1; //FILL
    Op.circle.wi = 0; // GrLineWidth; TAK NIEKONSEKWENTNIE SIĘ ZACHOWUJE ORYGINAŁ BITMAPOWY TODO?
    Op.circle.r = palette[c].r; //Może będzie ignorowane?
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

/// Not implemented in SVG module.
/// \param x
/// \param y
/// \param r
/// \param start
/// \param stop
[[maybe_unused]]
void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,            /*rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop)
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param r
/// \param start
/// \param stop
/// \param c
[[maybe_unused]]
void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
           ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param a
/// \param b
/// \param start
/// \param stop
[[maybe_unused]]
void earc_d(ssh_coordinate x,ssh_coordinate y,                         /*rysuje łuk eliptyczny */
            ssh_natural a,ssh_natural b,                               /* o półosiach `a` i `b` */
            ssh_radian start,ssh_radian stop)
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param a
/// \param b
/// \param start
/// \param stop
/// \param c
[[maybe_unused]]
void earc(ssh_coordinate x,ssh_coordinate y,
          ssh_natural a,ssh_natural b,
          ssh_radian start,ssh_radian stop,ssh_color c)               /* w kolorze c */
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param r
/// \param start
/// \param stop
/// \param pie
[[maybe_unused]]
void fill_arc_d(ssh_coordinate x, ssh_coordinate y, ssh_natural r,       /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start, ssh_radian stop, ssh_bool pie)         /* początek i koniec łuku */
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param r
/// \param start
/// \param stop
/// \param pie
/// \param c
[[maybe_unused]]
void fill_arc(ssh_coordinate x, ssh_coordinate y, ssh_natural r,         /* wirtualny środek i promień łuku */
              ssh_radian start, ssh_radian stop, ssh_bool pie, ssh_color c)            /* w kolorze c */
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param a
/// \param b
/// \param start
/// \param stop
/// \param pie
[[maybe_unused]]
void fill_earc_d(ssh_coordinate x, ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a, ssh_natural b,                          /* o półosiach `a` i `b` */
                 ssh_radian start, ssh_radian stop, ssh_bool pie)       /* początek i koniec łuku */
{
    //TODO
}

/// Not implemented in SVG module
/// \param x
/// \param y
/// \param a
/// \param b
/// \param start
/// \param stop
/// \param pie
/// \param c
[[maybe_unused]]
void fill_earc(ssh_coordinate x, ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a, ssh_natural b,                            /* o półosiach `a` i `b` */
               ssh_radian start, ssh_radian stop, ssh_bool pie, ssh_color c)           /* w kolorze `c` */
{
    //TODO
}

/// Wypełnienie prostokąta kolorem rgb.
/// \param x1
/// \param y1
/// \param x2
/// \param y2
/// \param r składowa 'r' wypełnienia
/// \param g składowa 'g' wypełnienia
/// \param b składowa 'b' wypełnienia
///
/// \details Prostokąt jest rozciągnięty między rogami x1y1 a x2y2
[[maybe_unused]]
void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,
                   ssh_coordinate x2,ssh_coordinate y2,
                   ssh_intensity r,ssh_intensity g,ssh_intensity b)  /* w kolorze rbg określonym składowymi koloru */
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_rect_d
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1; //FILL
    Op.rect.wi = 0; // GrLineWidth; BACKWARD COMPATIBILITY
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = r; //Może będzie ignorowane? TODO check!
    Op.rect.g = g;
    Op.rect.b = b;
    Op.rect.rf = r; //Kolor wypełnienia
    Op.rect.gf = g;
    Op.rect.bf = b;
}

/// Wypełnia prostokąt w kolorach domyślnych (pen & fill).
/// \param x1
/// \param y1
/// \param x2
/// \param y2
///
/// \details Prostokąt jest rozciągnięty między rogami x1y1 a x2y2
[[maybe_unused]]
void fill_rect_d(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_rect_d
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1; //FILL
    Op.rect.wi = GrLineWidth;
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = GrPenColor.r; //Prawdopodobnie będzie ignorowane
    Op.rect.g = GrPenColor.g;
    Op.rect.b = GrPenColor.b;
    Op.rect.rf = GrBrushColor.r; //Kolor wypełnienia
    Op.rect.gf = GrBrushColor.g;
    Op.rect.bf = GrBrushColor.b;
}

/// Wypełnia prostokąt w kolorze z palety.
/// \param x1
/// \param y1
/// \param x2
/// \param y2
/// \param c : kolor z palety
///
/// \details Prostokąt jest rozciągnięty między rogami x1y1 a x2y2
[[maybe_unused]]
void fill_rect(ssh_coordinate x1, ssh_coordinate y1, ssh_coordinate x2, ssh_coordinate y2, ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_rect
    if(ssh_trace_level>2) cout << x1 << SEP << y1 << SEP
                               << x2 << SEP << y2 << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    Op.empty.type = GrType::Rect;
    //struct Rect   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned x1 :16; unsigned y1:16; unsigned x2 :16; unsigned y2 :16;
    //                unsigned rf :8; unsigned gf :8; unsigned bf :8;} rect;
    Op.rect.mode = 0x1; //FILL
    Op.rect.wi = 0; // GrLineWidth; BACK COMPATIBILITY!!!
    Op.rect.x1 = x1;
    Op.rect.x2 = x2;
    Op.rect.y1 = y1;
    Op.rect.y2 = y2;
    Op.rect.r = palette[c].r; //Prawdopodobnie będzie ignorowane
    Op.rect.g = palette[c].g;
    Op.rect.b = palette[c].b;
    Op.rect.rf = palette[c].r; //Kolor wypełnienia
    Op.rect.gf = palette[c].g;
    Op.rect.bf = palette[c].b;
}

/// Wypełnia wielokąt przesunięty o vx, vy w kolorach domyślnych (pen & fill).
/// \param vx poziome przesunięcie wielokąta
/// \param vy pionowe przesuniecie wielokąta
/// \param points surowa tablica punktów (à la `C`)
/// \param number liczba punktów
[[maybe_unused]]
void fill_poly_d(ssh_coordinate vx, ssh_coordinate vy,
                 const ssh_point points[], int number)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_poly_d
    if(ssh_trace_level>2) cout << vx << SEP << vy << SEP
                               << number << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
                                                                                   assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Poly;
    //struct Poly   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned rf : 8; unsigned gf : 8;unsigned bf :8; wb_dynarray<ssh_point> points; } poly;
    Op.poly.mode = 0x1; //FILL
    Op.poly.wi = GrLineWidth;
    Op.poly.r = GrPenColor.r; //Prawdopodobnie będzie ignorowane
    Op.poly.g = GrPenColor.g;
    Op.poly.b = GrPenColor.b;
    Op.poly.rf = GrBrushColor.r; //Kolor wypełnienia
    Op.poly.gf = GrBrushColor.g;
    Op.poly.bf = GrBrushColor.b;
    //if(Op.poly.points!=NULL)
    //	delete [] Op.poly.points;
    Op.poly.points= new ssh_point[number];
    Op.poly.si=number;
    for (unsigned i = 0; i < number; i++)
    {
        Op.poly.points[i].x = points[i].x + vx;
        Op.poly.points[i].y = points[i].y + vy;
    }
}



/// Wypełnia wielokąt przesunięty o vx, vy kolorem z palety
/// \param vx poziome przesunięcie wielokąta
/// \param vy pionowe przesuniecie wielokąta
/// \param points surowa tablica punktów (à la `C`)
/// \param number liczba punktów
/// \param c kolor z palety
[[maybe_unused]]
void fill_poly(ssh_coordinate vx, ssh_coordinate vy,
               const ssh_point points[], int number,
               ssh_color c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //fill_poly
    if(ssh_trace_level>2) cout << vx << SEP << vy << SEP
                               << number << SEP
                               << (ssh_color)c << endl;

    GrOperation& Op = NextGrListEntry_(); //enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };
    assert(Op.empty.type==GrType::Empty);
    Op.empty.type = GrType::Poly;
    //struct Poly   { unsigned type :4; unsigned mode :4; unsigned r :8; unsigned g :8; unsigned b :8;
    //                unsigned rf : 8; unsigned gf : 8;unsigned bf :8; wb_dynarray<ssh_point> points; } poly;
    Op.poly.mode = 0x1; //FILL
    Op.poly.wi = 0; // GrLineWidth; BACKWARD COMPATIBILITY!
    Op.poly.r = palette[c].r; //Prawdopodobnie będzie ignorowane
    Op.poly.g = palette[c].g;
    Op.poly.b = palette[c].b;
    Op.poly.rf = palette[c].r; //Kolor wypełnienia
    Op.poly.gf = palette[c].g;
    Op.poly.bf = palette[c].b;
    //Op.poly.points.alloc(number);
    //if(Op.poly.points!=NULL)
    //	delete [] Op.poly.points;
    Op.poly.points= new ssh_point[number];
    Op.poly.si=number;
    for (unsigned i = 0; i < number; i++)
    {
        Op.poly.points[i].x = points[i].x + vx;
        Op.poly.points[i].y = points[i].y + vy;
    }
}

/** POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU)\n

Normalnie są to znaki skierowane do okna graficznego i niezwiązane ze
strumieniem wejściowym. W przypadku implementacji na pliku graficznym
można by tu zrobić nieblokujące standardowe wejście, ale chyba bardziej
elastyczny byłby "named pipe" o nazwie zależnej od PID
i nazwy pliku wykonywalnego
 **/

/// \brief Funkcja sprawdza, czy jest do odczytania jakieś zdarzenie wejściowe
/// \return SSH_YES or SSH_NO
[[maybe_unused]]
ssh_mode input_ready()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << GrCharMessage << endl; //input_ready
    if (GrCharMessage >= -1)
        return SSH_YES;
    else
        return SSH_NO;
}

/// Funkcja odczytywania znaków sterowania i zdarzeń.
/// \return znak, jak nie ma czego zwrócić, to zwraca neutralne 0.
/// \details
/// W tym module SVG nigdy nie staje na tej funkcji jak przy zwykłym oknie
[[maybe_unused]]
ssh_msg get_char()
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << GrCharMessage << endl; //get_char
    if (GrCharMessage >= -1)
    {
        int c = GrCharMessage;
        GrCharMessage = -2;
        return c;
    }
    else
        return 0; //Nigdy nie staje na tej funkcji, jak przy zwykłym oknie
    //return -1; //Ale nie -1, bo to oznacza koniec wejścia — np. kliknięcie "zamykacza okna" (co się tu normalnie nie zdarza)
}

/// Odesłanie znaku na wejście
/// \param c znak
/// \return  Zwraca 0, jeśli nie ma miejsca
/// \details Pewne jest tylko odesłanie jednego znaku
[[maybe_unused]]
ssh_stat set_char(ssh_msg c)
{
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << c << endl; //set_char
    if (GrCharMessage < -1)
    {
        GrCharMessage = c;
        return 1; //udało się
    }
    else
        return 0; //Nie udało się, bo poprzednie nie zostało odczytane
}

/// Funkcja odczytująca ostatnie zdarzenie myszy
/// \param xpos miejsce na wpisanie pozycji X
/// \param ypos miejsce na wpisanie pozycji Y
/// \param click miejsce na ewentualny indeks klikniętego przycisku
/// \return 1, jeśli są jakieś dane, a jeśli mysz nieaktywna to 0
/// \details  Można odczytać kiedykolwiek, ale sens ma tylko, gdy `get_char () ` zwróciło znak '\\b'
[[maybe_unused]]
ssh_stat get_mouse_event(ssh_coordinate *xpos, ssh_coordinate *ypos, ssh_coordinate *click)
{
    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << endl; //get_mouse_event
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

/// Podaje obszar okna (/ekranu wirtualnego), który ma byc odrysowany
/// \param x
/// \param y
/// \param width
/// \param height
/// \return 0, jeśli OK.
/// Jeśli zwraca -1 to brak danych lub brak implementacji! Odrysować trzeba całość.
/// Jeśli zwraca -2 to znaczy, że te dane już były wcześniej odczytane. Należy zignorować.
[[maybe_unused]]
ssh_stat repaint_area(int* x,int* y,unsigned* width,unsigned* height)
{
    if(ssh_trace_level>0) cout << "SVG: " << WB_FUNCTION_NAME_ << endl; //repaint_area
    //Nie powinien być raczej używany w module SVG, ale jeżeli już, to prosi o odrysowanie całości, bo tak bezpieczniej
    *x = 0;
    *y = 0;
    *width = GrScreenWi;
    *height = GrScreenHi;
    return -1;
}

/// Jakie są ustawienia RGB konkretnego koloru w palecie
/// \param c indeks koloru (0..511)
/// \return kolor w formacie rbg
[[maybe_unused]]
ssh_rgb get_rgb_from(ssh_color c)
{
	ssh_rgb pom;
    if(ssh_trace_level>2) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << (ssh_color)c << endl; //get_rgb_from
	pom = palette[c];
	return pom;
}

// Wewnętrzna implementacja termicznej skali kolorów,
// czyli wypełnienie palety rgb dla kolorów indeksowanych
[[maybe_unused]]
static void SetScale()
{
#ifndef M_PI
    const double M_PI=3.141595;
#endif

    if(ssh_trace_level>1) cout << "SVG: " << WB_FUNCTION_NAME_ << endl;

    if(UseGrayScale)//Używa skali szarości tam, gdzie normalnie są kolory
    {
        int k;
        for(k=0;k<255;k++)
        {
            long wal=k;
            //`fprintf(stderr,"%u %ul\n",k,wal);`
            set_rgb(k,wal,wal,wal); //Color part
            set_rgb(256+k,wal,wal,wal); //Gray scale part
        }

        if(ssh_trace_level & 4)
           cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "SetScale (0-255 Gray) completed" << endl;
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
            unsigned kk;
            for(kk=256; kk < PALETE_LENGHT; kk++)
                set_rgb(kk, (unsigned char)kk, (unsigned char)kk, (unsigned char)kk );
            if(ssh_trace_level & 4)
               cout << "SVG: " << WB_FUNCTION_NAME_ << SEP << "SetScale (Colors: 0-255; Gray: 256-->" << PALETE_LENGHT << ") completed" << endl;
        }
    }

    set_rgb(255,255,255,255); //Zazwyczaj oczekuje, że kolor 255 to biały albo chociaż jasny
}

/* NAJWAŻNIEJSZE FUNKCJE WEWNĘTRZNE - ZAPIS INFORMACJI DO PLIKU W FORMACIE WEKTOROWYM
 * ********************************************************************************** */

// Zapisuje w formacie "C++stream"
// \param o jakiś wyjściowy strumień C++
// \return 0 chyba że coś padło
static int writeSTR_(ostream& o)
{
    // extern const char* GrFileOutputByExtension; // = "str"; //Tym można sterować format pliku wyjściowego.
	o << "#otx file - objects as text" << endl;
	o << "#enum GrType { Empty = 0, Point=1,LineTo=2,Line=3,Circle=4,Rect=5,Text=6,Poly=7 };" << endl;
	ssh_rgb bac = get_rgb_from( get_background() );
	o << "BACKGROUND=( " << unsigned(bac.r) << ',' << unsigned(bac.g) << ',' << unsigned(bac.b) << " )" << endl;
	o << "GrOpt*[" << GrListPosition + 1 << "] {" << endl;
    if(GrListPosition!=-1)
      for (unsigned i = 0; i <= GrListPosition;i++)
        switch (GrList[i].empty.type)
		{
		case GrType::Empty:	break; //NIE ROBI NIC!
		case GrType::Point: {
			o << "Point" << "\t{\t";
			struct Point &pr = (GrList[i].point);
			o << pr.x << "; " << pr.y << "; 0x"<<hex<<pr.mode <<dec<< "; ";
			if (pr.mode != 0) //MODE == 0 MEANS MoveTo
				o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); "; //COLOR
			if (pr.mode == 1) //FLOOD FILL TO the BORDER
				o << "(" << pr.rb<< ',' << pr.gb<< ',' << pr.bb<< "); ";
			o << " }" << endl;
		} break;
		case GrType::LineTo: {
			o << "#LineTo" << "\t{\t";
			struct LineTo &pr = (GrList[i].lineTo);
			o << "NOT IMPLEMENTED!";
			o << " }" << endl;
		} break;
		case GrType::Line: {
			o << "Line" << "\t{\t";
			struct Line &pr = (GrList[i].line);
			o << pr.x1 << "; " << pr.y1 << "; " << pr.x2 << "; " << pr.y2 << "; "<< pr.wi << "; 0x"  <<hex<< pr.mode<<dec<< "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			o << " }" << endl;
		} break;
		case GrType::Circle: {
			o << "Circle" << "\t{\t";
			struct Ellipse &pr = (GrList[i].circle);
			o << pr.x << "; " << pr.y << "; " << pr.rx << "; " << pr.ry << "; " << pr.wi << "; 0x" << hex << pr.mode << dec << "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		} break;
		case GrType::Rect: {
			o << "Rect" << "\t{\t";
			struct Rect &pr = (GrList[i].rect);
			o << pr.x1 << "; " << pr.y1 << "; " << pr.x2 << "; " << pr.y2 << "; " << pr.wi << "; 0x" << hex << pr.mode << dec << "; ";
			o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 0x1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		} break;
		case GrType::Text: {
			o << "Text" << "\t{\t";
			struct Text &pr = (GrList[i].text);
			o << pr.x << "; " << pr.y << "; 0x" << hex << pr.mode << dec << "; "<<endl;
			o << "\t\t\"" << pr.txt << endl;
			o << "\t\t(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); "; //FOREGROUND
			if(pr.mode!= 0x1 )//WITH FILLED BACKGROUND
				o << "(" << pr.r << ',' << pr.g << ',' << pr.b << "); "; //T�O
			o << " }" << endl;
		} break;
		case GrType::Poly: {
			o << "Poly" << "\t{\t";
			struct Poly &pr = (GrList[i].poly);
			o << pr.wi << "; 0x" << hex << pr.mode << dec << "; " << endl;
			o << "\t\tint2d[" << pr.si << "] {";
			for (unsigned j = 0; j < pr.si; j++)
				o << " (" << pr.points[j].x << ',' << pr.points[j].y << ")";
			o << " }"<<endl;
			o << "\t\t(" << pr.r << ',' << pr.g << ',' << pr.b << "); ";
			if (pr.mode == 0x1)//FILL
				o << "(" << pr.rf << ',' << pr.gf << ',' << pr.bf << "); ";
			o << " }" << endl;
		} break;
		default:
			o << "#unknown type!!! " << GrList[i].empty.type << endl;
			break;
		}
	o << "};\t#End of GrOpt list" << endl;
	return 0;
}

// Zapisuje w formacie SVG
// \param o jakiś wyjściowy strumień C++
// \return 0, chyba że coś padło
static int writeSVG_(ostream& o)
{
    // `extern` const char* GrFileOutputByExtension; // = "str"; //Tym można sterować format pliku wyjściowego.
    // `extern` unsigned GrReloadInterval; // = 1000; //Co ile czasu skrypt w pliku SVG wymusza przeładowanie.
    //                                              Jak 0 to w ogóle nie ma skryptu przeładowania!!!
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
		o << "onload=\"init(evt)\" "; //Sam skrypt może dopiero na końcu? TODO?

	o <<" x=\"0px\" ";
	o <<"y=\"0px\" ";
	o <<" width=\"" << GrScreenWi     << "px\" ";
	o <<" height=\"" << GrScreenHi + 22 <<"px\" >"<<endl; //Trochę dodatkowego miejsca na wirtualnym ekranie na copyright

	//if(GrReloadInterval>0)
	//{
	//o<<"<META HTTP-EQUIV=\"Refresh\" CONTENT=\""<<int(GrReloadInterval/1000)<<"\">\n" //??? Tak to działa w HTMLu, ale w SVG nie bardzo
	//}
	if(GrReloadInterval>0)
	{
		o <<
		"<script type=\"text/ecmascript\"><![CDATA[ "
		"function init(evt){ "
		"setTimeout(function(){ "
       //location.href='http://XXX.XXX.pl'; //gdyby miał ładować coć innego
        "location.reload(1); "
		" }, "<< GrReloadInterval <<" ); "
		"}  ]]></script> "<<endl;
	}

	ssh_rgb bac = get_rgb_from(get_background());
	o << "<rect x=\"0px\" y=\"0px\" width=\"" << GrScreenWi << "px\" height=\"" << GrScreenHi << "px\" rx=\"0\" style=\"fill:"
		//<<"rgb(128,0,128)"<<" "
	  <<"rgb(" << unsigned(bac.r) << ',' << unsigned(bac.g) << ',' << unsigned(bac.b) << "); "
      <<"stroke:#000000; stroke-width:0px;\" />" << endl;

    o << "<text style=\"fill:red;\" x=\""<< 0 <<"\" y=\""<< GrScreenHi + 12 <<"\">This is SVG from "<<ScreenHeader<<" "<<ScreenTitle<<" </text>"<<endl;
    if(GrListPosition!=-1)
      for (unsigned i = 0; i <= GrListPosition; i++)
        switch (GrList[i].empty.type)
		{
		case GrType::Empty:	break; //NIE ROBI NIC!
        case GrType::LineTo: {
                o << "#LineTo" << "\t{\t";
                struct LineTo &pr = (GrList[i].lineTo);
                o << "NOT IMPLEMENTED!";
                o << " }" << endl;
            } break;
        case GrType::Arc: {
                o << "#Arc" << "\t{\t";
                struct Arc &pr = (GrList[i].arc);
                o << "NOT IMPLEMENTED!";
                o << " }" << endl;
            } break;
		case GrType::Point: {
			struct Point &pr = (GrList[i].point);
			if (pr.mode == 0) //MoveTo
			{
				curX = pr.x;
				curY = pr.y;
			}
			else
				if (pr.mode == 1) //FLOOD FILL TO the BORDER
				{
					// TODO https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/flood-color
					// o << " rgb(" << pr.r << ',' << pr.g << ',' << pr.b << "); "; //COLOR
					// o << " rgb(" << pr.rb << ',' << pr.gb << ',' << pr.bb << "); "; //BORDER
				}
				else
				{
					o << "<rect ";
					o << "x=\"" << pr.x << "px\" y=\"" << pr.y << "\" width=\"1px\" height=\"1px\" stroke=\"0px\" "; // << hex << pr.mode << dec << "; ";
					if (pr.mode != 0) //MODE == 0 MEANS MoveTo
						o << "fill=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" "; //COLOR
					o << "/>" << endl;
				}
		} break;
		case GrType::Line: {
			struct Line &pr = (GrList[i].line);
			//o << "<line x1 =\"0\" y1=\"0\" x2=\"100\" y2=\"50\" stroke=\"blue\" stroke-width=\"6\" />" << endl;
			o << "<line x1=\"" << pr.x1 << "px\" y1=\"" << pr.y1 << "px\" x2=\"" << pr.x2 << "px\" y2=\"" << pr.y2 << "px\" ";
			if(pr.wi>0) o << "stroke-width=\"" << pr.wi << "px\" ";
			o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" "; //COLOR
			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		} break;
		case GrType::Circle: {
			struct Ellipse &pr = (GrList[i].circle);
			//o << "<circle cx=\"120\" cy=\"120\" r=\"80\" fill=\"red\" stroke=\"black\" stroke-width=\"5\" />" << endl;
			//o << "<ellipse cx=\"200\" cy=\"200\" rx=\"20\" ry=\"7\" fill=\"none\" stroke=\"black\" stroke-width=\"6\" />" << endl;
            if (pr.rx == pr.ry) //Koło — circle
                o << "<circle r=\"" << pr.ry << "px\" ";
            else {
                o << "<ellipse rx=" << pr.rx << "px\" ry=\"" << pr.ry << "px\" ";
            }

			o << "cx=\"" << pr.x << "px\" cy=\"" << pr.y << "px\" ";

            if (pr.wi > 0)
                o << "stroke-width=\"" << pr.wi << "px\" " << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b
                  << ")\" "; //COLOR
            else {
                o << "stroke=\"none\" ";
            }

            if (pr.mode == 0x1)//FILL
                o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" "; //FILL
            else {
                o << "fill=\"none\" ";
            }
			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		} break;
		case GrType::Rect: {
			struct Rect &pr = (GrList[i].rect);
			o << "<rect "; // x = \"140\" y=\"120\" width=\"250\" height=\"250\" rx=\"40\"
			o << "x=\"" << pr.x1 << "px\" y=\"" << pr.y1 << "px\" width=\"" << pr.x2-pr.x1 << "px\" height=\"" << pr.y2-pr.y1 << "px\" ";

            if (pr.wi > 0)
                o << "stroke-width=\"" << pr.wi << "px\" " << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b
                  << ")\" "; //COLOR
            else {
                o << "stroke=\"none\" ";
            }

			if (pr.mode == 0x1)//FILL
				o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" "; //FILL
			else
				o << "fill=\"none\" ";

			//<< "; 0x" << hex << pr.mode << dec << "; ";
			o << "/>" << endl;
		} break;
		case GrType::Text: {
            struct Text& pr = (GrList[i].text);                                                     //assert(pr.txt != NULL);
            //cerr << '\t' << i << '\t' << pr.type << ' ' << pr.x << ' ' << pr.y;
            //cerr << ' ' << (pr.txt?pr.txt:"NULL") << endl;
            if (pr.txt == nullptr) // TO SIĘ NIE POWINNO ZDARZAĆ, ALE JEDNAK SIĘ ZDARZAŁO!!!
            {
                cerr << '\t' << i << '\t' << pr.type << ' ' << pr.x << ' ' << pr.y << " NULL" << endl;
                pr.txt = clone_str("@?@-NULL-@?@");                                         assert(pr.txt != nullptr);
                //exit(-1);
            }

			auto length = strlen(pr.txt);
			if (!pr.mode)//NOT TRANSPARENTLY
			{
				o << "<rect ";
				o << "x=\"" << pr.x << "px\" y=\"" << pr.y << "px\" width=\"" << length * GrFontWi << "px\" height=\"" << GrFontHi << "px\" "
					<< "stroke-width=\"" << 0 << "px\" ";
				o << "fill=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" />"; //FILL
			}
			auto realFont = (GrFontHi * 4) / 5;
			o << "<text style=\"font-size:" << realFont << "px; fill: rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ");\" ";
			//o << "textLength=\""<< length*GrFontWi <<"px\" ";
			o << "lengthAdjust=\"spacingAndGlyphs\" ";
			o << "x = \"" << pr.x << "\" y=\"" << pr.y + realFont << "\">" << pr.txt << "</text>" << endl;
		} break;
		case GrType::Poly: {
            struct Poly &pr = (GrList[i].poly);
            //	o << "<polygon class =\"MyStar\" fill=\"#3CB54A\"
            //points=\"134.973,14.204 143.295,31.066 161.903,33.77 148.438,46.896 151.617,65.43 134.973, 56.679, 118.329, 65.43 121.507, 46.896 108.042, 33.77 126.65, 31.066\" />" << endl;
            o << "<polygon ";
            if (pr.mode == 0x1)//FILL
                o << "fill=\"rgb(" << pr.rf << ',' << pr.gf << ',' << pr.bf << ")\" "; //FILL
            else
                o << "fill=\"none\" ";
            if (pr.wi > 0)
            {
                o << "stroke-width=\"" << pr.wi << "px\" ";
                o << "stroke=\"rgb(" << pr.r << ',' << pr.g << ',' << pr.b << ")\" "; //COLOR IF EXIST
            }
            else
                o << "stroke=\"none\" ";

			o << "points=\"";
			for (unsigned j = 0; j < pr.si; j++)
					o <<" "<< pr.points[j].x << ',' << pr.points[j].y <<" ";
			o << "\" />" << endl;
		} break;
		default:
			o << "#unknown type!!! " << GrList[i].empty.type << endl;
			break;
		}


	o << "</svg>" << endl;

	return 0;
}

/// Ostateczne uzgodnienie zawartości ekranu realnego z zawartością ekranu wirtualnego/tymczasowego w pamięci.
/// \internal
///     W module SVG zapisuje listę operacji graficznych do pliku o ustalonym formacie (najczęściej SVG)
void flush_plot()
{
    if(GrClosed)
    {
        cerr<<"SYMSHELL graphic is not initialized"<<endl;
        return;
    }

    //GrTmpOutputDirectory ?
    static unsigned flush_counter = 0; //Zliczamy
    flush_counter++; //Jednak nie używamy w nazwie pliku...
    if(ssh_trace_level>0) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP; //flush_plot
    if(ssh_trace_level>0) cout <<'#'<< flush_counter <<SEP<< GrList.get_size() <<SEP<< GrListPosition << endl;
    wb_pchar name(MAX_PATH);
    name.prn("%s%s_%0u", GrTmpOutputDirectory,  ScreenTitle, PID );
    dump_screen(name.get()); //Zapisuje listę operacji graficznych do pliku w ustalonym formacie
}

/// Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy.
/// \param Filename
/// \return wynik funkcji zapisu, zwykle 0
/// \implementation W module SVG dostępne są tekstowe formaty wektorowe, SVG (może kiedyś też EXM?) TODO?
ssh_stat	dump_screen(const char* Filename)
{
    if(ssh_trace_level>0) cout << "SVG: " << WB_FUNCTION_NAME_ << SEP;
    if(ssh_trace_level>0) cout << Filename <<'.'<< GrFileOutputByExtension << endl;

    wb_pchar name(MAX_PATH);

    //Sposób zapisu zależy od rozszerzenia nazwy pliku, ale na razie tworzymy plik tymczasowy
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
        ret = writeSVG_(Out); // local, internal
    }
    else
    {
        ret = writeSTR_(Out); // local, internal
    }

    if (ret)
        return ret; //Gdy błąd?
    else {
        Out.close();
    }


    wb_pchar name2(MAX_PATH);
    //Sposób zapisu zależy od rozszerzenia nazwy pliku
    name2.prn("%s.%s", Filename, GrFileOutputByExtension);

    remove(name2.get()); //Na wypadek, gdyby był już plik o tej nazwie
    ret=rename(name.get(),name2.get());

    return ret;
}

/* *******************************************************************/
/*               SYMSHELLLIGHT version 2026-01-o5                    */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/

