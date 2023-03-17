/** \file symshell.h
 *  \brief SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++
 * ********************************************************************* */
/**
 * \details
 *             Whole file changed massively: 15.11.2020
 *             Comments changed massively: 3-4.01.2022
 * \author borkowsk - W.Borkowski from University of Warsaw
 * \note
 *      - https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI
 *      - https://github.com/borkowsk
 * \n :atom_symbol:
 */

#ifndef _SYMSHELL_H_
#define _SYMSHELL_H_

/**
* @defgroup	GrxInterface Podstawowe funkcje interfejsu graficznego
* @brief	przenośne między X11 i Windows funkcje rysujące i stowarzyszone
* @details
*	        Większość to moduły w języku C a przynajmniej z takim interfejsem.
*	        Działa też implementowana w C++ wersja zapisująca do plików SVG.
*/
///@{

#ifdef __cplusplus
extern "C" {
const unsigned  PALETE_LENGHT=512;                /**< \brief Długość palety predefiniowanych kolorów */
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
#define SSH_YES              (1)
#define SSH_NO               (0)
#endif

/* TYPY */
typedef unsigned char                           uchar8b;       /**< \brief BASIC CHAR TYPE. MUST HAVE 8 bits!? */

typedef uchar8b                                 ssh_bool;      /**< \brief logic type. 0 or 1 */
typedef int                                     ssh_msg;       /**< \brief Character from keyboard or other special numbers, especially from menu */
typedef signed   int                            ssh_mode;      /**< \brief Only symbols defined above expected! */
typedef signed   int                            ssh_stat;      /**< \brief Values returned as status for some functions */
typedef signed   int                            ssh_coordinate;/**< \brief Wszelkie współrzędne ekranowe */
typedef unsigned int                            ssh_natural;   /**< \brief Liczby większe od zera, gdy zero jest sytuacją nieoczekiwaną */
typedef unsigned int                            ssh_intensity; /**< \brief Składowe kolorów itp. wartości od 0 wzwyż */
typedef struct ssh_point {ssh_coordinate x,y;}  ssh_point;     /**< \brief Punkt we współrzędnych ekranowych */
typedef struct ssh_rgb   {uchar8b r,g,b;}       ssh_rgb;       /**< \brief RGB type. What about alpha? Union with uint32? TODO */
//typedef struct ssh_rgba  {uchar8b r,g,b,a;}     ssh_rgba;    /**< \brief RGB with alpha. Union with uint32? TODO */
typedef unsigned int                            ssh_color;     /**< \brief Indexed color. TODO change name to ssh_color_index? */
typedef float                                   ssh_radian;    /**< \brief katy w radianach dla łuków */

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */

/** \brief Przekazanie parametrów wywołania i nazwy okna */
void shell_setup(const char* title,                            /**< Nazwa aplikacji używana jako tytuł okna lub jego część */
                 const int   iargc,                            /**< liczba WSZYSTKICH parametrów wywołania */
                 const char* iargv[]                           /**< Przekazanie parametrów wywołania */
                );

/** \brief Change window title bar */
void set_title(const char* title);

/** \brief Przełączanie buforowanie okna. Może nie działać po inicjacji*/
void buffering_setup(ssh_mode Yes);

/** \brief Czy symulować niezmienność rozmiarów okna */
void fix_size(ssh_mode Yes);

/** \brief Zmienia definicje koloru w palecie kolorów. Indeksy 0..255 */
void set_rgb(ssh_color color,                                  /**< indeks koloru */
             ssh_intensity r,                                  /**< wartość składowej red */
             ssh_intensity g,                                  /**< wartość składowej green */
             ssh_intensity b                                   /**< wartość składowej blue */
             );

/** \brief Zmiana definicje odcienia szarości w palecie szarości. Indeksy 256..511 */
void set_gray(ssh_color shade,ssh_intensity intensity);

/** \brief Ustala index koloru do czyszczenia. Może nie działać po inicjacji*/
void set_background(ssh_color c);

/** \brief inicjacja grafiki/semigrafiki
*   \return Zwraca 1, jeśli zadziałał poprawnie */
ssh_stat  init_plot(ssh_natural  a,                            /**< ile pikseli mam mieć okno w poziomie */
                    ssh_natural  b,                            /**< ile pikseli mam mieć okno w pionie */
                    ssh_natural ca,                            /**< ile dodatkowo kolumn tekstu po bokach */
                    ssh_natural cb                             /**< ile dodatkowo linii tekstu pod spodem przy domyślnej czcionce */
                    );


/** \brief zamkniecie grafiki/semigrafiki
*  \details Automatycznie instalowana w atexit(), stąd durne (void), żeby uniknąć warning-u */
void close_plot(void);

/** \brief If not 0 the window is usable */
extern unsigned long _ssh_window;

/** Czy zamykać od razu, czy dać szanse na przejrzenie zawartości.
* Do sterowania close_plot() - czy wymaga ono potwierdzenia od użytkownika */
extern int WB_error_enter_before_clean/* =0 */;

/* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO */
/* ****************************************** */

/** \brief Wymuszenie oczekiwania przez pewną liczbę milli seconds */
void delay_ms(ssh_natural ms);

/** \brief  Wymuszenie oczekiwania przez pewną liczbę micro seconds */
void delay_us(ssh_natural us);

/** \brief  Ostateczne uzgodnienie zawartości ekranu z zawartością pamięci */
void flush_plot();

/**  \brief  Czyści ekran lub ekran wirtualny */
void clear_screen();

/** \brief  W sposób ukryty zapomina poprzednią zawartość ekranu.
 * gdy liczymy, że i tak zostałaby zamazana.
 * \return Zwraca 1, jeśli zadziałał poprawnie */
ssh_stat  invalidate_screen();

/** \brief Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp.
* Może nie działać w trybie bez buforowania okna/ekranu. \return Zwraca 1, jeśli zadziałał poprawnie */
ssh_stat  dump_screen(const char* Filename);

/* Operacje przestawiania własności pracy okna graficznego */
/** \brief Ustala czy mysz ma byc obsługiwana. \return poprzedni stan flagi */
ssh_mode    mouse_activity(ssh_mode Yes);

/** \brief Włącza drukowanie tekstu bez zamazywania tła. Zwraca stan poprzedni */
ssh_mode    print_transparently(ssh_mode Yes);

/** \brief Ustala szerokość linii. Zwraca stan poprzedni  Grube linie są kosztowne! TODO WHAT ABOUT 0? */
ssh_natural line_width(ssh_natural width);

/** \brief Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
ssh_mode    line_style(ssh_mode Style);

/** \brief Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */
ssh_mode    put_style(ssh_mode Style);

/** \brief Ustala aktualny kolor linii i konturów za pomocą typu ssh_color oraz styl i grubość */
void set_pen(ssh_color c, ssh_natural width, ssh_mode style);

/** \brief  Ustala aktualny kolor wypełnień za pomocą typu ssh_color */
void set_brush(ssh_color c);

/** \brief Ustala aktualny kolor linii za pomocą składowych RGB oraz styl i grubość */
void set_pen_rgb(ssh_intensity r,                              /**< składowa red */
                 ssh_intensity g,                              /**< składowa green */
                 ssh_intensity b,                              /**< składowa blue */
                 ssh_natural width,                            /**< grubość linii */
                 ssh_mode style                                /**< styl rysowania linii jak w \see line_style */
                );

/** \brief Ustala aktualny kolor i przeźroczystość linii za pomocą składowych RGBA oraz styl i grubość */
void set_pen_rgba(ssh_intensity r,                             /**< składowa red */
                  ssh_intensity g,                             /**< składowa green */
                  ssh_intensity b,                             /**< składowa blue */
                  ssh_intensity a,                             /**< kanał alfa */
                  ssh_natural width,                           /**< grubość linii */
                  ssh_mode style                               /**< styl rysowania linii jak w \see line_style */
                  );

/** \brief Ustala aktualny kolor wypełnień za pomocą składowych RGB */
void set_brush_rgb(ssh_intensity r,                            /**< składowa red */
                   ssh_intensity g,                            /**< składowa green */
                   ssh_intensity b                             /**< składowa blue */
                   );

/** \brief Ustala aktualny kolor i przeźroczystość wypełnień za pomocą składowych RGB */
void set_brush_rgba(ssh_intensity r,                            /**< składowa red */
                    ssh_intensity g,                            /**< składowa green */
                    ssh_intensity b,                            /**< składowa blue */
                    ssh_intensity a                             /**< kanał alfa */
                    );

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO*/

/** \brief Sprawdza buforowanie. \return Zwraca 1, jeśli buforowane */
ssh_mode  buffered();

/** \brief Sprawdza czy okno ma zafiksowana wielkość? \return Zwraca 1, jeśli TAK */
ssh_mode  fixed();

/** \brief Jakie są ustawienia RGB konkretnego kolorku w palecie */
ssh_rgb   get_rgb_from(ssh_color c);

/** \brief Aktualny kolor tla... */
ssh_color background();

/** \brief Aktualny kolor linii jako ssh_color */
ssh_color get_pen();

/** \brief Aktualny kolor wypełnień jako ssh_color */
ssh_color get_brush();

/** \brief Aktualna grubość linii */
ssh_natural  get_line_width();

/** \brief Aktualne rozmiary okna po przeliczeniach z init_plot
 *         ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora */
ssh_natural  screen_width();                                    /**< Całkowita użyteczna szerokość okna w pikselach */
ssh_natural  screen_height();                                   /**< Całkowita użyteczna wysokość okna w pikselach */

/* Wielkości znaków i tekstów */

/** \brief Aktualne rozmiary znaku potrzebne do pozycjonowania tekstu */
ssh_natural  char_height(char znak);                            /**< Wysokość znaku */
ssh_natural  char_width(char znak);                             /**< Szerokość znaku */
ssh_natural  string_height(const char* str);                    /**< Wysokość łańcucha tekstowego na ekranie */
ssh_natural  string_width(const char* str);                     /**< Szerokość łańcucha tekstowego na ekranie */

/**  \brief  od zadanych koordynatów. Po za tym działa jak zwykłe \see printf */
/**  \details Domyślna wersja daje tekst w oknie czarno na białym */
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/**  \brief Wyprowadzanie tekstu w oknie w kolorystyce domyślnej */
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/** \brief Wyprowadzanie tekstu na ekran/okno od zadanych koordynatów w zadanych kolorach */
void printc(ssh_coordinate x,                                         /**< współrzędna pozioma */
            ssh_coordinate y,                                         /**< współrzędna pionowa */
            ssh_color fore,                                           /**< indeks koloru tekstu */
            ssh_color back,                                           /**< indeks koloru tła, jeśli drukuje nietransparentnie */
            const char* format,                                       /**< format tekstu jak w \see printf */
            ...                                                       /**< wolne parametry zależne od formatu */
            );

/** \brief Wyprowadzanie tekstu na ekran od zadanych koordynatów z możliwością ustawienia "tuszu" poprzez RGB */
void print_rgb(ssh_coordinate x,                                      /**< współrzędna pozioma */
               ssh_coordinate y,                                      /**< współrzędna pionowa */
               ssh_intensity  r,                                      /**< składowa red */
               ssh_intensity  g,                                      /**< składowa green */
               ssh_intensity  b,                                      /**< składowa blue */
               ssh_color   back,                                      /**< indeks koloru tła, jeśli drukuje nietransparentnie */
               const char* format,                                    /**< format tekstu jak w \see printf */
               ...                                                    /**< wolne parametry zależne od formatu */
               );

/* Zapalanie punktów na ekranie  */

void plot_d(ssh_coordinate x,ssh_coordinate y);                       /**< Wyświetlenie punktu na ekranie w kolorze domyślnym */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);            /**< Wyświetlenie punktu w kolorze z palety */

/** \brief Wyświetlenie punktu na ekranie w kolorze RGB */
void plot_rgb(ssh_coordinate x,                                       /**< współrzędna pozioma */
              ssh_coordinate y,                                       /**< współrzędna pionowa */
              ssh_intensity  r,                                       /**< składowa red */
              ssh_intensity  g,                                       /**< składowa green */
              ssh_intensity  b                                        /**< składowa blue */
              );

/** \brief Wypełnianie powodziowe lub algorytmem siania */
void fill_flood(ssh_coordinate x,                                     /**< współrzędna pozioma punktu startu */
                ssh_coordinate y,                                     /**< współrzędna pionowa punktu startu */
                ssh_color fill,                                       /**< indeks koloru wypełnienia */
                ssh_color border                                      /**< indeks koloru brzegu */
                );

/** \brief Wypełnianie powodziowe lub algorytmem siania */
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,                /**< współrzędne punktu startu */
                ssh_intensity rf,                                     /**< składowa R koloru wypełnienia */
                ssh_intensity gf,                                     /**< składowa G koloru wypełnienia */
                ssh_intensity bf,                                     /**< składowa B koloru wypełnienia */
                ssh_intensity rb,                                     /**< składowa R koloru brzegu */
                ssh_intensity gb,                                     /**< składowa G koloru brzegu */
                ssh_intensity bb                                      /**< składowa B koloru brzegu */
                );

/* RYSOWANIE LINII */

/** \brief Wyświetlenie linii w kolorze domyślnym od punktu x1y1 do x2y2 */
void line_d(ssh_coordinate x1,                                        /**< współrzędna pozioma punktu startu */
            ssh_coordinate y1,                                        /**< współrzędna pionowa punktu startu */
            ssh_coordinate x2,                                        /**< współrzędna pozioma punktu końca */
            ssh_coordinate y2                                         /**< współrzędna pionowa punktu końca */
            );

/** \brief Wyświetlenie linii w zadanym kolorze od punktu x1y1 do x2y2 */
/** \details od punktu x1y1 do x2y2 */
void line(ssh_coordinate x1,                                          /**< współrzędna pozioma punktu startu */
          ssh_coordinate y1,                                          /**< współrzędna pionowa punktu startu */
          ssh_coordinate x2,                                          /**< współrzędna pozioma punktu końca */
          ssh_coordinate y2,                                          /**< współrzędna pionowa punktu końca */
          ssh_color c                                                 /**< indeks koloru dla linii */
          );

/** \brief Wyświetlenie linii w kolorze rgb */
/** \details od punktu x1y1 do x2y2 */
void line_rgb(ssh_coordinate x1,                                      /**< współrzędna pozioma punktu startu */
              ssh_coordinate y1,                                      /**< współrzędna pionowa punktu startu */
              ssh_coordinate x2,                                      /**< współrzędna pozioma punktu końca */
              ssh_coordinate y2,                                      /**< współrzędna pionowa punktu końca */
              ssh_intensity r,ssh_intensity g,ssh_intensity b         /**< składowe koloru */
              );

/* RYSOWANIE KÓŁ, ELIPS i ŁUKÓW */

/** \brief Wyświetlenie okręgu o promieniu 'r' w kolorze domyślnym */
void circle_d(ssh_coordinate x,                                       /**< współrzędna pozioma środka */
              ssh_coordinate y,                                       /**< współrzędna pionowa środka */
              ssh_natural    r                                        /**< promień okręgu */
              );

/** \brief Wyświetlenie okręgu w kolorze indeksowanym c */
void circle(ssh_coordinate x,                                         /**< współrzędna pozioma środka */
            ssh_coordinate y,                                         /**< współrzędna pionowa środka */
            ssh_natural    r,                                         /**< promień okręgu */
            ssh_color      c                                          /**< indeks koloru */
            );

/** \brief Wyświetlenie okręgu w kolorze określonym składowymi koloru */
void circle_rgb(ssh_coordinate x,                                     /**< współrzędna pozioma środka */
                ssh_coordinate y,                                     /**< współrzędna pionowa środka */
                ssh_natural    r,                                     /**< promień okręgu */
                ssh_intensity rd,ssh_intensity gr,ssh_intensity bl    /**< składowe koloru */
                );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b'  w kolorze domyślnym */
void ellipse_d(ssh_coordinate x,                                      /**< współrzędna pozioma środka */
               ssh_coordinate y,                                      /**< współrzędna pionowa środka */
               ssh_natural    a,                                      /**< długość PÓŁOSI 'a' (poziomej) */
               ssh_natural    b                                       /**< długość PÓŁOSI 'b' (pionowej) */
               );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze 'c' */
void ellipse(ssh_coordinate x,                                        /**< współrzędna pozioma środka */
             ssh_coordinate y,                                        /**< współrzędna pionowa środka */
             ssh_natural    a,                                        /**< długość PÓŁOSI 'a' (poziomej) */
             ssh_natural    b,                                        /**< długość PÓŁOSI 'b' (pionowej) */
             ssh_color c                                              /**< indeks koloru */
             );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze określonym składowymi koloru RGB */
void ellipse_rgb(ssh_coordinate x,                                    /**< współrzędna pozioma środka */
                 ssh_coordinate y,                                    /**< współrzędna pionowa środka */
                 ssh_natural    a,                                    /**< długość PÓŁOSI 'a' (poziomej) */
                 ssh_natural    b,                                    /**< długość PÓŁOSI 'b' (pionowej) */
                 ssh_intensity rd,ssh_intensity gr,ssh_intensity bl   /**< składowe koloru */
                 );

/** \brief Rysowanie łuku kołowego o promieniu 'r' w kolorze domyślnym*/
void arc_d(ssh_coordinate x,                                          /**< współrzędna pozioma środka */
           ssh_coordinate y,                                          /**< współrzędna pionowa środka */
           ssh_natural    r,                                          /**< promień okręgu */
           ssh_radian start,                                          /**< kąt startowy w radianach */
           ssh_radian  stop                                           /**< kąt końcowy w radianach */
           );

/** \brief Rysowanie łuku kołowego o promieniu 'r' w kolorze indeksowanym 'c' */
void arc(ssh_coordinate x,                                         /**< współrzędna pozioma środka */
         ssh_coordinate y,                                         /**< współrzędna pionowa środka */
         ssh_natural    r,                                         /**< promień okręgu */
         ssh_radian start,                                         /**< kąt startowy w radianach */
         ssh_radian  stop,                                         /**< kąt końcowy w radianach */
         ssh_color c                                               /**< indeks koloru */
         );

/** \brief Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze domyślnym  */
void earc_d(ssh_coordinate x,                                      /**< współrzędna pozioma środka */
            ssh_coordinate y,                                      /**< współrzędna pionowa środka */
            ssh_natural a,                                         /**< długość PÓŁOSI 'a' (poziomej) */
            ssh_natural b,                                         /**< długość PÓŁOSI 'b' (pionowej) */
            ssh_radian start,                                      /**< kąt startowy w radianach */
            ssh_radian stop                                        /**< kąt końcowy w radianach */
            );

/** \brief Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze indeksowanym 'c' */
void earc(ssh_coordinate x,                                        /**< współrzędna pozioma środka */
          ssh_coordinate y,                                        /**< współrzędna pionowa środka */
          ssh_natural a,                                           /**< długość PÓŁOSI 'a' (poziomej) */
          ssh_natural b,                                           /**< długość PÓŁOSI 'b' (pionowej) */
          ssh_radian start,                                        /**< kąt startowy w radianach */
          ssh_radian stop,                                         /**< kąt końcowy w radianach */
          ssh_color c                                              /**< indeks koloru */
          );

/** \brief Wypełnienie kola o promieniu "r" w kolorach domyślnych */
void fill_circle_d(ssh_coordinate x,                               /**< współrzędna pozioma środka */
                   ssh_coordinate y,                               /**< współrzędna pionowa środka */
                   ssh_natural    r                                /**< promień okręgu */
                   );

/** \brief Wypełnienie kola w kolorem  indeksowanym 'c' */
void fill_circle(ssh_coordinate x,                             /**< współrzędna pozioma środka */
                 ssh_coordinate y,                             /**< współrzędna pionowa środka */
                 ssh_natural    r,                             /**< promień okręgu */
                 ssh_color      c                              /**< indeks koloru */
                 );

/** \brief Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem domyślnym */
void fill_ellipse_d(ssh_coordinate x,                          /**< współrzędna pozioma środka */
                    ssh_coordinate y,                          /**< współrzędna pionowa środka */
                    ssh_natural a,                             /**< długość PÓŁOSI 'a' (poziomej) */
                    ssh_natural b                             /**< długość PÓŁOSI 'b' (pionowej) */
                    );

/** \brief Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem indeksowanym "c" */
void fill_ellipse(ssh_coordinate x,                            /**< współrzędna pozioma środka */
                  ssh_coordinate y,                            /**< współrzędna pionowa środka */
                  ssh_natural a,                               /**< długość PÓŁOSI 'a' (poziomej) */
                  ssh_natural b,                               /**< długość PÓŁOSI 'b' (pionowej) */
                  ssh_color c                                  /**< indeks koloru */
                  );

/** \brief Wypełnienie łuku kołowego o promieniu r kolorem domyślnym*/
void fill_arc_d(ssh_coordinate x,                              /**< współrzędna pozioma wirtualnego środka */
                ssh_coordinate y,                              /**< współrzędna pionowa wirtualnego środka */
                ssh_natural r,                                 /**< promień okręgu */
                ssh_radian start,                              /**< kąt startowy w radianach */
                ssh_radian stop,                               /**< kąt końcowy w radianach */
                ssh_bool pie                                   /**< czy wypełniać jako wycinek tortu */
                );

/** \brief Wypełnienie łuku kołowego o promieniu r kolorem indeksowanym 'c' */
void fill_arc(ssh_coordinate x,                                /**< współrzędna pozioma wirtualnego środka */
              ssh_coordinate y,                                /**< współrzędna pionowa wirtualnego środka */
              ssh_natural r,                                   /**< promień okręgu */
              ssh_radian start,                                /**< kąt startowy w radianach */
              ssh_radian stop,                                 /**< kąt końcowy w radianach */
              ssh_bool pie,                                    /**< czy wypełniać jako wycinek tortu */
              ssh_color c                                      /**< indeks koloru */
              );

/** \brief Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem domyślnym*/
void fill_earc_d(ssh_coordinate x,                                   /**< współrzędna pozioma wirtualnego środka */
                 ssh_coordinate y,                                   /**< współrzędna pionowa wirtualnego środka */
                 ssh_natural a,                                      /**< długość wirtualnej PÓŁOSI 'a' (poziomej) */
                 ssh_natural b,                                      /**< długość wirtualnej PÓŁOSI 'b' (pionowej) */
                 ssh_radian start,                                   /**< kąt startowy w radianach */
                 ssh_radian stop,                                    /**< kąt końcowy w radianach */
                 ssh_bool pie                                        /**< czy wypełniać jako wycinek tortu */
                 );

/** \brief Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem indeksowany 'c' */
void fill_earc(ssh_coordinate x,                                     /**< współrzędna pozioma wirtualnego środka */
               ssh_coordinate y,                                     /**< współrzędna pionowa wirtualnego środka */
               ssh_natural a,                                        /**< długość wirtualnej PÓŁOSI 'a' (poziomej) */
               ssh_natural b,                                        /**< długość wirtualnej PÓŁOSI 'b' (pionowej) */
               ssh_radian start,                                     /**< kąt startowy w radianach */
               ssh_radian stop,                                      /**< kąt końcowy w radianach */
               ssh_bool pie,                                         /**< czy wypełniać jako wycinek tortu */
               ssh_color c                                           /**< indeks koloru */
               );


/* WIELOKĄTY */

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem domyślnym */
void fill_rect_d(ssh_coordinate x1,                                  /**< współrzędna pozioma narożnika startu */
                 ssh_coordinate y1,                                  /**< współrzędna pionowa narożnika startu */
                 ssh_coordinate x2,                                  /**< współrzędna pozioma narożnika końca */
                 ssh_coordinate y2                                   /**< współrzędna pionowa narożnika końca */
                 );

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem indeksowanym 'c' */
void fill_rect(ssh_coordinate x1,                                    /**< współrzędna pozioma narożnika startu */
               ssh_coordinate y1,                                    /**< współrzędna pionowa narożnika startu */
               ssh_coordinate x2,                                    /**< współrzędna pozioma narożnika końca */
               ssh_coordinate y2,                                    /**< współrzędna pionowa narożnika końca */
               ssh_color c                                           /**< indeks koloru */
               );

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem RGB */
void fill_rect_rgb(ssh_coordinate x1,                                /**< współrzędna pozioma narożnika startu */
                   ssh_coordinate y1,                                /**< współrzędna pionowa narożnika startu */
                   ssh_coordinate x2,                                /**< współrzędna pozioma narożnika końca */
                   ssh_coordinate y2,                                /**< współrzędna pionowa narożnika końca */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b   /**< kolor określony składowymi koloru */
                   );

/** \brief Wypełnia wielokąt przesunięty o "vx","vy" kolorem domyślnym */
void fill_poly_d(ssh_coordinate vx,                                   /**< pozioma składowa wektora przesunięcia */
                 ssh_coordinate vy,                                   /**< pionowa składowa wektora przesunięcia */
                 const ssh_point points[],                            /**< tablica wierzchołków wielokąta */
                 int   length                                         /**< ... i jej długość */
                 );

/** \brief Wypełnia wielokąt przesunięty o "vx","vy" kolorem indeksowanym 'c' */
void fill_poly(ssh_coordinate vx,                                    /**< pozioma składowa wektora przesunięcia */
               ssh_coordinate vy,                                    /**< pionowa składowa wektora przesunięcia */
               const ssh_point points[],                             /**< tablica wierzchołków wielokąta */
               int   length,                                         /**< ... i jej długość */
               ssh_color c                                           /**< indeks koloru */
               );



/* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) */

ssh_mode  input_ready(); /**< \brief Funkcja sprawdzająca, czy jest coś do wzięcia z wejścia */

ssh_msg   get_char();    /**< \brief Funkcja odczytywania znaków sterowania i zdarzeń
                          * \return  indeks znaku z klawiatury lub znak specjalny:
                          * '/r' : Wymagane odrysowanie co najmniej fragmentu ekranu
                          * '/b' : Jest zdarzenie myszy do przetworzenia
                          *  EOF : Zamknięto okno graficzne
                          *  /n   Albo liczbę reprezentującą komendę z menu (zazwyczaj dużą)
                          *  */

ssh_stat  set_char(ssh_msg ch); /**< \brief Odesłanie znaku na wejście. \return Zwraca 0, jeśli nie ma miejsca
                                * \details Gwarantowane jest tylko odesłanie jednego znaku! */

/** \brief Funkcja odczytująca ostatnie zdarzenie myszy. \return ??? */
ssh_stat  get_mouse_event(ssh_coordinate* x_pos,            /**< [out] adres na który wpisze poziome położenie kursora */
                          ssh_coordinate* y_pos,            /**< [out] adres na który wpisze pionowe położenie kursora */
                          ssh_coordinate* click             /**< [out] adres na który wpisze informacje o kliku lub 0  */
                          );

/** \brief Funkcja podaje obszar, który ma być odnowiony na żądanie '/r'.
 * \return  zwraca 0 jak poprawnie (TODO?)
 *          Jeśli zwraca -1 to brak danych lub brak implementacji. Należy odrysować całość.
 *          Jeśli zwraca -2 to znaczy, że dane już były odczytane. Należy zignorować. */
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

/// \warning FUNKCJE INLINE DOSTĘPNE SĄ TYLKO Z POZIOMU C++ !!!\n
//TODO namespace SYMSHELL ???

/// \brief Budowanie wartości RGB ze składowych \return ssh_rgb
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

/// \brief Temporary implementation set_background RGB TODO REAL IMPLEMENTATION!
inline void set_background(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{   // TODO!
    set_background(r);
}

/// \brief Konwerter na referencje, żeby nie trzeba było używać adresów
inline ssh_stat  get_mouse_event(int& xpos,int& ypos,int& click)
{
    return get_mouse_event(&xpos,&ypos,&click);
}

/// \brief Konwerter na referencje, żeby nie trzeba było używać adresów
inline ssh_stat  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

/// \warning FUNKCJE PRZESTARZAŁE TAKŻE DOSTĘPNE TYLKO DLA KOMPILATORA C++
inline ssh_mode   get_buffering(){ return 	buffered(); }       /**< Zwraca 1, jeśli buforowane */
inline ssh_mode   get_fixed(){ return fixed(); }                /**< Czy okno ma zafiksowana wielkość */
inline ssh_color  get_background(void){ return background(); }  /**< Aktualny kolor tla... */
#endif

///@}
/* ****************************************************************** */
/*              SYMSHELLLIGHT  version 2023-03-14                     */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
#endif /* _SYMSHELL_H_ */


