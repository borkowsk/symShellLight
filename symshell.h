/**       SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++          */
/* *********************************************************************/
/** \file SYMSHELL.H                                                    *
 * \author W.Borkowski from University of Warsaw                        *
 * \n https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI           *
 * \n https://github.com/borkowsk                                       */
/* File changed massively: 15.11.2020                                   */

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
typedef signed   int                            ssh_stat;      /* Values returned as status for some functions */
typedef signed   int                            ssh_coordinate;/* Wszelkie współrzędne ekranowe */
typedef unsigned int                            ssh_natural;   /* Liczby większe od zera, gdy zero jest sytuacją nieoczekiwaną */
typedef unsigned int                            ssh_intensity; /* Składowe kolorów itp. wartości od 0 wzwyż */
typedef struct ssh_point {ssh_coordinate x,y;}  ssh_point;     /* Punkt we współrzędnych ekranowych */
typedef struct ssh_rgb   {uchar8b r,g,b;}       ssh_rgb;       /* alpha? Union with uint32? TODO */
//typedef struct ssh_rgba  {uchar8b r,g,b,a;}     ssh_rgba;    /* With alpha. Union with uint32? TODO */
typedef unsigned int                            ssh_color;     /* ssh_color_index? TODO */
typedef float                                   ssh_radian;    /* katy w radianach dla łuków */

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */
void shell_setup(const char* title,                            /* Nazwa aplikacji używana jako tytuł okna lub jego część */
                 int iargc,const char* iargv[]);               /* Przekazanie parametrów wywołania */

void set_title(const char* title);                             /* Change window title bar */

void buffering_setup(ssh_mode Yes);                            /* Przełączanie buforowanie okna. Może nie działać po inicjacji*/
void fix_size(ssh_mode Yes);                                   /* Czy symulować niezmienność rozmiarów okna */

                                                               /* Zmienia definicje koloru w palecie kolorów. Indeksy 0..255 */
void set_rgb(ssh_color color,                                  /* - indeks koloru */
             ssh_intensity r,ssh_intensity g,ssh_intensity b); /*- wartości składowych */
void set_gray(ssh_color shade,ssh_intensity intensity);        /* Zmiana definicje odcienia szarości w palecie szarości. Indeksy 256..511 */

void set_background(ssh_color c);                              /* Ustala index koloru do czyszczenia. Może nie działać po inicjacji*/

                                                               /* inicjacja grafiki/semigrafiki */
ssh_stat  init_plot(ssh_natural  a,ssh_natural   b,            /* ile pikseli mam mieć okno */
                    ssh_natural ca, ssh_natural cb);           /* ile dodatkowo linii i kolumn tekstu na dole i po bokach przy domyślnej czcionce */
                                                               /* Zwraca 1, jeśli zadziałał poprawnie */
extern unsigned long _ssh_window;                              /* If not 0 the window is usable */

void close_plot(void);                                         /* zamkniecie grafiki/semigrafiki */
                                                               /* Automatycznie instalowana w atexit, stąd durne (void), żeby uniknął warning-u */
                                                               /* Do sterowania close_plot() - czy wymaga potwierdzenia od użytkownika */
extern int WB_error_enter_before_clean/* =0 */;                /* Czy zamykać od razu, czy dać szanse na przejrzenie zawartości */

/* OPERACJE DOTYCZACE CALEGO OKNA GRAFICZNEGO */
void delay_ms(ssh_natural ms);                                 /* Wymuszenie oczekiwania przez pewną liczbę milli seconds */
void delay_us(ssh_natural us);                                 /* Wymuszenie oczekiwania przez pewną liczbę micro seconds */
void flush_plot();                                             /* Ostateczne uzgodnienie zawartości ekranu z zawartością pamięci */
void clear_screen();                                           /* Czyści ekran lub ekran wirtualny */
ssh_stat  invalidate_screen();                                 /* W sposób ukryty zapomina poprzednią zawartość ekranu, gdy liczymy, że i tak zostałaby zamazana */
ssh_stat  dump_screen(const char* Filename);                   /* Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */
                                                               /* Może nie działać w trybie bez buforowania okna/ekranu */
/* Operacje przestawiania własności pracy okna graficznego */
ssh_mode    mouse_activity(ssh_mode Yes);                      /* Ustala czy mysz ma byc obsługiwana. Zwraca poprzedni stan flagi */
ssh_mode    print_transparently(ssh_mode Yes);                 /* Włącza drukowanie tekstu bez zamazywania tła. Zwraca stan poprzedni */
ssh_natural line_width(ssh_natural width);                     /* Ustala szerokość linii. Może być kosztowne. Zwraca stan poprzedni WHAT ABOUT 0?*/
ssh_mode    line_style(ssh_mode Style);                        /* Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
ssh_mode    put_style(ssh_mode Style);                         /* Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */

void set_pen(ssh_color c, ssh_natural width, ssh_mode style);  /* Ustala aktualny kolor linii i konturów za pomocą typu ssh_color oraz styl i grubość */
void set_brush(ssh_color c);                                   /* Ustala aktualny kolor wypełnień za pomocą typu ssh_color */
void set_pen_rgb(ssh_intensity r, ssh_intensity g, ssh_intensity b, /* Ustala aktualny kolor linii za pomocą składowych RGB */
                    ssh_natural width, ssh_mode style);         /*  oraz styl i grubość */
void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,
                  ssh_intensity a,ssh_natural size,ssh_mode style);
void set_brush_rgb(ssh_intensity r,ssh_intensity g,ssh_intensity b); /* Ustala aktualny kolor wypełnień za pomocą składowych RGB */
void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,
                   ssh_intensity a);

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO*/
ssh_mode  buffered();                                           /* Zwraca 1, jeśli buforowane */
ssh_mode  fixed();                                              /* Czy okno ma zafiksowana wielkość? */

ssh_rgb   get_rgb_from(ssh_color c);                            /* Jakie są ustawienia RGB konkretnego kolorku w palecie */
ssh_color background();                                         /* Aktualny kolor tla... */
ssh_color get_pen();                                            /* Aktualny kolor linii jako ssh_color */
ssh_color get_brush();                                          /* Aktualny kolor wypełnień jako ssh_color */

ssh_natural  screen_height();                                   /* Aktualne rozmiary okna po przeliczeniach z init_plot*/
ssh_natural  screen_width();                                    /*  ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora */
ssh_natural  get_line_width();                                  /* Aktualna grubość linii */

/* Wielkości znaków i tekstów */
ssh_natural  char_height(char znak);                            /* Aktualne rozmiary znaku  */
ssh_natural  char_width(char znak);                             /* ...potrzebne do pozycjonowania tekstu */
ssh_natural  string_height(const char* str);                    /* Aktualne rozmiary łańcucha tekstowego na ekranie */
ssh_natural  string_width(const char* str);                     /* ...potrzebne do jego pozycjonowania */

/* WYPROWADZANIE TEKSTU */
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);/* Tekst w oknie w kolorze brush()*/
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);/* Tekst w oknie czarno na białym*/
void printc(ssh_coordinate x,ssh_coordinate y,                         /* Tekst w oknie w zadanych kolorach */
            ssh_color fore,                                            /* - kolor tekstu */
            ssh_color back,                                            /* - kolor tła, jeśli drukuje nietransparentnie */
            const char* format,...);
void print_rgb(ssh_coordinate x,ssh_coordinate y,                      /* Drukuje z możliwością ustawienia tuszu poprzez RGB */
           ssh_intensity r,ssh_intensity g,ssh_intensity b,            /*- składowe koloru tekstu */
           ssh_color back,                                             /*- kolor tła, jeśli drukuje nietransparentnie */
           const char* format,...);

/* PUNKTOWANIE  */
void plot_rgb(ssh_coordinate x,ssh_coordinate y,                       /* Współrzędne */
              ssh_intensity r,ssh_intensity g,ssh_intensity b);        /*- składowe koloru */
void plot_d(ssh_coordinate x,ssh_coordinate y);                        /* Wyświetlenie punktu na ekranie w kolorze domyślnym */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);             /* Wyświetlenie punktu w kolorze z palety */

void fill_flood(ssh_coordinate x,ssh_coordinate y,                     /* Wypełnia powodziowo lub algorytmem siania */
                ssh_color fill,                                        /*- kolor wypełnienia */
                ssh_color border);                                     /*- kolor brzegu */

void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,                 /* Wypełnia powodziowo lub algorytmem siania */
                ssh_intensity rf,ssh_intensity gf,ssh_intensity bf,    /*- kolor wypełnienia podany składowymi */
                ssh_intensity rb,ssh_intensity gb,ssh_intensity bb);   /*- kolor brzegu podany składowymi */

/* RYSOWANIE LINII */
void line_rgb(ssh_coordinate x1,ssh_coordinate y1,                     /* Wyświetlenie linii w kolorze rgb */
              ssh_coordinate x2,ssh_coordinate y2,                     /* od punktu x1y1 do x2y2 */
              ssh_intensity r,ssh_intensity g,ssh_intensity b );       /*- składowe koloru */
void line_d(ssh_coordinate x1,ssh_coordinate y1,                       /* Wyświetlenie linii w kolorze domyślnym */
            ssh_coordinate x2,ssh_coordinate y2);                      /* od punktu x1y1 do x2y2 */
void line(ssh_coordinate x1,ssh_coordinate y1,                         /* Wyświetlenie linii w zadanym kolorze */
          ssh_coordinate x2,ssh_coordinate y2,                         /* od punktu x1y1 do x2y2 */
          ssh_color c);                                                /* - kolor dla linii */

/* RYSOWANIE KÓŁ, ELIPS i ŁUKÓW */
void circle_rgb(ssh_coordinate x,ssh_coordinate y,ssh_natural r,       /* Wyświetlenie okręgu w kolorze określonym */
                ssh_intensity rd,ssh_intensity gr,ssh_intensity bl);   /* składowymi koloru */
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c);/* Wyświetlenie okręgu w kolorze c */
void circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r);        /* Wyświetlenie okręgu w kolorze domyślnym */

void ellipse_rgb(ssh_coordinate x,ssh_coordinate y,                    /* Wyświetlenie elipsy */
                 ssh_natural a,ssh_natural b,                          /* o PÓŁOSIACH długości 'a' i 'b', */
                 ssh_intensity rd,ssh_intensity gr,ssh_intensity bl);  /* i kolorze wg. składowych koloru */
void ellipse_d(ssh_coordinate x,ssh_coordinate y,                      /* Wyświetlenie elipsy w kolorze domyślnym */
               ssh_natural a,ssh_natural b);                           /* o PÓŁOSIACH długości 'a' i 'b' */
void ellipse(ssh_coordinate x,ssh_coordinate y,                        /* Wyświetlenie elipsy w kolorze c */
             ssh_natural a,ssh_natural b, ssh_color c);                /* o PÓŁOSIACH długości 'a' i 'b' */

void arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,            /* rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop);                          /* w kolorze domyślnym */
void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,              /* rysuje łuk kołowy o promieniu r*/
           ssh_radian start,ssh_radian stop,ssh_color c);              /* w kolorze c */

void earc_d(ssh_coordinate x,ssh_coordinate y,                         /*rysuje łuk eliptyczny */
            ssh_natural a,ssh_natural b,                               /* o półosiach "a" i "b" */
            ssh_radian start,ssh_radian stop);
void earc(ssh_coordinate x,ssh_coordinate y,                           /*rysuje łuk eliptyczny */
          ssh_natural a,ssh_natural b,                                 /* o półosiach "a" i "b" */
          ssh_radian start,ssh_radian stop,ssh_color c);               /* w kolorze "c" */

void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r);   /* Wypełnienie kola o promieniu "r" w kolorach domyślnych*/
void fill_circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,
                 ssh_color c);                                         /* Wypełnienie kola w kolorze c */

void fill_ellipse_d(ssh_coordinate x,ssh_coordinate y,                 /* Wypełnienie elipsy w kolorach domyślnych*/
                    ssh_natural a,ssh_natural b);                      /* o PÓŁOSIACH długości "a" i "b" */
void fill_ellipse(ssh_coordinate x,ssh_coordinate y,                   /* Wypełnienie elipsy w kolorze "c" */
                  ssh_natural a,ssh_natural b,                         /* o PÓŁOSIACH długości "a" i "b" */
                  ssh_color c);                                        /* i kolorze "c" */

void fill_arc_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r,       /* wypełnia łuk kołowy o promieniu r*/
                ssh_radian start,ssh_radian stop,ssh_bool pie);        /* początek i koniec łuku */
void fill_arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,         /* wirtualny środek i promień łuku */
              ssh_radian start,ssh_radian stop,                        /* początku i końcu */
              ssh_bool pie,ssh_color c);                               /* w kolorze c */

void fill_earc_d(ssh_coordinate x,ssh_coordinate y,                    /* wypełnia łuk eliptyczny */
                 ssh_natural a,ssh_natural b,                          /* o półosiach "a" i "b" */
                 ssh_radian start,ssh_radian stop,ssh_bool pie);       /* początek i koniec łuku */
void fill_earc(ssh_coordinate x,ssh_coordinate y,                      /* wirtualny środek łuku */
               ssh_natural a,ssh_natural b,                            /* o półosiach "a" i "b" */
               ssh_radian start,ssh_radian stop,                       /* początku i końcu */
               ssh_bool pie,ssh_color c);                              /* w kolorze "c" */

/* WIELOKĄTY */
void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,                /* Wypełnienie prostokąta */
                   ssh_coordinate x2,ssh_coordinate y2,                /* rozciągniętego między rogami x1y1 a x2y2 */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b);   /* w kolorze rbg określonym składowymi koloru */

void fill_rect_d(ssh_coordinate x1, ssh_coordinate y1,                 /* Wypełnienie prostokąta w kolorach domyślnych*/
                 ssh_coordinate x2, ssh_coordinate y2);                /* rozciągniętego między rogami x1y1 a x2y2 */
void fill_rect(ssh_coordinate x1,ssh_coordinate y1,                    /* Wypełnienie prostokąta w kolorze c */
               ssh_coordinate x2,ssh_coordinate y2,ssh_color c);       /* rozciągniętego między rogami x1y1 a x2y2 */

void fill_poly_d(ssh_coordinate vx,ssh_coordinate vy,                  /* Wypełnia wielokąt przesunięty o "vx","vy" w kolorach domyślnych */
                const ssh_point points[],int number);                  /* - tablica wierzchołków wielokąta i jej długość */
void fill_poly(ssh_coordinate vx,ssh_coordinate vy,                    /* Wypełnia wielokąt przesunięty o "vx","vy" w kolorze c */
                const ssh_point points[],int number,                   /* - tablica wierzchołków wielokąta i jej długość */
                ssh_color c);                                          /* - kolor wypełnienia */

/* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) */
ssh_mode  input_ready(); /* Funkcja sprawdzająca, czy jest cos do wzięcia z wejścia */
ssh_msg   get_char();    /* Funkcja odczytywania znaków sterowania i zdarzeń */
                         /* '\r': Wymagane odrysowanie co najmniej fragmentu ekranu */
                         /* '\b': Jest zdarzenie myszy do przetworzenia */
                         /*  EOF: Zamknięto okno graficzne */

ssh_stat  set_char(ssh_msg ch);/* Odesłanie znaku na wejście. Zwraca 0, jeśli nie ma miejsca */
                               /* Pewne jest tylko odesłanie jednego znaku. */

ssh_stat  get_mouse_event(ssh_coordinate* x_pos,ssh_coordinate* y_pos,ssh_coordinate* click);/* Funkcja odczytująca ostatnie zdarzenie myszy */
ssh_stat  repaint_area(ssh_coordinate* x, ssh_coordinate* y,
                  ssh_natural* width, ssh_natural* height); /* Podaje obszar, który ma być odnowiony na żądanie '\r' i zwraca 0 */
                                                        /* Jeśli zwraca -1 to brak danych lub brak implementacji. Należy odrysować całość. */
                                                        /* Jeśli zwraca -2 to znaczy, że dane już były odczytane. Należy zignorować. */


#ifdef __cplusplus
} //extern C
#endif

#ifdef __cplusplus
// FUNKCJE INLINE DOSTĘPNE TYLKO Z POZIOMU C++
static_assert( sizeof(uchar8b)==1 , "uchar8b has more than 1 byte" );//???

//namespace SYMSHELL ???

inline
ssh_rgb RGB(ssh_intensity r,ssh_intensity g,ssh_intensity b)
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

// Konwerter na referencje, żeby nie trzeba było używać adresów
inline
ssh_stat  get_mouse_event(int& xpos,int& ypos,int& click)
{
    return get_mouse_event(&xpos,&ypos,&click);
}

// Konwerter na referencje, żeby nie trzeba było używać adresów
inline
ssh_stat  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

// FUNKCJE PRZESTARZAŁE TAKŻE DOSTĘPNE TYLKO DLA KOMPILATORA C++
inline ssh_mode   get_buffering(){ return 	buffered(); }       // Zwraca 1, jeśli buforowane
inline ssh_mode   get_fixed(){ return fixed(); }                // Czy okno ma zafiksowana wielkość
inline ssh_color  get_background(void){ return background(); }  // Aktualny kolor tla...
#endif

#endif /* _SYMSHELL_H_ */

/* ****************************************************************** */
/*              SYMSHELLLIGHT  version 2022-01-03                     */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
