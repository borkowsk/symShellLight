//-//////////////////////////////////////////////////////////////////////////////
/// @file
///  Szablon automatu komórkowego zastosowany do algorytmu dyfuzji.
///                   (przykładowy program SYMSHELL'a)
///------------------------------------------------------------------------------
/// Prosta obsługa grafiki, ale z odtwarzaniem ekranu i obsługą zdarzeń.
/// Linux:
///     Wymagane -lX11 -lXpm
/// Windows:
///     VC++ linkuje biblioteki Windows automatycznie
///     Dev-Cpp potrzebne są dwie bibloteki:
///     ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
/// @date 2026-01-27 (last update)
//-//////////////////////////////////////////////////////////////////////////////
//#define MULTITR (1)  //Jeśli chcemy użyć wielowątkowości

#include <cassert>
#include <cstdio>    //Wyjście na konsole w stylu języka C - printf(....)
#include <cmath>     //Trochę funkcji matematycznych z języka C
#include <ctime>     //do pomiaru czasu

#include <fstream>   // C++ strumienie plikowe
#include <iostream>  // C++ obsługa konsoli

#ifdef MULTITR
#include <thread>    //Obsluga wątków
const unsigned max_threads=16;//Maksymalna przewidziana liczba wątków
unsigned prefered_threads=8; //Ile wątków domyślnie?

//Ziarno dla wątkowego generatora liczb pseudolosowych
#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
#pragma warning(disable : 4244) //time_t --> unsigned int
__declspec(thread) //thread_local z C++11 powinno działać ale w VC++2012 jednak tego nie robi
#else
thread_local 
#endif
unsigned long  myholdrand=1;

int myrand()//Na wzór rand() Microsoftu
{
    return( ((myholdrand = myholdrand * 214013L
            + 2531011L) >> 16) & 0x7fff );
}
#endif

#include "symshell.h"
#include "wb_cpucl.hpp"
#include "optParam.hpp"
cticker MyCPUClock;         //Czas od startu programu do liczenia "średniego czasu kroku brutto"

#define NAZWAMODELU  "2x2 gas v1.0mt " //Użycie define, a nie const char* ułatwia montowanie stałych łańcuchów

//Wyjściowy rozmiar świata i "ekranu" symulacji
const int MAXSIDE=1000;
int     curr_side=500;//Raczej powinno być parzyste!

//Do wizualizacji obsługi zdarzeń
const char* CZEKAM="?>"; //Monit w pętli zdarzeń
const int DELA=0;//Jak długie oczekiwanie w obrębie pętli zdarzeń
unsigned VISUAL=1;//Co ile kroków symulacji odrysowywać widok
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

time_t RANDOM_SEED=time(NULL);    //Zarodek generatora pseudolosowego 
unsigned DENSITY=(curr_side*curr_side)/100; //Musi być tyle, żeby były miejsca z komórkami obok siebie
                                  //TODO: manipulacja gęstością
                        
wbrtm::OptionalParameterBase* Parameters[]={ //sizeof(Parameters)/sizeof(Parameters[])
new wbrtm::ParameterLabel("PARAMETERS FOR SINGLE SIMULATION"),
new wbrtm::OptionalParameter<unsigned>(DENSITY,1,10000,"DENSITY","How many particles"),
new wbrtm::OptionalParameter<int>(curr_side,0,MAXSIDE,"SIDELEN","Side of the world"),
new wbrtm::ParameterLabel("TECHNICAL PARAMETERS"),
new wbrtm::OptionalParameter<unsigned>(VISUAL,1,10000,"VISUAL","How many steps between visualisation"),
#ifdef MULTITR
new wbrtm::OptionalParameter<unsigned>(prefered_threads,1,max_threads,"THREADS","How many threads"),
#else
new wbrtm::OptionalParameter<time_t>(RANDOM_SEED,1,0x1FFFFFF,"RANDSEED", //Do zasiewanie liczby losowej
                        "Use, if you want the same simulation many times. Do not work with multithreated version"),
#endif
new wbrtm::ParameterLabel("END OF LIST")
};    


//Klasyczny automat komórkowy zmienia stan wszystkich komórek "jednocześnie"
//co w praktyce oznacza, że potrzebna jest jedna tablica na stan aktualny 
//i jedna na przyszły.
typedef unsigned char WorldType[MAXSIDE][MAXSIDE]; //Zadeklarowanie takiego typu pomaga obejść
                                             //pokrętny sposób deklaracji wskaźnika do tablicy
//unsigned char żeby było od 0 do 255, bo typ char bywa też "signed" (zaleznie od kompilatora)
//Dodatkowa zaleta że wystarczy zmienić ten typedef i zmieniają się wszystkie zmienne 
//tego typu.

WorldType World1;//Pierwsze tablica świata - wyzerowana na poczatku bo globalna
WorldType World2;//Druga tablica świata - j.w.
WorldType* World=(&World1);//Aktualna tablica świata, a właściwie adres do niej

unsigned step_counter=0;//Licznik realnych kroków modelu

void init_world()
//Funkcja do "zapoczątkowania świata"
//Wsypujemy trochę jedynek w losowe miejsca używając rand() z C RTL
{
    srand(RANDOM_SEED); //Inicjacja generatora liczb pseudolosowych
    for(unsigned k=0;k<DENSITY;k++)
    {
        unsigned i=rand() % curr_side; //% operacja reszta z dzielenia
        unsigned j=rand() % curr_side; //czyli "modulo". Chmmm... *
        World1[i][j]=1; //Gdzieniegdzie coś ma być...
    }
    // * Robienie modulo z wynikiem funkcji rand() jest podobno odradzane przez znawców :-)
    //   Przy tak małym "świecie" może jednak nie zobaczymy problemu
}

//Funkcja do zamykania świata w torus
inline //<-- To funkcja "rozwijana w kodzie" 
unsigned BezpiecznyOffset(int offset,unsigned start,unsigned WYMIAR)
{
    //Operacje na typie BEZ ZNAKU! Główne założenie: nigdy nie przejść poniżej zera!!!!!
    unsigned pom=WYMIAR;//Bezpieczna "głębia"
    pom=pom+start+offset;//Problem byłby tylko, gdy offset ujemny i ABS(offset)>WYMIAR+start
    //... ale to by była duża złośliwość :-)
    //Jednak teraz na pewno wychodzi poza WYMIAR od strony dodatniej
    pom=pom % WYMIAR;    //To załatwia "reszta z dzielenia", którą i tak byśmy robili
    return pom;
}

unsigned char Rules[16][7]={
    { 0, 0, 0, 0, 0, 0, 0},// --  0000 -> 0x0
                           // --
    { 1, 8, 4, 2, 1, 8, 4},// --  0001 --> 0x1
                           // -x
    { 1, 4, 2, 1, 8, 4, 2},// --  0010 --> 0x2
                           // x-
    { 2,12,10, 9, 6, 5, 3},// --  0011 --> 0x3
                           // xx
    { 1, 2, 1, 8, 4, 2, 1},// -x  0100 --> 0x4
                           // --
    { 2,10, 9, 6, 5, 3,12},// -x  0101 --> 0x5
                           // -x
    { 2, 9, 6, 5, 3,12,10},// -x  0110 --> 0x6
                           // x-
    { 3,14,13,11, 7,14,13},// -x  0111 --> 0x7
                           // xx
    { 1, 1, 8, 4, 2, 1, 8},// x-  1000 --> 0x8
                           // --
    { 2, 6, 5, 3,12,10, 9},// x-  1001 --> 0x9
                           // -x
    { 2, 5, 3,12,10, 9, 6},// x-  1010 --> 0xa czyli 10
                           // x-
    { 3,13,11, 7,14,13,11},// x-  1011 --> 0xb czyli 11
                           // xx
    { 2, 3,12,10, 9, 6, 5},// x-  1100 --> 0xc czyli 12
                           // x-
    { 3,11, 7,14,13,11, 7},// xx  1101 --> 0xd czyli 13
                           // -x
    { 3, 7,14,13,11, 7,14},// xx  1110 --> 0xe czyli 14
                           // x-
    { 4,15,15,15,15,15,15} // xx  1111 --> 0xf czyli 15
                           // xx
};

//DEFINICJA FUNKCJI RegulaIZmiana()
//Będącej właściwą implementacją automatu. Tu się ustala stan aktualny oraz wybiera następny
inline //<-- To funkcja "rozwijana w kodzie"
void RegulaIZmiana( unsigned i, //Wiersz startowej komórki bloku
                    unsigned j, //Kolumna startowej komórki
                    WorldType& SW,//Z jakiego świata?
                    WorldType& TW //Do jakiego świata
                                  //,unsigned& rstate //stan generatora rand_r //TODO PO CO ?
                            )
{
    unsigned le=(j+1)%curr_side; //Zamknięcie w torus
    unsigned bo=(i+1)%curr_side; //jest uproszczone...
    unsigned char old=(SW[i][ j]&1)*8 + (SW[bo][ j]&1)*4
            +(SW[i][le]&1)*2 + (SW[bo][le]&1)*1;                  assert(old<16);
    //unsigned randVal=4;               //Test bez rand()
#ifdef MULTITR
    unsigned randVal=myrand(); //używa wielowątkowo specyficznego stanu czyi każdy watek ma inny ciąg pseudolosowy
#else
    unsigned randVal=rand();   //Czy rand() jest jakoś zabezpieczone względem wątków? OGÓLNIE WĄTPIE. W  MSVC++ nie
#endif

    unsigned char nex=old==0?0:old==15?15:Rules[old][1+randVal%6];//Sprawdzenie stanu, ale 0 i 15 nie mają szans na zmianę
                                                                  assert(nex<16);
                                                                  assert(Rules[old][0] == Rules[nex][0]);
    TW[ i][ j]= nex&8?1:0;
    TW[bo][ j]= nex&4?1:0;
    TW[ i][le]= nex&2?1:0;
    TW[bo][le]= nex&1?1:0;
}

#ifdef MULTITR
void doMove(int StartLine,int EndLine,int Parity, WorldType* pSW,WorldType* pTW)
{
    //printf("%u %u\n",StartLine,EndLine);fflush(stdout);
    myholdrand=time(NULL)+(EndLine*StartLine);//Jakieś zróżnicowane źródło ziarna

    for(unsigned i=StartLine;i<=EndLine;i+=2)
    {                                                              assert(i<size);
        for(unsigned j=Parity;j<size;j+=2)
            RegulaIZmiana(i,j,*pSW,*pTW);
    }
}
#endif

void single_step()
//Funkcja robiąca jeden SYNCHRONICZNY krok symulacji
{
    //Ustalenie co jest stare a co nowe tym razem
    WorldType* OldWorld=NULL;//Dla pewności pusty wskaźnik
    unsigned parity=step_counter%2;
    if(parity==0)//Dla wyjścia z kroku 0 i parzystych
    {
        OldWorld=(&World1);World=(&World2);//Świat pierwszy jest źródłem danych a nowym światem (wynikiem) będzie drugi świat
    }
    else //Dla wyjścia z kroku 1 i nieparzystych
    {
        OldWorld=(&World2); World=(&World1);//Świat drugi jest źródłem danych a nowym światem będzie ponownie pierwszy świat
    }

    //Teraz właściwe wyliczenie nowego stanu automatu
#ifdef MULTITR
    std::thread theThreads[max_threads];
    int lines_per_thr=(size/2)/prefered_threads;//Tylko co druga linia może być startową
    int rest_lines=(size/2)%prefered_threads;//Wątek pierwszy dostaje całą resztę
    int fl=parity;
    int ll=fl+(rest_lines+lines_per_thr)*2-1;//ll to indeks ostatniej linii
    for(int t=0;t<prefered_threads;t++)
    {
        theThreads[t]=std::thread(doMove,fl,ll,parity,OldWorld,World);//T1: doMove(parity,size/pref-1,parity,OldWorld,World);
        fl=ll+1;ll=fl+lines_per_thr*2-1;
        //if(ll>=size) ll=size-1;//To ostatnie możliwa linia
    }
    //Główny będzie sobie czekał
    for(int t=0;t<prefered_threads;t++)
        theThreads[t].join();
#else
    for(unsigned i=parity;i<curr_side;i+=2)
        for(unsigned j=parity;j<curr_side;j+=2)
            RegulaIZmiana(i,j,*OldWorld,*World);
#endif
    step_counter++;
}

//Statystyki i ich liczenie oraz wyświetlanie
unsigned alife_counter=0;//Licznik żywych komórek. Do użycia też w wyświetlaniu, więc globalny

#ifdef MULTITR
void doStat(int StartLine,int EndLine,unsigned& Summ)
{
    //printf("%u %u\n",StartLine,EndLine);fflush(stdout);
    unsigned tmp=0;
    for(int y=StartLine;y<=EndLine;y++)//Tylko wskazana część świata
        for(int x=0;x<size;x++)//World jest wskaźnikiem na tablicę, więc trzeba go użyć jak
            if((*World)[y][x]!=0)     //wskaźnika, a dopiero potem jak tablicy. Nawias konieczny.
                tmp++;
    Summ=tmp;
}
#endif

void statistics()
//Funkcja do obliczenia statystyk
{
    alife_counter=0;
#ifdef MULTITR
    //const unsigned max_threads=16;//Maksymalna przewidziana liczba wątków
    //unsigned prefered_threads=3; //Ile wątków domyślnie?
    unsigned part_alife_counters[max_threads];
    std::thread* theThreads[max_threads];
    int lines_per_thr=size/prefered_threads;
    int rest_lines=size%prefered_threads;
    for(int fl=0,ll=lines_per_thr-1,t=0;t<prefered_threads;t++)//ll to indeks ostatniej lini
    {
        part_alife_counters[t]=0;//
        if( rest_lines-- > 0)
            ll++; //Puki jest reszta z dzielenia, rozkładamy ją na wątki które wystartują pierwsze
        theThreads[t]=new std::thread(doStat,fl,ll,std::ref(part_alife_counters[t]));
        fl=ll+1;ll+=lines_per_thr;
    }
    //Główny będzie sobie czekał
    for(int t=0;t<prefered_threads;t++)
    {
        theThreads[t]->join();
        alife_counter+=part_alife_counters[t];//Musi być po join!
        delete theThreads[t];
    }
#else
    for(int x=0;x<curr_side;x++)
        for(int y=0;y<curr_side;y++)
        {          //World jest wskaźnikiem na tablicę, więc trzeba go użyć jak
            //wskaźnika, a dopiero potem jak tablicy. Nawias konieczny.
            if((*World)[y][x]!=0)
                alife_counter++;
        }
#endif
    printc(curr_side/2,curr_side,200,64,"%06u  ",alife_counter);//Licznik kroków
    // std::cout<<step_counter<<"      \t"<< alife_counter <<std::endl;
}

//TODO 3: zapis do pliku...

//Kilka deklaracja zapowiadających inne funkcje obsługujące model
void replot(); //Funkcja odrysowująca
void read_mouse(); // Obsługa myszy. Używać tylko gdy potrzebne (spowalnia)
void write_to_file(); // Obsługa zapisu do pliku.  Używać tylko gdy potrzebne (spowalnia)
void screen_to_file(); //Zapis ekranu do pliku

void replot()
//Rysuje coś na ekranie
{
    for(int x=0;x<curr_side-1;x++)
        for(int y=0;y<curr_side-1;y++)
        {          //World jest wskaźnikiem na tablicę, więc trzeba go użyć jak
            //wskaźnika, a dopiero potem jak tablicy. Nawias konieczny.
            unsigned z=(*World)[y][x]*200;//Spodziewana wartość to 0 lub 1
            z%=256; //Żeby nie przekroczyć kolorów
            //z%=512; //Albo z szarościami
            plot(x,y,z);
        }
    printc(curr_side/5,curr_side,128,255,"%06u MstT:%g  ",
           step_counter,(double)MyCPUClock/step_counter);//Licznik kroków
    //Ostatnie położenie kliku: biały krzyżyk
    //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
    //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne są parametry wywołania programu
{
    printf("Model \"%s\". File version %s\n",NAZWAMODELU,__TIMESTAMP__);
    if(wbrtm::OptionalParameterBase::parse_options(argc,argv,Parameters,sizeof(Parameters)/sizeof(Parameters[0])))
    {
        exit(222);
    }
#ifdef MULTITR
    printf("Number of hardware concurent contexts: %d\n",std::thread::hardware_concurrency());
    printf("Default number of threads: %d\n\n",prefered_threads); //Ile wątków domyślnie?
    myholdrand=2;     //INICJACJA myrand() dla głównego wątku - pewnie nieistotna
#endif
    fix_size(1);       // Czy udajemy, że ekran ma zawsze taki sam rozmiar?
    mouse_activity(0); // Czy mysz będzie obsługiwana?
    buffering_setup(1);// Czy będzie rysować poprzez bitmapę z zawartością ekranu?
    shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z użyciem parametrów wywołania
    init_plot(curr_side,curr_side,0,1);// Otwarcie okna SIZExSIZE pikseli + 1 wiersz znaków za pikselami

    // Teraz można rysować i pisać w oknie
    init_world();  //Tu jest też wywołanie srand();
    replot();
    statistics();
    flush_plot();	// Ekran (wirtualny) po inicjalizacji jest już gotowy
    //screen_to_file();//ODKOMENTOWAĆ jak chcemy materiał do filmu

    bool not_finished=true;//Zmienna sterująca zakończeniem programu
    unsigned loop=0;    //Do zliczania nawrotów pętli zdarzeń
    while(not_finished) //PĘTLA OBSŁUGI ZDARZEŃ
    {
        int pom; //NA ZNAK Z WEJŚCIE OKNA GRAFICZNEGO
        loop++;
        if(input_ready()) //Czy jest zdarzenie do obsługi?
        {
            pom=get_char(); //Przeczytaj nadesłany znak
            switch(pom)
            {
            case 'd': screen_to_file();break;//Zrzut grafiki
            case 'p': write_to_file();break;//Zapis do pliku tekstowego
            case '\r': replot();break;//Wymagane odrysowanie
            case '\b': read_mouse();break;//Jest zdarzenie myszy
            case EOF:  //Typowe zakończenie
            case  27:  //ESC
            case 'q':  //Zakończenie zdefiniowane przez programistę
            case 'Q':
                        not_finished=false;break;// Wymuszenie zakończenia pętli
            default:
                printbw(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
                printf("Nie wiem co znaczy %c [%d] ",pom,pom);
                flush_plot();	// Grafika gotowa
                break;
            }
        }
        else //Symulacja - jako akcja na wypadek braku zdarzeń do obsługi
        {
            single_step(); //Następny krok
            if(step_counter%VISUAL==0) //Odrysuj, gdy reszta z dzielenia równa 0
            {
                replot();
                statistics(); //Uwaga nie "stat" bo to funkcja z RTL języka C!
                if(loop%3==0)
                    printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
                else
                    printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
                flush_plot();// Ekran gotowy. Właściwie co chwila wywoływane
                //screen_to_file();//ODKOMENTOWAĆ jak chcemy materiał do filmu
            }

            delay_ms(DELA);//Wymuszenie drobnego oczekiwania (szansa na przełączenie procesów),
                           //żeby pętla aktywna nie zjadała całego czasu CPU
        }
    }

    printf("Wykonano %d obrotów pętli.\nDo widzenia!\n",loop);
    close_plot();// Zamykamy okno. Już po zabawie.
    printf("Do widzenia!\n");
    return 0;
}

void read_mouse() //Procedura obsługi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      xmouse=xpos;ymouse=ypos;
      //TODO - zaimplementować jeśli będzie potrzebne
      //...
   }                      
}

void write_to_file()
{
    const char* NazwaPliku=NAZWAMODELU ".out";//Używamy sztuczki ze zlepianiem stałych
                                              //łańcuchowych przez kompilator
    std::cout<<"Zapis stanu do pliku \""<<NazwaPliku<<'"';
    std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry
    //TODO - funkcja powinna zapisać wyniki modelu do pliku
    //Format - tabela liczb odpowiedniego typu rozdzielanych tabulacjami
    //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
    for(int x=0;x<curr_side;x++)
    {
        for(int y=0;y<curr_side;y++)
        {   //World jest wskaźnikiem na tablicę, więc trzeba go użyć jak
            //wskaźnika, a dopiero potem jak tablicy. Nawias konieczny.
            unsigned z=(*World)[y][x];//Spodziewana wartość to 0 lub 1
            out<<z<<'\t';
        }
        out<<'\n';
    }
    out.close();
    std::cout<<std::endl;
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{  
    char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
#ifdef _MSC_VER /*MSVC*/
    _snprintf_s(bufor,255,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#else
    sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#endif
    std::cout<<"Zapis ekranu do pliku \""<<bufor<<'"';
    dump_screen(bufor);
    std::cout<<std::endl;
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
