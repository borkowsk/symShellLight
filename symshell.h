/** \file symshell.h                                                    *
 *  \brief SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++         */
/* **********************************************************************/
/**                                                                     *
 * \author W.Borkowski from University of Warsaw                        *
 * \n https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI           *
 * \n https://github.com/borkowsk                                       *
 * \n :atom_symbol:
 */
/* File changed massively: 15.11.2020                                   */
/* Comments changed massively: 3-4.01.2022                              */

#ifndef _SYMSHELL_H_
#define _SYMSHELL_H_

#ifdef __cplusplus
extern "C" {
const unsigned  PALETE_LENGHT=512;                /**< Długość palety predefiniowanych kolorów */
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
#define PALETE_LENGHT      (512)                /**< Długość palety predefiniowanych kolorów */
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
typedef unsigned char                           uchar8b;       /**< MUST HAVE 8 bits!? */

typedef uchar8b                                 ssh_bool;      /**< 0 or 1 */
typedef int                                     ssh_msg;       /**< Character from keyboard or other special numbers, especially from menu */
typedef signed   int                            ssh_mode;      /**< Only symbols defined above expected! */
typedef signed   int                            ssh_stat;      /**< Values returned as status for some functions */
typedef signed   int                            ssh_coordinate;/**< Wszelkie współrzędne ekranowe */
typedef unsigned int                            ssh_natural;   /**< Liczby większe od zera, gdy zero jest sytuacją nieoczekiwaną */
typedef unsigned int                            ssh_intensity; /**< Składowe kolorów itp. wartości od 0 wzwyż */
typedef struct ssh_point {ssh_coordinate x,y;}  ssh_point;     /**< Punkt we współrzędnych ekranowych */
typedef struct ssh_rgb   {uchar8b r,g,b;}       ssh_rgb;       /**< alpha? Union with uint32? TODO */
//typedef struct ssh_rgba  {uchar8b r,g,b,a;}     ssh_rgba;    /**< With alpha. Union with uint32? TODO */
typedef unsigned int                            ssh_color;     /**< ssh_color_index? TODO */
typedef float                                   ssh_radian;    /**< katy w radianach dla łuków */

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */

/** Przekazanie parametrów wywołania i nazwy okna */
void shell_setup(const char* title,                            /**< Nazwa aplikacji używana jako tytuł okna lub jego część */
                 int iargc,                                    /**< liczba WSZYSTKICH parametrów wywołania */
                 const char* iargv[]                           /**< Przekazanie parametrów wywołania */
                );

void set_title(const char* title);                             /**< Change window title bar */

void buffering_setup(ssh_mode Yes);                            /**< Przełączanie buforowanie okna. Może nie działać po inicjacji*/
void fix_size(ssh_mode Yes);                                   /**< Czy symulować niezmienność rozmiarów okna */

/** Zmienia definicje koloru w palecie kolorów. Indeksy 0..255 */
void set_rgb(ssh_color color,                                  /**< indeks koloru */
             ssh_intensity r,                                  /**< wartość składowej red */
             ssh_intensity g,                                  /**< wartość składowej green */
             ssh_intensity b                                   /**< wartość składowej blue */
             );


void set_gray(ssh_color shade,ssh_intensity intensity);        /**< Zmiana definicje odcienia szarości w palecie szarości. Indeksy 256..511 */

void set_background(ssh_color c);                              /**< Ustala index koloru do czyszczenia. Może nie działać po inicjacji*/

/** inicjacja grafiki/semigrafiki */
/** \return Zwraca 1, jeśli zadziałał poprawnie */
ssh_stat  init_plot(ssh_natural  a,                            /**< ile pikseli mam mieć okno w poziomie */
                    ssh_natural  b,                            /**< ile pikseli mam mieć okno w pionie */
                    ssh_natural ca,                            /**< ile dodatkowo kolumn tekstu po bokach */
                    ssh_natural cb                             /**< ile dodatkowo linii tekstu pod spodem przy domyślnej czcionce */
                    );



void close_plot(void);                                         /**< zamkniecie grafiki/semigrafiki
                                                                * Automatycznie instalowana w atexit, stąd durne (void), żeby uniknął warning-u */

extern unsigned long _ssh_window;                              /**< If not 0 the window is usable */

extern int WB_error_enter_before_clean/* =0 */;                /**< Czy zamykać od razu, czy dać szanse na przejrzenie zawartości.
                                                                * Do sterowania close_plot() - czy wymaga ono potwierdzenia od użytkownika */

/* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO */
/* ****************************************** */

void delay_ms(ssh_natural ms);                                 /**< Wymuszenie oczekiwania przez pewną liczbę milli seconds */
void delay_us(ssh_natural us);                                 /**< Wymuszenie oczekiwania przez pewną liczbę micro seconds */
void flush_plot();                                             /**< Ostateczne uzgodnienie zawartości ekranu z zawartością pamięci */
void clear_screen();                                           /**< Czyści ekran lub ekran wirtualny */

ssh_stat  invalidate_screen();                                 /**< W sposób ukryty zapomina poprzednią zawartość ekranu, gdy liczymy,
                                                                * że i tak zostałaby zamazana. \return Zwraca 1, jeśli zadziałał poprawnie */

ssh_stat  dump_screen(const char* Filename);                   /**< Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp.
                                                                * Może nie działać w trybie bez buforowania okna/ekranu. \return Zwraca 1, jeśli zadziałał poprawnie */

/* Operacje przestawiania własności pracy okna graficznego */
ssh_mode    mouse_activity(ssh_mode Yes);                      /**< Ustala czy mysz ma byc obsługiwana. Zwraca poprzedni stan flagi */
ssh_mode    print_transparently(ssh_mode Yes);                 /**< Włącza drukowanie tekstu bez zamazywania tła. Zwraca stan poprzedni */
ssh_natural line_width(ssh_natural width);                     /**< Ustala szerokość linii. Zwraca stan poprzedni  Grube linie są kosztowne! TODO WHAT ABOUT 0?*/
ssh_mode    line_style(ssh_mode Style);                        /**< Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
ssh_mode    put_style(ssh_mode Style);                         /**< Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */

void set_pen(ssh_color c, ssh_natural width, ssh_mode style);  /**< Ustala aktualny kolor linii i konturów za pomocą typu ssh_color oraz styl i grubość */
void set_brush(ssh_color c);                                   /**< Ustala aktualny kolor wypełnień za pomocą typu ssh_color */

/** Ustala aktualny kolor linii za pomocą składowych RGB oraz styl i grubość */
void set_pen_rgb(ssh_intensity r,                              /**< składowa red */
                 ssh_intensity g,                              /**< składowa green */
                 ssh_intensity b,                              /**< składowa blue */
                 ssh_natural width,                            /**< grubość linii */
                 ssh_mode style                                /**< styl rysowania linii jak w \see line_style */
);

/** Ustala aktualny kolor i przeźroczystość linii za pomocą składowych RGBA oraz styl i grubość */
void set_pen_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a,ssh_natural size,ssh_mode style);

/** Ustala aktualny kolor wypełnień za pomocą składowych RGB */
void set_brush_rgb(ssh_intensity r,                            /**< składowa red */
                   ssh_intensity g,                            /**< składowa green */
                   ssh_intensity b                             /**< składowa blue */
                   );

/** Ustala aktualny kolor i przeźroczystość wypełnień za pomocą składowych RGB */
void set_brush_rgba(ssh_intensity r,ssh_intensity g,ssh_intensity b,ssh_intensity a);

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO*/
ssh_mode  buffered();                                           /**< Sprawdza buforowanie. \return Zwraca 1, jeśli buforowane */
ssh_mode  fixed();                                              /**< Sprawdza czy okno ma zafiksowana wielkość? \return Zwraca 1, jeśli TAK */

ssh_rgb   get_rgb_from(ssh_color c);                            /**< Jakie są ustawienia RGB konkretnego kolorku w palecie */
ssh_color background();                                         /**< Aktualny kolor tla... */
ssh_color get_pen();                                            /**< Aktualny kolor linii jako ssh_color */
ssh_color get_brush();                                          /**< Aktualny kolor wypełnień jako ssh_color */
ssh_natural  get_line_width();                                  /**< Aktualna grubość linii */

/** Aktualne rozmiary okna po przeliczeniach z init_plot
 *  ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora */
ssh_natural  screen_width();                                    /**< Całkowita użyteczna szerokość okna w pikselach */
ssh_natural  screen_height();                                   /**< Całkowita użyteczna wysokość okna w pikselach */

/* Wielkości znaków i tekstów */

/** Aktualne rozmiary znaku potrzebne do pozycjonowania tekstu */
ssh_natural  char_height(char znak);                            /**< Wysokość znaku */
ssh_natural  char_width(char znak);                             /**< Szerokość znaku */
ssh_natural  string_height(const char* str);                    /**< Wysokość łańcucha tekstowego na ekranie */
ssh_natural  string_width(const char* str);                     /**< Szerokość łańcucha tekstowego na ekranie */

/** Wyprowadzanie tekstu na ekran od zadanych koordynatów. Po za tym działa jak \see printf */
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);/**< Tekst w oknie czarno na białym*/
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);/**< Tekst w oknie w kolorystyce domyślnej */


/** Wyprowadzanie tekstu na ekran od zadanych koordynatów w zadanych kolorach */
void printc(ssh_coordinate x,                                          /**< współrzędna pozioma */
            ssh_coordinate y,                                          /**< współrzędna pionowa */
            ssh_color fore,                                            /**< indeks koloru tekstu */
            ssh_color back,                                            /**< indeks koloru tła, jeśli drukuje nietransparentnie */
            const char* format,                                        /**< format tekstu jak w \see printf */
            ...                                                        /**< wolne parametry zależne od formatu */
            );

/** Wyprowadzanie tekstu na ekran od zadanych koordynatów z możliwością ustawienia "tuszu" poprzez RGB */
void print_rgb(ssh_coordinate x,                                       /**< współrzędna pozioma */
               ssh_coordinate y,                                       /**< współrzędna pionowa */
               ssh_intensity  r,                                       /**< składowa red */
               ssh_intensity  g,                                       /**< składowa green */
               ssh_intensity  b,                                       /**< składowa blue */
               ssh_color   back,                                       /**< indeks koloru tła, jeśli drukuje nietransparentnie */
               const char* format,                                     /**< format tekstu jak w \see printf */
               ...                                                     /**< wolne parametry zależne od formatu */
               );

/* Zapalanie punktów na ekranie  */

void plot_d(ssh_coordinate x,ssh_coordinate y);                        /**< Wyświetlenie punktu na ekranie w kolorze domyślnym */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);             /**< Wyświetlenie punktu w kolorze z palety */

/** Wyświetlenie punktu na ekranie w kolorze RGB */
void plot_rgb(ssh_coordinate x,                                       /**< współrzędna pozioma */
              ssh_coordinate y,                                       /**< współrzędna pionowa */
              ssh_intensity  r,                                       /**< składowa red */
              ssh_intensity  g,                                       /**< składowa green */
              ssh_intensity  b                                        /**< składowa blue */
              );

/** Wypełnianie powodziowe lub algorytmem siania */
void fill_flood(ssh_coordinate x,                                      /**< współrzędna pozioma punktu startu */
                ssh_coordinate y,                                      /**< współrzędna pionowa punktu startu */
                ssh_color fill,                                        /**< indeks koloru wypełnienia */
                ssh_color border                                       /**< indeks koloru brzegu */
                );

/** Wypełnianie powodziowe lub algorytmem siania */
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,                 /**< współrzędne punktu startu */
                ssh_intensity rf,ssh_intensity gf,ssh_intensity bf,    /**< kolor wypełnienia podany składowymi */
                ssh_intensity rb,ssh_intensity gb,ssh_intensity bb     /**< kolor brzegu podany składowymi */
                );

/* RYSOWANIE LINII */

/** Wyświetlenie linii w kolorze domyślnym od punktu x1y1 do x2y2 */
void line_d(ssh_coordinate x1,                                        /**< współrzędna pozioma punktu startu */
            ssh_coordinate y1,                                        /**< współrzędna pionowa punktu startu */
            ssh_coordinate x2,                                        /**< współrzędna pozioma punktu końca */
            ssh_coordinate y2                                         /**< współrzędna pionowa punktu końca */
            );

/** Wyświetlenie linii w zadanym kolorze od punktu x1y1 do x2y2 */
void line(ssh_coordinate x1,ssh_coordinate y1,ssh_coordinate x2,ssh_coordinate y2,          /* od punktu x1y1 do x2y2 */
          ssh_color c                                                  /**< indeks koloru dla linii */
          );

/** Wyświetlenie linii w kolorze rgb */
void line_rgb(ssh_coordinate x1,ssh_coordinate y1,ssh_coordinate x2,ssh_coordinate y2,      /* od punktu x1y1 do x2y2 */
              ssh_intensity r,ssh_intensity g,ssh_intensity b          /**< składowe koloru */
              );

/* RYSOWANIE KÓŁ, ELIPS i ŁUKÓW */

/** Wyświetlenie okręgu o promieniu 'r' w kolorze domyślnym */
void circle_d(ssh_coordinate x,                                        /**< współrzędna pozioma środka */
              ssh_coordinate y,                                        /**< współrzędna pionowa środka */
              ssh_natural    r                                         /**< promień okręgu */
              );

/** Wyświetlenie okręgu w kolorze indeksowanym c */
void circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,                                      /* PARAMETRY OKRĘGU */
            ssh_color c                                                /**< indeks koloru */
            );

/** Wyświetlenie okręgu w kolorze określonym składowymi koloru */
void circle_rgb(ssh_coordinate x,ssh_coordinate y,ssh_natural r,                                  /* PARAMETRY OKRĘGU */
                ssh_intensity rd,ssh_intensity gr,ssh_intensity bl     /**< składowe koloru */
                );

/** Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b'  w kolorze domyślnym */
void ellipse_d(ssh_coordinate x,                                        /**< współrzędna pozioma środka */
               ssh_coordinate y,                                        /**< współrzędna pionowa środka */
               ssh_natural    a,                                        /**< długość PÓŁOSI 'a' (poziomej) */
               ssh_natural    b                                         /**< długość PÓŁOSI 'b' (pionowej) */
               );

/** Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze 'c' */
void ellipse(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b,/* PARAMETRY elipsy */
             ssh_color c                                                 /**< indeks koloru */
             );

/** Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze określonym składowymi koloru RGB */
void ellipse_rgb(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b,                   /* PARAMETRY elipsy */
                 ssh_intensity rd,ssh_intensity gr,ssh_intensity bl      /**< składowe koloru */
                 );

/** Rysowanie łuku kołowego o promieniu 'r' w kolorze domyślnym*/
void arc_d(ssh_coordinate x,                                        /**< współrzędna pozioma środka */
           ssh_coordinate y,                                        /**< współrzędna pionowa środka */
           ssh_natural    r,                                        /**< promień okręgu */
           ssh_radian start,                                        /**< kąt startowy w radianach */
           ssh_radian stop                                          /**< kąt końcowy w radianach */
           );

/** Rysowanie łuku kołowego o promieniu 'r' w kolorze indeksowanym 'c' */
void arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_radian start,ssh_radian stop,          /* PARAMETRY ŁUKU */
         ssh_color c                                                /**< indeks koloru */
         );

/** Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze domyślnym  */
void earc_d(ssh_coordinate x,                                      /**< współrzędna pozioma środka */
            ssh_coordinate y,                                      /**< współrzędna pionowa środka */
            ssh_natural a,                                         /**< długość PÓŁOSI 'a' (poziomej) */
            ssh_natural b,                                         /**< długość PÓŁOSI 'b' (pionowej) */
            ssh_radian start,                                      /**< kąt startowy w radianach */
            ssh_radian stop                                        /**< kąt końcowy w radianach */
            );

/** Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze indeksowanym 'c' */
void earc(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b,ssh_radian start,ssh_radian stop,/* PARAMETRY ŁUKU */
          ssh_color c                                                 /**< indeks koloru */
          );

void fill_circle_d(ssh_coordinate x,ssh_coordinate y,ssh_natural r);  /**< Wypełnienie kola o promieniu "r" w kolorach domyślnych*/
void fill_circle(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_color c);/**< Wypełnienie kola w kolorem  indeksowanym 'c' */

/** Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem domyślnym */
void fill_ellipse_d(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b);

/** Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem indeksowanym "c" */
void fill_ellipse(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b,ssh_color c);

/** Wypełnienie łuku kołowego o promieniu r kolorem domyślnym*/
void fill_arc_d(ssh_coordinate x,                                      /**< współrzędna pozioma wirtualnego środka */
                ssh_coordinate y,                                      /**< współrzędna pionowa wirtualnego środka */
                ssh_natural r,                                         /**< promień okręgu */
                ssh_radian start,                                      /**< kąt startowy w radianach */
                ssh_radian stop,                                       /**< kąt końcowy w radianach */
                ssh_bool pie                                           /**< czy wypełniać jako wycinek tortu */
                );

/** Wypełnienie łuku kołowego o promieniu r kolorem indeksowanym 'c' */
void fill_arc(ssh_coordinate x,ssh_coordinate y,ssh_natural r,ssh_radian start,ssh_radian stop,
              ssh_bool pie,                                            /**< czy wypełniać jako wycinek tortu */
              ssh_color c                                              /**< indeks koloru */
              );

/** Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem domyślnym*/
void fill_earc_d(ssh_coordinate x,                                      /**< współrzędna pozioma wirtualnego środka */
                 ssh_coordinate y,                                      /**< współrzędna pionowa wirtualnego środka */
                 ssh_natural a,                                         /**< długość wirtualnej PÓŁOSI 'a' (poziomej) */
                 ssh_natural b,                                         /**< długość wirtualnej PÓŁOSI 'b' (pionowej) */
                 ssh_radian start,                                      /**< kąt startowy w radianach */
                 ssh_radian stop,                                       /**< kąt końcowy w radianach */
                 ssh_bool pie                                           /**< czy wypełniać jako wycinek tortu */
                 );

/** Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem indeksowany 'c' */
void fill_earc(ssh_coordinate x,ssh_coordinate y,ssh_natural a,ssh_natural b,ssh_radian start,ssh_radian stop,
               ssh_bool pie,                                            /**< czy wypełniać jako wycinek tortu */
               ssh_color c                                              /**< indeks koloru */
               );


/* WIELOKĄTY */

/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem domyślnym */
void fill_rect_d(ssh_coordinate x1,                                     /**< współrzędna pozioma narożnika startu */
                 ssh_coordinate y1,                                     /**< współrzędna pionowa narożnika startu */
                 ssh_coordinate x2,                                     /**< współrzędna pozioma narożnika końca */
                 ssh_coordinate y2                                      /**< współrzędna pionowa narożnika końca */
                 );

/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem indeksowanym 'c' */
void fill_rect(ssh_coordinate x1,ssh_coordinate y1,ssh_coordinate x2,ssh_coordinate y2,       /* PARAMETRY PROSTOKĄTA */
               ssh_color c                                              /**< indeks koloru */
               );

/** Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem RGB */
void fill_rect_rgb(ssh_coordinate x1,ssh_coordinate y1,ssh_coordinate x2,ssh_coordinate y2,   /* PARAMETRY PROSTOKĄTA */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b       /**< kolor określony składowymi koloru */
                   );

/** Wypełnia wielokąt przesunięty o "vx","vy" kolorem domyślnym */
void fill_poly_d(ssh_coordinate vx,                                     /**< pozioma składowa wektora przesunięcia */
                 ssh_coordinate vy,                                     /**< pionowa składowa wektora przesunięcia */
                 const ssh_point points[],                              /**< tablica wierzchołków wielokąta */
                 int   length                                           /**< ... i jej długość */
                 );

/** Wypełnia wielokąt przesunięty o "vx","vy" kolorem indeksowanym 'c' */
void fill_poly(ssh_coordinate vx,ssh_coordinate vy,const ssh_point points[],int length,        /* PARAMETRY WIELOKĄTA */
               ssh_color c                                              /**< indeks koloru */
               );



/* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) */

ssh_mode  input_ready(); /**< Funkcja sprawdzająca, czy jest coś do wzięcia z wejścia */

ssh_msg   get_char();    /**< Funkcja odczytywania znaków sterowania i zdarzeń
                          * \return
                          * indeks znaku z klawiatury lub znak specjalny:
                          * '\\r': Wymagane odrysowanie co najmniej fragmentu ekranu
                          * '\\b': Jest zdarzenie myszy do przetworzenia
                          *  EOF: Zamknięto okno graficzne
                          *  \n Albo liczbę reprezentującą komendę z menu (zazwyczaj dużą)
                          *  */

ssh_stat  set_char(ssh_msg ch);/**< \brief Odesłanie znaku na wejście. \return Zwraca 0, jeśli nie ma miejsca
                                * \details Gwarantowane jest tylko odesłanie jednego znaku! */

/** Funkcja odczytująca ostatnie zdarzenie myszy. \return ??? */
ssh_stat  get_mouse_event(ssh_coordinate* x_pos,            /**< [out] adres na który wpisze poziome położenie kursora */
                          ssh_coordinate* y_pos,            /**< [out] adres na który wpisze pionowe położenie kursora */
                          ssh_coordinate* click             /**< [out] adres na który wpisze informacje o kliku lub 0  */
                          );

/** Funkcja podaje obszar, który ma być odnowiony na żądanie '\r'.
 * \return  zwraca 0 jak poprawnie (TODO?)
 * Jeśli zwraca -1 to brak danych lub brak implementacji. Należy odrysować całość.
 * Jeśli zwraca -2 to znaczy, że dane już były odczytane. Należy zignorować. */
ssh_stat  repaint_area(ssh_coordinate* x,                 /**< [out] adres na który wpisze poziomą współrzędną rogu obszaru */
                       ssh_coordinate* y,                 /**< [out] adres na który wpisze pionową współrzędną rogu obszaru */
                       ssh_natural* width,                /**< [out] adres na który wpisze szerokość obszaru */
                       ssh_natural* height                /**< [out] adres na który wpisze wysokość obszaru */
                       );

#ifdef __cplusplus
} //extern C
#endif

#ifdef __cplusplus
static_assert( sizeof(uchar8b)==1 , "uchar8b has more than 1 byte" );//???

/// FUNKCJE INLINE DOSTĘPNE SĄ TYLKO Z POZIOMU C++ !!!\n
//TODO namespace SYMSHELL ???

/// Budowanie wartości RGB ze składowych \return ssh_rgb
inline ssh_rgb RGB( ssh_intensity r,                                       /**< składowa red */
                    ssh_intensity g,                                       /**< składowa green */
                    ssh_intensity b                                        /**< składowa blue */
                    )
{
    ssh_rgb po;
    po.r=(uchar8b)(r & 0xff);
    po.g=(uchar8b)(g & 0xff);
    po.b=(uchar8b)(b & 0xff);
    return po;
}

/// Temporary implementation set_background RGB TODO REAL IMPLEMENTATION!
inline void set_background(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{   // TODO!
    set_background(r);
}

/// Konwerter na referencje, żeby nie trzeba było używać adresów
inline ssh_stat  get_mouse_event(int& xpos,int& ypos,int& click)
{
    return get_mouse_event(&xpos,&ypos,&click);
}

/// Konwerter na referencje, żeby nie trzeba było używać adresów
inline ssh_stat  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

/// FUNKCJE PRZESTARZAŁE TAKŻE DOSTĘPNE TYLKO DLA KOMPILATORA C++
inline ssh_mode   get_buffering(){ return 	buffered(); }       /**< Zwraca 1, jeśli buforowane */
inline ssh_mode   get_fixed(){ return fixed(); }                /**< Czy okno ma zafiksowana wielkość */
inline ssh_color  get_background(void){ return background(); }  /**< Aktualny kolor tla... */
#endif

#endif /* _SYMSHELL_H_ */

/* ****************************************************************** */
/*              SYMSHELLLIGHT  version 2022-01-04                     */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
