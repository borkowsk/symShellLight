/// @file
/// @brief "Mrówka Langtona"/Langton Ant
/// @date 2026-04-30 (last update)
// -------------------------------------------------------------------------------
#ifdef USE_ENGLISH_IF_POSSIBLE
/// @page page_e7_turmit LANGTON'S ANT IMPLEMENTED IN SYMSHELL
/// @brief A prototype turmite, designed for easy modification.
///
/// @section intro_sec_e7 Description of the "Langton's Ant" Example
///     Example of simple graphics handling featuring screen refreshing and event management,
///     along with a custom context menu that differs from the default.
///
///     A turmite consists of an active element and an environment—specifically a head
///     and memory—since a turmite is essentially a 2-dimensional generalization of
///     a Turing machine.
/// @include Turmit.cpp
#else
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
#endif
//-//////////////////////////////////////////////////////////////////////////////////

#include <cstdio> //Wyjście na konsole à la język C - printf(....)
#include <fstream>

#include "symshell.h"
#include "sshutils.hpp"
#include "../SVG/symshsvgdef.h"

#ifdef USE_ENGLISH_IF_POSSIBLE
#define NAZWA_MODELU  "Turmit_the_fast" ///< Model name. Using "define" makes it easier to mount const character strings.
#else
#define NAZWA_MODELU  "Turmit_przyspieszony" ///< Nazwa modelu. Użycie "define" ułatwia montowanie stałych łańcuchów
#endif

/// @brief Wyjściowy rozmiar świata i "ekranu" symulacji.
/// EN: The output size of the world and the simulation "screen".
const int size=700;

unsigned char World[size][size]; ///< @brief Tablica świata: wyzerowana na początku, bo globalna.
                                 ///<        EN: World table: zeroed at the beginning because it is global.
                                 ///< Jest `unsigned char`, żeby było od 0 do 255, bo typ `char` bywa też "signed"
                                 ///< (zależnie od kompilatora)

unsigned step_counter=0; ///< @brief Licznik realnych kroków modelu. EN:Counter of actually completed model steps.

/// @brief Funkcja do zapoczątkowania świata. Tu pusta, choć można by pokombinować.
/// EN: A function to start the world. It's empty here, though it could be tweaked.
void init_world()
{
    //World[0][0]=255; //TODO: ewentualna inicjalizacja świata (losowo?)
}

/// @brief Struktura definiująca stan turmita.
/// EN: The structure defining the turmite state.
struct Turmit
{
    int x,y;  //!< położenie/position.
    int stan; //!< Pamięć robocza "głowicy", czyli aktualny kierunek. EN:Working memory of the "head", i.e. the current direction.
    Turmit(int ix,int iy,int is):x(ix),y(iy),stan(is) ///< KONSTRUKTOR.
    {}
};

/// @brief Struktura anonimowa dla kierunków ruchu. Kolejność góra, prawo, dół, lewo.
/// EN:Anonymous structure for movement directions. Order: up, right, down, left.
struct
{
    int dx,dy;
} Directions[4]={{0,-1},{1,0},{0,1},{-1,0}};


#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Setting the language for on-screen "subtitles".
///        User variable for the `lang` or `_lingo` function set.
///        A value of 0 indicates Polish, 1 English, and 2 has never been used.
unsigned int lang_selector=1;

/// @brief Variable representing the turmite. Initialize the turmite in the center with the direction "up".
#else
/// @brief Ustalenie języka dla napisów na ekranie.
///        Zmienna użytkownika dla zestawu funkcji `lang` albo `_lingo`.
///        Wartość 0 oznacza język polski, 1 angielski, 2 nigdy nie była używana.
unsigned int lang_selector=0;

/// @brief Zmienna reprezentująca turmita. Inicjalizacja turmita na środku z kierunkiem "góra".
#endif
Turmit LaAnt(size/2,size/2,0);

/// @brief Funkcja robiąca jeden krok symulacji.
/// EN: A function that performs one simulation step.
void single_step()
{
    LaAnt.x=( LaAnt.x+Directions[LaAnt.stan].dx //Przesunięcie po X
            + size ) % size; //i zabezpieczenie, żeby nie wyjść za tablicę
    LaAnt.y=( LaAnt.y+Directions[LaAnt.stan].dy //Przesuniecie po Y
            + size ) % size; //i zabezpieczenie, żeby nie wyjść za tablicę
    
    if(World[LaAnt.y][LaAnt.x]==0)
    {
        plot(LaAnt.x,LaAnt.y,128);
        World[LaAnt.y][LaAnt.x]=1;  //Może być inna liczba. To tylko marker odwiedzin
        LaAnt.stan=(LaAnt.stan+1)%4; //Mrówka jest czterokierunkowa
    }
    else
    {
        plot(LaAnt.x,LaAnt.y,255);
        World[LaAnt.y][LaAnt.x]=0;
        LaAnt.stan=(LaAnt.stan+4-1)%4; //Mrówka jest czterokierunkowa
    }

    step_counter++; //Licznik kroków mrówki
}

/// @brief Funkcja do obliczenia statystyk.
/// EN: Function to calculate statistics.
void stats()
{
  //TODO: Np średniej liczby odwiedzeń już odwiedzonych oraz liczby pustych.
}

// Do wizualizacji obsługi zdarzeń:
// EN: To visualize event handling:
const int DELA=0;          ///< @brief Jak długie oczekiwanie w obrębie pętli zdarzeń.
                           ///< EN: Specifies the length of the wait within the event loop.
const int VISUAL=1000;     ///< @brief Co ile kroków symulacji odrysowywać widok.
                           ///< EN: Number of simulation steps between full screen refreshes.
const char* CZEKAM=lang("Tylko patrz!","Just look!"); ///< @brief Prompt in event loop. EN: Prompt in event loop.

int x_mouse=10,y_mouse=10; ///< UNUSED FOR NOW.

//Kilka deklaracji zapowiadających inne funkcje obsługujące model:
//EN: A few declarations announcing other model handling functions:
void replot();         // Funkcja odrysowująca
void read_mouse();     // Obsługa myszy. Używać, o ile potrzebne!
void write_to_file();  // Obsługa zapisu do pliku. Używać, o ile potrzebne!
void screen_to_file(); // Zapis ekranu do pliku

/// @brief Rysuje na ekranie. EN: Restores the entire contents of the window.
void replot()
{
    invalidate_screen();
    for(int x=0;x<size;x++)
        for(int y=0;y<size;y++)
        {
            unsigned z=World[y][x]*20; //Co tam było? Wzmocnione
            z%=256; //Żeby nie przekroczyć kolorów
            // z %= 512; //Albo wersja  z szarościami
            plot(x,y,z); //Rysowanie punktu "świata"
        }
    printc(size/3,size,128,255,"%06u  ",step_counter); //Licznik kroków
    //Ostatnie położenie kliku — biały krzyżyk
    //line(x_mouse, y_mouse-10, x_mouse, y_mouse+10,255);
    //line(x_mouse-10, y_mouse, x_mouse+10, y_mouse,255);
}

/** @brief Specyficzna dla tego programu definicja menu kontekstowego.
 *  EN: A context menu definition specific to this program. */
ssh_menu_item_definition  context_menu_default[]= {
        {lang("ZAPISZ TEKST","TO TEXT"), 'p'},
        {lang("ZAPISZ GRAFIKĘ","DUMP GRAPHIC"), 'd'},
        {lang("KONIEC","QUIT"), 'q'}
};
unsigned context_menu_default_size= sizeof(context_menu_default) / sizeof(context_menu_default[0]);


/** @brief Główna funkcja. Potrzebne są parametry wywołania programu do ustawień symulacji i grafiki.
 *  EN: Main function. Program call parameters are needed for simulation and graphics settings. */
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
            case '\b': read_mouse();break; //Jest zdarzenie myszy
            case EOF:  //Typowe zakończenie
            case  27:  //ESC
            case 'q':  //Zakończenie zdefiniowane przez programistę
            case 'Q': not_finished=false;break;
            default:
                printbw(screen_width()/2,screen_height()-char_height('C'),lang("Co znaczy %c [%d] ?","What means  %c [%d] ?"),pom,pom);
                printf(lang("Nie wiem, co znaczy %c [%d] ","I don't know what means %c [%d] "),pom,pom);
                flush_plot();	// Grafika gotowa
                break;
            }
        }
        else //Symulacja jako akcja na wypadek braku zdarzeń do obsługi
        {
            single_step(); //Następny krok symulacji
            if(step_counter%VISUAL==0) //Odrysuj "świat", gdy reszta z dzielenia równa 0
            {
                flush_plot(); //Żeby to, co rysowane w trakcie choć przez chwilę błysnęło
                replot(); //Pełny replot() jest najbardziej kosztowny!
                printc(0,screen_height()-char_height('C'),128,(step_counter%3?255:250),CZEKAM);
                flush_plot(); // obraz gotowy. Właściwie "co chwila" wywoływane.
                delay_ms(DELA); //Wymuszenie oczekiwania, żeby aktywna pętla nie zjadała całego czasu procesora
            }
        }
    }

    printf(lang("Wykonano %d obrotów pętli.\n",
                "%d loop iterations completed.\n"),loop);

    close_plot();/* Zamykamy okno i jest po zabawie. EN: We close the window and the fun is over. */

    printf("%s",lang("Do widzenia!\n","Goodbye!\n"));
    return 0;
}

/// @brief SZKIELETOWA procedura obsługi myszy. EN: SKELETON mouse handler.
void read_mouse()
{ 
    int x_pos,y_pos,click;
    if(get_mouse_event(&x_pos, &y_pos, &click) != -1)//Operator & - pobranie adresu
    {
        x_mouse=x_pos;y_mouse=y_pos;
        //TODO - zaimplementować jeśli potrzebne
        //...
    }
}

/// @brief Zapis stanu modelu do pliku. EN: Saving the model state to a file. TODO !!!
void write_to_file()
{
    const char* NazwaPliku= NAZWA_MODELU ".out"; // Używamy sztuczki ze zlepianiem stałych.
                                                 // łańcuchowych przez kompilator
    std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry

    //TODO - funkcja powinna zapisać wyniki modelu do pliku zamiast wyrysowywać na ekranie
    //       Format: tabela liczb odpowiedniego typu rozdzielanych tabulacjami
    //       EN: The function should save the model results to a file instead of plotting them on the screen.
    //       Format: a table of tab-delimited numbers of the appropriate type.
    //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;

    out.close();
}

/// @brief Zapis ekranu do pliku. EN: Saving the screen to a file
void screen_to_file()
{
    char bufor[255]; //Tymczasowe miejsce na utworzenie nazwy pliku
    sprintf(bufor, "%s%06u", NAZWA_MODELU, step_counter); //Nazwa + Numer kroku na 6 polach
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
