/// @file
/// @brief "Mrówka Langtona" z możliwością wariacji na temat (turmit prototypowy)
///                   (przykładowy program SYMSHELL-a)
// -------------------------------------------------------------------------------
/// @date 2026-04-24 (last update)
/// @page page_e7_turmit MRÓWKA LANGTONA ZAKODOWANA W SYMSHELL-u
/// @brief Turmit prototypowy, łatwy do modyfikacji
///
/// @section intro_sec_e7 Opis przykładu "Mrówka Langtona"
///     Prosta obsługa grafiki, ale z odtwarzaniem ekranu i obsługą zdarzeń
///     , oraz własne menu kontekstowe, różne od domyślnego.
///
///     Turmit ma element aktywny i środowisko, tzw. głowicę albo "czoło" oraz pamięć
///     , ponieważ turmit jest tak naprawdę 2 wymiarowym uogólnieniem maszyny Turinga
/// @include Turmit.cpp
//-//////////////////////////////////////////////////////////////////////////////////


#include <stdio.h> //Wyjście na konsole à la język C - printf(....)
//#include <math.h>
#include <fstream>

#if defined(_MSC_VER)
//#pragma warning(disable:4068)
#pragma warning(disable : 4996) //deprecated functions
#pragma warning(disable : 4521) //multiple copy constructor
#pragma warning(disable : 4522) //multiple assigment operator
//TYMCZASEM - OSTRZEŻENIA O "conversion from 'A' to 'B', possible loss of data"
//#pragma warning(disable : 4267)
//#pragma warning(disable : 4244)
#endif
#include "symshell.h"

#define NAZWA_MODELU  "Turmit_przyspieszony" //Użycie define ułatwia montowanie stałych łańcuchów

/// Wyjściowy rozmiar świata i "ekranu" symulacji.
const int size=700;

unsigned char World[size][size]; ///< Tablica świata: wyzerowana na początku, bo globalna.
                                 ///< Jest `unsigned char`, żeby było od 0 do 255, bo typ `char` bywa też "signed"
                                 ///< (zależnie od kompilatora)

unsigned step_counter=0; ///< Licznik realnych kroków modelu

void init_world() ///< Funkcja do zapoczątkowania świata.
{
    //World[0][0]=255;//TODO: ewentualna inicjalizacja świata (losowo?)
}

/// Struktura definiująca stan turmita.
struct Turmit
{
    int x,y;//położenie
    int stan;//I pamięć robocza. Np. aktualny kierunek
    Turmit(int ix,int iy,int is):x(ix),y(iy),stan(is) //KONSTRUKTOR TURMITA!
    {}
};

/// Struktura anonimowa dla kierunków ruchu. Kolejność góra, prawo, dół, lewo.
struct
{
    int dx,dy;
} Directions[4]={{0,-1},{1,0},{0,1},{-1,0}};

Turmit LaAnt(size/2,size/2,0); ///< Nasz turmit. Inicjalizacja turmita na środku z kierunkiem "góra".

void single_step() ///< Funkcja robiąca jeden krok symulacji
{
    LaAnt.x=( LaAnt.x+Directions[LaAnt.stan].dx //Przesunięcie po X
            + size ) % size; //i zabezpieczenie, żeby nie wyjść za tablicę
    LaAnt.y=( LaAnt.y+Directions[LaAnt.stan].dy //Przesuniecie po Y
            + size ) % size; //i zabezpieczenie, żeby nie wyjść za tablicę
    
    if(World[LaAnt.y][LaAnt.x]==0)
    {
        plot(LaAnt.x,LaAnt.y,128);
        World[LaAnt.y][LaAnt.x]=1;  //Może być inna liczba. To tylko marker odwiedzin
        LaAnt.stan=(LaAnt.stan+1)%4;//Mrówka jest czterokierunkowa
    }
    else
    {
        plot(LaAnt.x,LaAnt.y,255);
        World[LaAnt.y][LaAnt.x]=0;
        LaAnt.stan=(LaAnt.stan+4-1)%4;//Mrówka jest czterokierunkowa
    }

    step_counter++;//Licznik kroków mrówki
}

void stats() ///< Funkcja do obliczenia statystyk
{
  //TODO: Np średniej liczby odwiedzeń już odwiedzonych oraz liczby pustych.
}

//Do wizualizacji obsługi zdarzeń:
const int DELA=0;      ///< Jak długie oczekiwanie w obrębie pętli zdarzeń.
const int VISUAL=1000; ///< Co ile kroków symulacji odrysowywać widok.
const char* CZEKAM="Tylko patrz! "; ///< Monit w pętli zdarzeń.
int x_mouse=10,y_mouse=10; ///< Pozycja ostatniego "kliku" myszy (na razie zbędne).

//Kilka deklaracji zapowiadających inne funkcje obsługujące model:
void replot();         // Funkcja odrysowująca
void read_mouse();     // Obsługa myszy. Używać, o ile potrzebne!
void write_to_file();  // Obsługa zapisu do pliku. Używać, o ile potrzebne!
void screen_to_file(); // Zapis ekranu do pliku

void replot() ///< Rysuje na ekranie
{
    for(int x=0;x<size;x++)
        for(int y=0;y<size;y++)
        {
            unsigned z=World[y][x]*20; //Co tam było? Wzmocnione
            z%=256; //Żeby nie przekroczyć kolorów
            // z %= 512; //Albo wersja  z szarościami
            plot(x,y,z); //Rysowanie punktu "świata"
        }
    printc(size/3,size,128,255,"%06u  ",step_counter);//Licznik kroków
    //Ostatnie położenie kliku — biały krzyżyk
    //line(x_mouse, y_mouse-10, x_mouse, y_mouse+10,255);
    //line(x_mouse-10, y_mouse, x_mouse+10, y_mouse,255);
}

/** Własna definicja menu kontekstowego. */
ssh_menu_item_definition  context_menu_default[]= {
        {"TO TEXT", 'p'},
        {"DUMP GRAPHIC", 'd'},
        {"QUIT", 'q'}
};
unsigned context_menu_default_size= sizeof(context_menu_default) / sizeof(context_menu_default[0]);

/** Główna funkcja. Potrzebne są parametry wywołania programu do ustawień symulacji i grafiki. */
int main(int argc,const char* argv[])
{
    fix_size(1);        // Czy udajemy, że ekran ma zawsze taki sam rozmiar?
    mouse_activity(0);  // Czy mysz będzie obsługiwana?
    buffering_setup(1); // Czy będzie pamiętać w bitmapie zawartość ekranu? PAMIĘTANIE PRZYŚPIESZA!
    shell_setup(NAZWA_MODELU, argc, argv); // Przygotowanie okna z użyciem parametrów wywołania
    init_plot(size,size,0,1); // Otwarcie okna SIZE x SIZE pikseli + 1 wiersz znaków za pikselami

    // Teraz można rysować i pisać w oknie
    init_world();
    replot();
    flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa

    bool not_finished=true; //Zmienna sterująca zakończeniem programu
    unsigned loop=0;        //Do zliczania nawrotów pętli zdarzeń

    while(not_finished)    //PĘTLA OBSŁUGI ZDARZEŃ
    {
        int pom; //NA ZNAK Z WEJŚCIE OKNA GRAFICZNEGO
        loop++;
        if(step_counter%(VISUAL/10)==0  //Dla przyśpieszenia tylko czasami sprawdzamy
        && input_ready()) //Czy jest zdarzenie do obsługi?
        {
            pom=get_char(); //Przeczytaj nadesłany znak
            switch(pom)
            {
            case '\0': /* do nothing */ break;
            case 'D':
            case 'd': screen_to_file();break; //"Zrzut" grafiki
            case 'p': write_to_file();break; //Zapis do pliku tekstowego
            case '\r': replot(); flush_plot();break; //Wymagane odrysowanie
            case '\b': read_mouse();break;//Jest zdarzenie myszy
            case EOF:  //Typowe zakończenie
            case  27:  //ESC
            case 'q':  //Zakończenie zdefiniowane przez programistę
            case 'Q': not_finished=false;break;
            default:
                printbw(screen_width()/2,screen_height()-char_height('C'),"Co znaczy %c [%d] ?",pom,pom);
                printf("Nie wiem, co znaczy %c [%d] ",pom,pom);
                flush_plot();	// Grafika gotowa
                break;
            }
        }
        else //Symulacja jako akcja na wypadek braku zdarzeń do obsługi
        {
            single_step(); //Następny krok symulacji
            if(step_counter%VISUAL==0) //Odrysuj "świat", gdy reszta z dzielenia równa 0
            {
                flush_plot();//Żeby to, co rysowane w trakcie choć przez chwilę błysnęło
                replot();//Pełny replot() jest najbardziej kosztowny!
                printc(0,screen_height()-char_height('C'),128,(step_counter%3?255:250),CZEKAM);
                flush_plot();// obraz gotowy. Właściwie "co chwila" wywoływane.
                delay_ms(DELA);//Wymuszenie oczekiwania, żeby aktywna pętla nie zjadała całego czasu procesora
            }
        }
    }

    printf("Wykonano %d obrotów pętli.\nDo widzenia!\n",loop);
    close_plot();/* Zamykamy okno; jest po zabawie */
    printf("Do widzenia!\n");
    return 0;
}

void read_mouse() ///< Procedura obsługi myszy. SZKIELETOWA!
{ 
    int x_pos,y_pos,click;
    if(get_mouse_event(&x_pos, &y_pos, &click) != -1)//Operator & - pobranie adresu
    {
        x_mouse=x_pos;y_mouse=y_pos;
        //TODO - zaimplementować jeśli potrzebne
        //...
    }
}

void write_to_file() ///< Zapis stanu modelu do pliku. SZKIELET!
{
    const char* NazwaPliku= NAZWA_MODELU ".out"; // Używamy sztuczki ze zlepianiem stałych.
                                                 // łańcuchowych przez kompilator
    std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry
    //TODO - funkcja powinna zapisać wyniki modelu do pliku zamiast wyrysowywać na ekranie
    //Format - tabela liczb odpowiedniego typu rozdzielanych tabulacjami
    //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;

    out.close();
}

void screen_to_file() ///< Zapis ekranu do pliku.
{
    char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
    sprintf(bufor, "%s%06u", NAZWA_MODELU, step_counter);//Nazwa + Numer kroku na 6 polach
    dump_screen(bufor);
}

/* *******************************************************************/
/*                SYMSHELLLIGHT  version 2026                        */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
