/* SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++    */
/************************************************************************/
/* designed by W.Borkowski from University of Warsaw                    */
/* https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI              */
/* https://github.com/borkowsk                                          */
/* File changed masively: 15.11.2020                                    */

#ifndef _SYMSHELL_H_
#define _SYMSHELL_H_

#ifdef __cplusplus
extern "C" {
const unsigned  PALETE_LENGHT=512;
const unsigned  SSH_SOLID_TEXT=0;
const unsigned  SSH_TRANSPARENT_TEXT=1;
const unsigned  SSH_SOLID_PUT=1;
const unsigned  SSH_XOR_PUT=2;
const unsigned  SSH_LINE_SOLID=1;
const unsigned  SSH_LINE_DOTTED=2;
const unsigned  SSH_LINE_DASHED=3;
const unsigned  SSH_YES=1;
const unsigned  SSH_NO=0;
#else
#define PALETE_LENGHT      (512)
#define SSH_SOLID_TEXT       (0)
#define SSH_TRANSPARENT_TEXT (1)
#define SSH_SOLID_PUT        (1)
#define SSH_XOR_PUT          (2)
#define SSH_LINE_SOLID       (1)
#define SSH_LINE_DOTTED      (2)
#define SSH_LINE_DASHED      (3)
#define SSH_YES    (1)
#define SSH_NO     (0)
#endif

/* TYPY */
typedef unsigned char                           uchar8b;       /* MUST HAVE 8 bits! */
typedef uchar8b                                 ssh_bool;      /* 0 or 1 */
typedef int                                     ssh_msg;       /* Character from keyboard or other special numbers, especially from menu */
typedef signed   int                            ssh_mode;      /* Only symbols defined above expected! */
typedef signed   int                            ssh_coordinate;/* Wszelkie wspołrzędne ekranowe */
typedef unsigned int                            ssh_natural;   /* Liczby większe od zera gdy zero jest sytuacją nieoczekiwaną */
typedef unsigned int                            ssh_intensity; /* Składowe kolorów itp. wartości od 0 wzwyż */
typedef struct ssh_point {ssh_coordinate x,y;}  ssh_point;     /* Punkt we współrzędnych ekranowych */
typedef struct ssh_rgb   {uchar8b r,g,b;}       ssh_rgb;       /* alpha? union with uint32? TODO */
typedef unsigned int                            ssh_color;     /* ssh_color_index? TODO */
typedef float                                   ssh_radian;   /* katy w radianach dla łuków */

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacja */
void shell_setup(const char* title,                            /* Nazwa aplikacji używana jako tytuł okna lub jego część */
                 int iargc,const char* iargv[]);               /* Przekazanie parametrow wywolania */

void set_title(const char* title);                             /* Change window title bar */

void buffering_setup(ssh_mode Yes);                            /* Przelaczanie buforowanie okna - moze nie dzialac po inicjacji*/
void fix_size(ssh_mode Yes);                                   /* Czy symulować niezmiennosc rozmiarow okna */

                                                               /* Zmienia definicje koloru w palecie kolorow. Indeksy 0..255 */
void set_rgb(ssh_color color,                                  /* - indeks koloru */
             ssh_intensity r,ssh_intensity g,ssh_intensity b); /*- wartości składowych */
void set_gray(ssh_color shade,ssh_intensity intensity);        /* Zmiania definicje odcienia szarosci w palecie szarosci. Indeksy 256..511 */

void set_background(ssh_color c);                              /* Ustala index koloru do czyszczenia - moze nie dzialac po inicjacji*/

                                                               /* inicjacja grafiki/semigrafiki */
int  init_plot(ssh_natural  a,ssh_natural   b,                 /* ile pikseli mam mieć okno */
               ssh_natural ca, ssh_natural cb);                /* ile dodatkowo lini i kolumn tekstu na dole i po bokach przy domyślnej czcionce */

void close_plot(void);                                         /* zamkniecie grafiki/semigrafiki */
                                                               /* Automatycznie instalowana w atexit - stad durne (void) zeby uniknac warningu */
                                                               /* Do sterowania close_plot() - czy wymaga potwierdzenia od użtkownika */
extern int WB_error_enter_before_clean/* =0 */;                /* Czy zamykac od razu czy dac szanse na przejrzenie zawartosci */

/* OPERACJE DOTYCZACE CALEGO OKNA GRAFICZNEGO */
void delay_ms(ssh_natural ms);                                 /* Wymuszenie oczekiwania przez pewną liczbę milli seconds */
void delay_us(ssh_natural ms);                                 /* Wymuszenie oczekiwania przez pewną liczbę micro seconds */
void flush_plot();                                             /* Ostateczne uzgodnienie zawartosci ekranu z zawartoscia pamieci */
void clear_screen();                                           /* Czysci ekran lub ekran wirtualny */
int  invalidate_screen();                                      /* W sposób ukryty zapomina poprzednią zawartość ekranu gdy liczymy że i tak zostałaby zamazana */
int  dump_screen(const char* Filename);                        /* Zapisuje zawartosc ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
                                                               /* Może nie działać w trybie bez buforowania okna/ekranu */
/* Operacje przestawiania wlasnosci pracy okna graficznego */
ssh_mode    mouse_activity(ssh_mode Yes);                      /* Ustala czy mysz ma byc obslugiwana. Zwraca poprzedni stan flagi */
ssh_mode    print_transparently(ssh_mode Yes);                 /* Wlacza drukowanie tekstu bez zamazywania tła. Zwraca stan poprzedni */
ssh_natural line_width(ssh_natural width);                     /* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni WHAT ABOUT 0?*/
ssh_mode    line_style(ssh_mode Style);                        /* Ustala styl rysowania lini: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
ssh_mode    put_style(ssh_mode Style);                         /* Ustala stosunek nowego rysowania do starej zawartosci ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */

void set_pen(ssh_color c, ssh_natural width, ssh_mode style);    /* Ustala aktualny kolor linii i konturów za pomoca typu ssh_color oraz styl i grubość */
void set_brush(ssh_color c);                                   /* Ustala aktualny kolor wypelnien za pomoca typu ssh_color */
void set_pen_rgb(ssh_intensity r, ssh_intensity g, ssh_intensity b, /* Ustala aktualny kolor linii za pomoca skladowych RGB */
                    ssh_natural width, ssh_mode style);         /*  oraz styl i grubość */
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b); /* Ustala aktualny kolor wypelnien za pomoca skladowych RGB */

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEN OKNA GRAFICZNEGO*/
ssh_mode  buffered();                                           /* Zwraca 1 jesli buforowane */
ssh_mode  fixed();                                              /* Czy okno ma zafiksowana wielkosc */

ssh_rgb   get_rgb_from(ssh_color c);                            /* Jakie są ustawienia RGB konkretnego kolorku w palecie */
ssh_color background();                                         /* Aktualny kolor tla... */
ssh_color get_pen();                                            /* Aktualny kolor linii jako ssh_color */
ssh_color get_brush();                                          /* Aktualny kolor wypelnien jako ssh_color */

ssh_natural  screen_height();                                   /* Aktualne rozmiary okna po przeliczeniach z init_plot*/
ssh_natural  screen_width();                                    /*  ...i ewentualnych zmianach uczynionych "recznie" przez operatora */
ssh_natural  get_line_width();                                  /* Aktualna grubosc linii */

/* Wielkości znaków i tekstów */
ssh_natural  char_height(char znak);                            /* Aktualne rozmiary znaku  */
ssh_natural  char_width(char znak);                             /* ...potrzebne do pozycjonowania tekstu */
ssh_natural  string_height(const char* str);                    /* Aktualne rozmiary lancucha */
ssh_natural  string_width(const char* str);                     /* ...potrzebne do jego pozycjonowania */

/* WYPROWADZANIE TEKSTU */
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);/* Tekst w oknie w kolorze brush()*/
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);/* Tekst w oknie czarno na bialym*/
void printc(ssh_coordinate x,ssh_coordinate y,                         /* Tekst w oknie w zadanych kolorach */
            ssh_color fore,                                            /* - kolor tekstu */
            ssh_color back,                                            /* - kolor tła jeśli drukuje nietransparentnie */
            const char* format,...);
void print_rgb(ssh_coordinate x,ssh_coordinate y,                      /* Drukuje z możliwością ustawienia tuszu poprzez RGB */
           ssh_intensity r,ssh_intensity g,ssh_intensity b,            /*- składowe koloru tekstu */
           ssh_color back,                                             /*- kolor tła jeśli drukuje nietransparentnie */
           const char* format,...);

/* PUNKTOWANIE  */
void plot_rgb(ssh_coordinate x,ssh_coordinate y,                       /* Współrzędne */
              ssh_intensity r,ssh_intensity g,ssh_intensity b);        /*- składowe koloru */
void plot_d(ssh_coordinate x,ssh_coordinate y);                        /* Wyswietlenie punktu na ekranie w kolorze domyslnym */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);             /* Wyswietlenie punktu w kolorze z palety */

void fill_flood(ssh_coordinate x,ssh_coordinate y,                     /* Wypelnia powodziowo lub algorytmem siania */
                ssh_color fill,                                        /*- kolor wypełnienia */
                ssh_color border);                                     /*- kolor brzegu */
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,
                ssh_intensity rf,ssh_intensity gf,ssh_intensity bf,    /*- kolor wypełnienia podany składowymi */
                ssh_intensity rb,ssh_intensity gb,ssh_intensity bb);   /*- kolor brzegu podany składowymi */

/* RYSOWANIE LINI */
void line_d(ssh_coordinate x1,ssh_coordinate y1,                       /* Wyswietlenie lini w kolorze domyslnym */
            ssh_coordinate x2,ssh_coordinate y2);                      /* od punktu x1y1 do x2y2 */
void line(ssh_coordinate x1,ssh_coordinate y1,                         /* Wyswietlenie lini w zadanym kolorze */
          ssh_coordinate x2,ssh_coordinate y2,                         /* od punktu x1y1 do x2y2 */
          ssh_color c);                                                /* - kolor dla linii */

/* RYSOWANIE KÓŁ, ELIPS i ŁUKÓW */
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c);/* Wyswietlenie okregu w kolorze c */
void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r);        /* Wyswietlenie okregu w kolorze domyslnym */

void ellipse_d(ssh_coordinate x,ssh_coordinate y,                      /* Wyswietlenie elipsy w kolorze domyslnym */
               ssh_natural a,ssh_natural b);                           /* o PÓŁOSIACH dlugości a i b */
void ellipse(ssh_coordinate x,ssh_coordinate y,                        /* Wyswietlenie elipsy w kolorze c */
             ssh_natural a,ssh_natural b, ssh_color c);                /* o PÓŁOSIACH dlugości a i b */

void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,            /*rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop);
void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
           ssh_radian start,ssh_radian stop,ssh_color c);              /* w kolorze c */
void earc_d(ssh_coordinate x,ssh_coordinate y,                         /*rysuje łuk eliptyczny */
            ssh_natural a,ssh_natural b,                               /* o półosiach a i b */
            ssh_radian start,ssh_radian stop);
void earc(ssh_coordinate x,ssh_coordinate y,
          ssh_natural a,ssh_natural b,
          ssh_radian start,ssh_radian stop,ssh_color c);               /* w kolorze c */

void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r);   /* Wypełnienie kola o promieniu r w kolorach domyslnych*/
void fill_circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
                 ssh_color c);                                         /* Wypełnienie kola w kolorze c */

void fill_ellipse_d(ssh_coordinate x,ssh_coordinate y,                 /* Wypełnienie elipsy w kolorach domyslnych*/
                    ssh_natural a,ssh_natural b);                      /* o PÓŁOSIACH dlugości a i b */
void fill_ellipse(ssh_coordinate x,ssh_coordinate y,                   /* Wypełnienie elipsy w kolorze c */
                  ssh_natural a,ssh_natural b,                         /* o PÓŁOSIACH dlugości a i b */
                  ssh_color c);                                        /* i kolorze c */

void fill_arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,       /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start,ssh_radian stop,ssh_bool pie);        /* początek i koniec łuku */
void fill_arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,         /* wirtualny środek i promień łuku */
              ssh_radian start,ssh_radian stop,                        /* początku i końcu */
              ssh_bool pie,ssh_color c);                               /* w kolorze c */

void fill_earc_d(ssh_coordinate x,ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a,ssh_natural b,                          /* o półosiach a i b */
                 ssh_radian start,ssh_radian stop,ssh_bool pie);       /* początek i koniec łuku */
void fill_earc(ssh_coordinate x,ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a,ssh_natural b,                            /* o półosiach a i b */
               ssh_radian start,ssh_radian stop,                       /* początku i końcu */
               ssh_bool pie,ssh_color c);                              /* w kolorze c */

/* WIELOKĄTY */
void fill_rect_d(ssh_coordinate x1, ssh_coordinate y1,                 /* Wypełnienie prostokata w kolorach domyslnych*/
                 ssh_coordinate x2, ssh_coordinate y2);                /* rozciągniętego między rogami x1y1 a x2y2 */
void fill_rect(ssh_coordinate x1,ssh_coordinate y1,                    /* Wypełnienie prostokata w kolorze c */
               ssh_coordinate x2,ssh_coordinate y2,ssh_color c);       /* rozciągniętego między rogami x1y1 a x2y2 */

void fill_poly_d(ssh_coordinate vx,ssh_coordinate vy,                  /* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
                const ssh_point points[],int number);                  /* - tablica wierzchołków wielokąta i jej długość */
void fill_poly(ssh_coordinate vx,ssh_coordinate vy,                    /* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
                const ssh_point points[],int number,                   /* - tablica wierzchołków wielokąta i jej długość */
                ssh_color c);                                          /* - kolor wypełnienia */

/* POBIERANIE ZNAKOW Z KLAWIATURY i ZDAZEN OKIENNYCH (w tym z MENU) */
ssh_mode  input_ready(); /* Funkcja sprawdzajaca czy jest cos do wziecia z wejscia */
ssh_msg   get_char();    /* Funkcja odczytywania znakow sterowania i zdarzen */
int  set_char(ssh_msg c);/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
                         /* Pewne jest tylko odeslanie jednego znaku. */

int  get_mouse_event(ssh_coordinate* xpos,ssh_coordinate* ypos,ssh_coordinate* click);/* Funkcja odczytujaca ostatnie zdazenie myszy */
int  repaint_area(ssh_coordinate* x, ssh_coordinate* y,
                  ssh_natural* width, ssh_natural* height); /* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
                                                        /* Jesli zwraca -1 to brak danych lub brak implementacji ! Odrysowac calosc. */
                                                        /* Jesli zwraca -2 to znaczy ze dane juz zostaly odczytane. Nalezy zignorowac. */

#ifdef __cplusplus
} //extern C
#endif

#ifdef __cplusplus

static_assert( sizeof(uchar8b)==1 , "uchar8b has more than 1 byte" );

inline
ssh_rgb RGB(ssh_intensity r,ssh_intensity g,ssh_intensity b)/// DO PRZENIESIENIA DO symshell.h TODO!
{
    ssh_rgb po;
    po.r=(uchar8b)(r & 0xff);
    po.g=(uchar8b)(g & 0xff);
    po.b=(uchar8b)(b & 0xff);
    return po;
}

// Temporary implementation set_background RGB
inline
void set_background(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{   // TODO!
    set_background(r);
}

// Konwerter na referencje żeby nie trzeba było używać adresów
inline
int get_mouse_event(int& xpos,int& ypos,int& click)
{
    return get_mouse_event(&xpos,&ypos,&click);
}

inline
int  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

// FUNKCJE PRZESTARZALE DOSTEPNE TYLKO DLA KOMPILATORA C++
inline ssh_mode   get_buffering(){ return 	buffered(); }   // Zwraca 1 jesli buforowane
inline ssh_mode   get_fixed(){ return fixed(); }                  // Czy okno ma zafiksowana wielkosc
inline ssh_color  get_background(void){ return background(); }    // Aktualny kolor tla...
#endif

#endif /* _SYMSHELL_H_ */

/********************************************************************/
/*              SYMSHELLLIGHT  version 2020-11-17                   */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
