/** @file
 * @brief SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++ (PL Doxygen).
 * @date 2026-04-26 (last modification)                                      */
/* ========================================================================= */
 /**
 * \details
 *      - Cały plik został znacząco zmieniony:     15.11.2020
 *      - Komentarze zostały znacząco rozbudowane: 01.03-04.2022
 *      - ... a potem zimą                         2025-2026
 *      - Menu kontekstowe z użyciem „rofi” dodano w 2026 roku
 *
 * \note
 *      - https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI
 *      - https://github.com/borkowsk
 *
 ** \author     Designed by W. Borkowski from the University of Warsaw
 **
 ** \library    SYMSHELLLIGHT  version 2026c
 */
#ifndef SYMSHELL_H_INCLUDED_
#define SYMSHELL_H_INCLUDED_ (1)

/**
* @defgroup	GrxInterface Podstawowe funkcje interfejsu graficznego
* @brief	przenośne między X11 i Windows funkcje rysujące i stowarzyszone.
* @details
*		Większość to moduły w języku C, a przynajmniej z takim interfejsem.
*		Działa też implementowana w C++ wersja zapisująca do plików SVG.
*/
/// @{

/* TYPY */
typedef unsigned char		uchar8b;		/**< \brief PODSTAWOWY TYP ZNAKÓW. MUSI MIEĆ 8 BITÓW (co najmniej). */

typedef	uchar8b				ssh_bool;		/**< \brief Zastępczy typ logiczny. Może być 0 albo 1. */
typedef	signed	int			ssh_msg;		/**< \brief Znak z klawiatury lub inne specjalne liczby, szczególnie z menu. */
typedef	signed	int			ssh_mode;		/**< \brief Zazwyczaj jest 0 lub 1, może 2, ale jak -1 to znaczy, że jakiś błąd. */
typedef	signed	int			ssh_stat;		/**< \brief Wartości zwracane jako status niektórych funkcji.*/
typedef	signed	int			ssh_coordinate;	/**< \brief Wszelkie współrzędne ekranowe. */
typedef unsigned	int		ssh_length;		/**< \brief Sytuacje, gdy 0 jest dopuszczalne, ale nie coś ujemnego np. długości tablic */
typedef	unsigned	int		ssh_natural;	/**< \brief Liczby większe od zera, gdy zero jest sytuacją nieoczekiwaną. */
typedef	unsigned	int		ssh_intensity;	/**< \brief Składowe kolorów itp. wartości od 0 wzwyż. */
typedef	unsigned	int		ssh_color;		/**< \brief Kolor indeksowany. TODO change name to ssh_color_index? */
typedef	float				ssh_radian;		/**< \brief Kąty w radianach dla łuków. */
/** \brief Punkt we współrzędnych ekranowych. */
typedef	struct	ssh_point	{ssh_coordinate x,y;}		ssh_point;
/** \brief Typ dla zestawu składowych RGB. TODO What about alpha? Union with uint32?  */
typedef	struct	ssh_rgb		{uchar8b r,g,b;}			ssh_rgb;
/** \brief TODO RGB with alpha. Union with uint32?  */
typedef	struct	ssh_rgba	{uchar8b r,g,b,a;}			ssh_rgba;

#ifdef __cplusplus
extern "C" {
const ssh_mode  PALETTE_LENGTH=512;               /**< \brief Długość palety predefiniowanych kolorów. */
const ssh_mode  PALETE_LENGHT=512;                /**< \brief Długość palety predefiniowanych kolorów (stara nazwa). */
const ssh_mode  SSH_SOLID_TEXT=0;                 /**< \brief Tekst na wypełnionym pasku tła. */
const ssh_mode  SSH_TRANSPARENT_TEXT=1;           /**< \brief Tekst na przezroczystym tle. */
const ssh_mode  SSH_SOLID_PUT=1;                  /**< \brief Zawartość nakładana na tło. */
const ssh_mode  SSH_XOR_PUT=2;                    /**< \brief Zawartość XOR-owane z tłem. */
const ssh_mode  SSH_LINE_SOLID=1;                 /**< \brief Linia ciągła. */
const ssh_mode  SSH_LINE_DOTTED=2;                /**< \brief Linia kropkowana. */
const ssh_mode  SSH_LINE_DASHED=3;                /**< \brief Linia przerywana. */
const ssh_mode  SSH_YES=1;                        /**< \brief Flaga potwierdzająca. */
const ssh_mode  SSH_NO=0;                         /**< \brief Flaga zaprzeczająca. */
#else
#define PALETTE_LENGTH      (512)                 /**< \brief Długość palety predefiniowanych kolorów. */
#define PALETE_LENGHT       (512)                 /**< \brief Długość palety predefiniowanych kolorów (stara nazwa). */
#define SSH_SOLID_TEXT       (0)                  /**< \brief Tekst na wypełnionym pasku tła. */
#define SSH_TRANSPARENT_TEXT (1)                  /**< \brief Tekst na przezroczystym tle. */
#define SSH_SOLID_PUT        (1)                  /**< \brief Zawartość nakładana na tło. */
#define SSH_XOR_PUT          (2)                  /**< \brief Zawartość XOR-owane z tłem. */
#define SSH_LINE_SOLID       (1)                  /**< \brief Linia ciągła. */
#define SSH_LINE_DOTTED      (2)                  /**< \brief Linia kropkowana. */
#define SSH_LINE_DASHED      (3)                  /**< \brief Linia przerywana. */
#define SSH_YES              (1)                  /**< \brief Flaga potwierdzająca. */
#define SSH_NO               (0)                  /**< \brief Flaga zaprzeczająca. */
#endif

/* ZMIENNE I STAŁE ZALEŻNE OD MODUŁU
 * ================================= */
/** \brief Nazwa aktualnie używanej implementacji funkcji graficznych.
 *  Aktualnie może być "X11", "WINDOWS" or "SVG". */
extern const char*  _ssh_grx_module_name;

/** \brief Jeśli nie jest to 0, to okno można używać.
 *         Niezerowa wartość oznacza co najmniej, że `init_plot` zadziałał z sukcesem.
 *         W zależności od modułu może tu być zapisany uchwyt okna, uchwyt pliku (pipe) albo po prostu 1. */
extern unsigned long _ssh_window;

/** \brief Określa czy zamykać od razu, czy dać szanse na przejrzenie zawartości.
* Do sterowania `close_plot` - czy wymaga ono potwierdzenia od użytkownika. */
extern int WB_error_enter_before_clean/* =0 */;

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* =============================================== */

/* Operacje konfiguracyjne o działaniu gwarantowanym przed inicjacją */
/* ----------------------------------------------------------------- */

/** \brief Przekazanie parametrów wywołania i nazwy okna. */
void shell_setup(const char* title,                      /**< Nazwa aplikacji używana jako tytuł okna lub jego część. */
                 const int   i_argc,                      /**< Liczba WSZYSTKICH parametrów wywołania. */
                 const char* i_argv[]                     /**< Przekazanie parametrów wywołania. */
                );

/** \brief Zmiana tytułu okna. (EN) Changes window new_window_name bar. */
void set_title(const char* new_window_name);

/** \brief Przełączanie buforowanie okna. Może nie działać po inicjacji.*/
void buffering_setup(ssh_mode yes);

/** \brief Określa, czy symulować niezmienność rozmiarów okna.
 *  W takim trybie zmiana wielkości okna powiększa piksele o całkowitą wielokrotność. */
void fix_size(ssh_mode yes);

/** \brief Zmienia definicje koloru w palecie kolorów. Indeksy 0..255. */
void set_rgb(ssh_color color,                                  /**< indeks koloru. */
             ssh_intensity r,                                  /**< Wartość składowej `red`. */
             ssh_intensity g,                                  /**< Wartość składowej `green`. */
             ssh_intensity b                                   /**< Wartość składowej `blue`. */
             );

/** \brief Zmiana definicje odcienia szarości w palecie szarości. Indeksy od 256 do 511. */
void set_gray(ssh_color shade,ssh_intensity intensity);

/** \brief Ustala index koloru do czyszczenia. Może nie działać po inicjacji.*/
void set_background(ssh_color c);

/* Inicjalizacja okna i całkowite zamykanie okna */
/* --------------------------------------------- */

/** \brief Właściwa dla platformy inicjacja grafiki (a kiedyś też semigrafiki!).
*   \return Zwraca 1, jeśli zadziałał poprawnie. */
ssh_stat  init_plot(ssh_natural  a,                            /**< Ile pikseli ma mieć okno w poziomie. */
                    ssh_natural  b,                            /**< Ile pikseli ma mieć okno w pionie. */
                    ssh_natural ca,                            /**< Ile dodatkowo kolumn tekstu po bokach. */
                    ssh_natural cb                             /**< Ile dodatkowo linii tekstu pod spodem przy domyślnej czcionce. */
                    );


/** \brief zamkniecie grafiki (lub semigrafiki 😁).
*  \details Automatycznie instalowana w `atexit`, stąd durne (void), żeby uniknąć warning-u. */
void close_plot(void);

/* OPERACJE DOTYCZĄCE CAŁEGO OKNA GRAFICZNEGO
 * ========================================== */

/** \brief Wymuszenie oczekiwania przez pewną liczbę "milli-seconds". */
void delay_ms(ssh_natural ms);

/** \brief  Wymuszenie oczekiwania przez pewną liczbę "micro-seconds". */
void delay_us(ssh_natural us);

/** \brief Ostateczne uzgodnienie zawartości ekranu z poprzednio zleconymi akcjami.
 *  \details Przy działaniu X11 przez sieć gwarantuje wysyłkę, może przelać ekran wirtualny na aktualny itp. */
void flush_plot();

/**  \brief  Bezwzględnie czyści ekran/okno lub ekran wirtualny. Może być czasochłonne. */
void clear_screen();

/** \brief  W sposób ukryty zapomina poprzednią zawartość ekranu.
 * \details Używany, gdy liczymy, że i tak zawartość zostałaby zamazana, a taka akcja jest tania (zwłaszcza gdy moduł wektorowy).
 * \return Zwraca 1, jeśli zadziałał poprawnie.
 * \warning DO NOTHING UNDER X11 and MS Windows */
ssh_stat  invalidate_screen();

/** \brief Zapisuje zawartość ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM, SVG itp.
 * \details Może nie działać w trybie bez buforowania okna/ekranu.
 * \return Zwraca 1, jeśli zadziałał poprawnie. */
ssh_stat  dump_screen(const char* file_name);

/* Operacje przestawiania własności pracy okna graficznego
 * ======================================================= */

/** \brief Ustala czy mysz ma byc obsługiwana. \return poprzedni stan flagi. */
ssh_mode    mouse_activity(ssh_mode yes);

/** \brief Włącza drukowanie tekstu bez zamazywania tła. \return Poprzednie ustawienie. */
ssh_mode    print_transparently(ssh_mode yes);

/** \brief Ustala szerokość linii. Grube linie są kosztowne! TODO WHAT ABOUT 0?
 * \return Poprzednie ustawienie. */
ssh_natural line_width(ssh_natural new_width);

/** \brief Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED.
 * \return Poprzednie ustawienie. */
ssh_mode    line_style(ssh_mode style);

/** \brief Ustala stosunek nowego rysowania do starej zawartości ekranu: SSH_SOLID_PUT, SSH_XOR_PUT.
 * \return Poprzednie ustawienie. */
ssh_mode    put_style(ssh_mode style);

/** \brief Ustala aktualny kolor linii i konturów za pomocą typu ssh_color oraz styl i grubość. */
void set_pen(ssh_color c, ssh_natural width, ssh_mode style);

/** \brief  Ustala aktualny kolor wypełnień za pomocą typu `ssh_color`. */
void set_brush(ssh_color c);

/** \brief Ustala aktualny kolor linii za pomocą składowych RGB oraz stylu i grubości. */
void set_pen_rgb(ssh_intensity r,                              /**< Składowa `red`. */
                 ssh_intensity g,                              /**< Składowa `green`. */
                 ssh_intensity b,                              /**< Składowa `blue`. */
                 ssh_natural width,                            /**< Grubość linii. */
                 ssh_mode style                                /**< Styl rysowania linii jak w `line_style`. */
                );

/** \brief Ustala aktualny kolor i przeźroczystość linii za pomocą składowych RGBA oraz styl i grubość
 *  \note Składowa `alfa` może być całkowicie ignorowana! */
void set_pen_rgba(ssh_intensity r,                             /**< Składowa `red`. */
                  ssh_intensity g,                             /**< Składowa `green`. */
                  ssh_intensity b,                             /**< Składowa `blue`. */
                  ssh_intensity a,                             /**< kanał alfa.*/
                  ssh_natural width,                           /**< Grubość linii. */
                  ssh_mode style                               /**< Styl rysowania linii jak w `line_style`. */
                  );

/** \brief Ustala aktualny kolor wypełnień za pomocą składowych RGB */
void set_brush_rgb(ssh_intensity r,                            /**< Składowa red. */
                   ssh_intensity g,                            /**< Składowa green. */
                   ssh_intensity b                             /**< Składowa blue. */
                   );

/** \brief Ustala aktualny kolor i przeźroczystość wypełnień za pomocą składowych RGB. */
void set_brush_rgba(ssh_intensity r,                            /**< Składowa red. */
                    ssh_intensity g,                            /**< Składowa green. */
                    ssh_intensity b,                            /**< Składowa blue. */
                    ssh_intensity a                             /**< kanał alfa. */
                    );

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEŃ OKNA GRAFICZNEGO
 * =================================================== */

/** \brief Sprawdza buforowanie. \return Zwraca 1, jeśli buforowane. */
ssh_mode  buffered();

/** \brief Sprawdza, czy okno ma zafiksowana wielkość? \return Zwraca SSH_YES albo SSH_NO.
 *  \details Aktualnie tylko w środowisku MS Windows dostępne są obie opcje.
 *           W SVG rozmiar jest zawsze zafiksowany, a w X11 jest elastyczny zewnętrznie, ale multiplikowany.
 *           Zatem grafika jest skalowana wielokrotnościami, ale napisy pozostają w tym samym rozmiarze. */
ssh_mode  fixed();

/** \brief Jakie są ustawienia RGB konkretnego kolorku w palecie. */
ssh_rgb   get_rgb_from(ssh_color c);

/** \brief Aktualny kolor tla... */
ssh_color background();

/** \brief Aktualny kolor linii jako `ssh_color` (indeks koloru w tabeli).
    \details
        Jeśli "pen" został ustawiony w trybie RBG(A), to zwraca wartość `(unsigned)(-1)`.
        W przypadku implementacji SVG zawsze zwraca -1024. */
ssh_color get_pen();

/** \brief Aktualny kolor wypełnień jako `ssh_color`.
    \details
        Jeśli "brush" został ustawiony w trybie RBG(A), to zwraca wartość `(unsigned)(-1)`.
        W przypadku implementacji SVG zawsze zwraca 0 (czarny) */
ssh_color get_brush();

/** \brief Aktualna grubość linii. */
ssh_natural  get_line_width();

/** \brief Aktualne rozmiary okna po przeliczeniach z init_plot
 *         ...i ewentualnych zmianach uczynionych "ręcznie" przez operatora. */
ssh_natural  screen_width();                                    /**< Całkowita użyteczna szerokość okna w pikselach. */
ssh_natural  screen_height();                                   /**< Całkowita użyteczna wysokość okna w pikselach. */

/* Wielkości znaków i tekstów 
   ==========================  */

/** \brief Aktualne rozmiary znaku potrzebne do pozycjonowania tekstu. */
ssh_natural  char_height(char sample);         /**< Wysokość znaku. Najlepiej dać 'X'. */
ssh_natural  char_width(char sample);          /**< Szerokość znaku. @note W 99% sytuacji mamy font stały (monotyp). */
ssh_natural  string_height(const char* str);   /**< Wysokość łańcucha tekstowego na ekranie. */
ssh_natural  string_width(const char* str);    /**< Szerokość łańcucha tekstowego na ekranie. */

/* DRUKOWANIE NA EKRANIE
 * ===================== */

/**  \brief  Rysuje napis od zadanych koordynatów graficznych. Poza tym działa jak zwykłe `printf`. */
/**  \details Domyślna wersja daje tekst w oknie czarno na białym. */
void printbw(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/**  \brief  Rysuje napis w oknie w kolorystyce domyślnej. */
void print_d(ssh_coordinate x,ssh_coordinate y,const char* format,...);

/** \brief Wyprowadzanie tekstu na ekran/okno od zadanych koordynatów i w zadanych kolorach. */
void printc(ssh_coordinate x,                                         /**< Współrzędna pozioma. */
            ssh_coordinate y,                                         /**< Współrzędna pionowa. */
            ssh_color fore,                                           /**< indeks koloru tekstu. */
            ssh_color back,                                           /**< indeks koloru tła, jeśli drukuje nietransparentnie. */
            const char* format,                                       /**< Format tekstu jak w \see `printf`. */
            ...                                                       /**< Wolne parametry zależne od formatu. */
            );

/** \brief Wyprowadzanie tekstu na ekran od zadanych koordynatów z możliwością ustawienia "tuszu" poprzez RGB. */
void print_rgb(ssh_coordinate x,                                      /**< Współrzędna pozioma. */
               ssh_coordinate y,                                      /**< Współrzędna pionowa. */
               ssh_intensity  r,                                      /**< Składowa red. */
               ssh_intensity  g,                                      /**< Składowa green. */
               ssh_intensity  b,                                      /**< Składowa blue. */
               ssh_color   back,                                      /**< indeks koloru tła, jeśli drukuje nietransparentnie. */
               const char* format,                                    /**< Format tekstu jak w \see `printf`. */
               ...                                                    /**< Wolne parametry zależne od formatu. */
               );

/* Zapalanie punktów na ekranie  
   ============================  */

/** Wyświetlenie punktu na ekranie w kolorze domyślnym.
 *  \param x,y Współrzędne punktu. */
void plot_d(ssh_coordinate x,ssh_coordinate y);

/** Wyświetlenie punktu w kolorze z palety.
 *  \param x,y Współrzędne punktu.
 *  \param c Indeks koloru punktu. */
void plot(ssh_coordinate x,ssh_coordinate y, ssh_color c);

/** \brief Wyświetlenie punktu na ekranie w kolorze RGB. */
void plot_rgb(ssh_coordinate x,                                       /**< Współrzędna pozioma. */
              ssh_coordinate y,                                       /**< Współrzędna pionowa. */
              ssh_intensity  r,                                       /**< Składowa red. */
              ssh_intensity  g,                                       /**< Składowa green. */
              ssh_intensity  b                                        /**< Składowa blue. */
              );

/** \brief Wypełnianie powodziowe lub algorytmem siania. */
void fill_flood(ssh_coordinate x,                                     /**< Współrzędna pozioma punktu startu. */
                ssh_coordinate y,                                     /**< Współrzędna pionowa punktu startu. */
                ssh_color fill,                                       /**< Indeks koloru wypełnienia. */
                ssh_color border                                      /**< Indeks koloru brzegu. */
                );

/** \brief Wypełnianie powodziowe lub algorytmem siania */
void fill_flood_rgb(ssh_coordinate x,ssh_coordinate y,                /**< Współrzędne punktu startu. */
                ssh_intensity rf,                                     /**< Składowa R koloru wypełnienia. */
                ssh_intensity gf,                                     /**< Składowa G koloru wypełnienia. */
                ssh_intensity bf,                                     /**< Składowa B koloru wypełnienia. */
                ssh_intensity rb,                                     /**< Składowa R koloru brzegu. */
                ssh_intensity gb,                                     /**< Składowa G koloru brzegu. */
                ssh_intensity bb                                      /**< Składowa B koloru brzegu. */
                );

/* RYSOWANIE LINII 
   ===============  */

/** \brief Wyświetlenie linii w kolorze domyślnym od punktu x1y1 do x2y2. */
void line_d(ssh_coordinate x1,                                        /**< Współrzędna pozioma punktu startu. */
            ssh_coordinate y1,                                        /**< Współrzędna pionowa punktu startu. */
            ssh_coordinate x2,                                        /**< Współrzędna pozioma punktu końca. */
            ssh_coordinate y2                                         /**< Współrzędna pionowa punktu końca. */
            );

/** \brief Wyświetlenie linii w zadanym kolorze od punktu x1y1 do x2y2. */
/** \details od punktu x1y1 do x2y2. */
void line(ssh_coordinate x1,                                          /**< Współrzędna pozioma punktu startu. */
          ssh_coordinate y1,                                          /**< Współrzędna pionowa punktu startu. */
          ssh_coordinate x2,                                          /**< Współrzędna pozioma punktu końca. */
          ssh_coordinate y2,                                          /**< Współrzędna pionowa punktu końca. */
          ssh_color c                                                 /**< Indeks koloru dla linii. */
          );

/** \brief Wyświetlenie linii w kolorze RGB. */
/** \details od punktu x1y1 do x2y2. */
void line_rgb(ssh_coordinate x1,                                      /**< Współrzędna pozioma punktu startu. */
              ssh_coordinate y1,                                      /**< Współrzędna pionowa punktu startu. */
              ssh_coordinate x2,                                      /**< Współrzędna pozioma punktu końca. */
              ssh_coordinate y2,                                      /**< Współrzędna pionowa punktu końca. */
              ssh_intensity r,ssh_intensity g,ssh_intensity b         /**< Składowe koloru. */
              );

/* RYSOWANIE KÓŁ, ELIPS i ŁUKÓW 
   ============================ */

/** \brief Wyświetlenie okręgu o promieniu 'r' w kolorze domyślnym. */
void circle_d(ssh_coordinate x,                                       /**< Współrzędna pozioma środka. */
              ssh_coordinate y,                                       /**< Współrzędna pionowa środka. */
              ssh_natural    r                                        /**< Promień okręgu. */
              );

/** \brief Wyświetlenie okręgu w kolorze indeksowanym `c`. */
void circle(ssh_coordinate x,                                         /**< Współrzędna pozioma środka. */
            ssh_coordinate y,                                         /**< Współrzędna pionowa środka. */
            ssh_natural    r,                                         /**< Promień okręgu. */
            ssh_color      c                                          /**< Indeks koloru. */
            );

/** \brief Wyświetlenie okręgu w kolorze określonym składowymi koloru. */
void circle_rgb(ssh_coordinate x,                                     /**< Współrzędna pozioma środka. */
                ssh_coordinate y,                                     /**< Współrzędna pionowa środka. */
                ssh_natural    r,                                     /**< Promień okręgu. */
                ssh_intensity rd,ssh_intensity gr,ssh_intensity bl    /**< Składowe koloru. */
                );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b'  w kolorze domyślnym. */
void ellipse_d(ssh_coordinate x,                                      /**< Współrzędna pozioma środka. */
               ssh_coordinate y,                                      /**< Współrzędna pionowa środka. */
               ssh_natural    a,                                      /**< Długość PÓŁOSI 'a' (poziomej). */
               ssh_natural    b                                       /**< Długość PÓŁOSI 'b' (pionowej). */
               );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze 'c'. */
void ellipse(ssh_coordinate x,                                        /**< Współrzędna pozioma środka. */
             ssh_coordinate y,                                        /**< Współrzędna pionowa środka. */
             ssh_natural    a,                                        /**< Długość PÓŁOSI 'a' (poziomej). */
             ssh_natural    b,                                        /**< Długość PÓŁOSI 'b' (pionowej). */
             ssh_color c                                              /**< Indeks koloru. */
             );

/** \brief Wyświetlenie elipsy o PÓŁOSIACH długości 'a' i 'b' w kolorze określonym składowymi koloru RGB. */
void ellipse_rgb(ssh_coordinate x,                                    /**< Współrzędna pozioma środka. */
                 ssh_coordinate y,                                    /**< Współrzędna pionowa środka. */
                 ssh_natural    a,                                    /**< Długość PÓŁOSI 'a' (poziomej). */
                 ssh_natural    b,                                    /**< Długość PÓŁOSI 'b' (pionowej). */
                 ssh_intensity rd,ssh_intensity gr,ssh_intensity bl   /**< Składowe koloru. */
                 );

/** \brief Rysowanie łuku kołowego o promieniu 'r' w kolorze domyślnym. */
void arc_d(ssh_coordinate x,                                          /**< Współrzędna pozioma środka. */
           ssh_coordinate y,                                          /**< Współrzędna pionowa środka. */
           ssh_natural    r,                                          /**< Promień okręgu. */
           ssh_radian start,                                          /**< Kąt startowy w radianach. */
           ssh_radian  stop                                           /**< Kąt końcowy w radianach. */
           );

/** \brief Rysowanie łuku kołowego o promieniu 'r' w kolorze indeksowanym 'c'. */
void arc(ssh_coordinate x,                                         /**< Współrzędna pozioma środka. */
         ssh_coordinate y,                                         /**< Współrzędna pionowa środka. */
         ssh_natural    r,                                         /**< Promień okręgu. */
         ssh_radian start,                                         /**< Kąt startowy w radianach. */
         ssh_radian  stop,                                         /**< Kąt końcowy w radianach. */
         ssh_color c                                               /**< Indeks koloru. */
         );

/** \brief Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze domyślnym.  */
void earc_d(ssh_coordinate x,                                      /**< Współrzędna pozioma środka. */
            ssh_coordinate y,                                      /**< Współrzędna pionowa środka. */
            ssh_natural a,                                         /**< Długość PÓŁOSI 'a' (poziomej). */
            ssh_natural b,                                         /**< Długość PÓŁOSI 'b' (pionowej). */
            ssh_radian start,                                      /**< Kąt startowy w radianach. */
            ssh_radian stop                                        /**< Kąt końcowy w radianach. */
            );

/** \brief Rysowanie łuku eliptycznego o PÓŁOSIACH długości 'a' i 'b' w kolorze indeksowanym 'c'. */
void earc(ssh_coordinate x,                                        /**< Współrzędna pozioma środka. */
          ssh_coordinate y,                                        /**< Współrzędna pionowa środka. */
          ssh_natural a,                                           /**< Długość PÓŁOSI 'a' (poziomej). */
          ssh_natural b,                                           /**< Długość PÓŁOSI 'b' (pionowej). */
          ssh_radian start,                                        /**< Kąt startowy w radianach. */
          ssh_radian stop,                                         /**< Kąt końcowy w radianach. */
          ssh_color c                                              /**< Indeks koloru. */
          );

/** \brief Wypełnienie koła o promieniu "r" w kolorach domyślnych. */
void fill_circle_d(ssh_coordinate x,                               /**< Współrzędna pozioma środka. */
                   ssh_coordinate y,                               /**< Współrzędna pionowa środka. */
                   ssh_natural    r                                /**< Promień okręgu. */
                   );

/** \brief Wypełnienie koła indeksowanym kolorem 'c'. */
void fill_circle(ssh_coordinate x,                             /**< Współrzędna pozioma środka. */
                 ssh_coordinate y,                             /**< Współrzędna pionowa środka. */
                 ssh_natural    r,                             /**< Promień okręgu. */
                 ssh_color      c                              /**< Indeks koloru. */
                 );

/** \brief Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem domyślnym. */
void fill_ellipse_d(ssh_coordinate x,                          /**< Współrzędna pozioma środka. */
                    ssh_coordinate y,                          /**< Współrzędna pionowa środka. */
                    ssh_natural a,                             /**< Długość PÓŁOSI 'a' (poziomej). */
                    ssh_natural b                              /**< Długość PÓŁOSI 'b' (pionowej). */
                    );

/** \brief Wypełnienie elipsy o PÓŁOSIACH długości "a" i "b" kolorem indeksowanym "c". */
void fill_ellipse(ssh_coordinate x,                            /**< Współrzędna pozioma środka. */
                  ssh_coordinate y,                            /**< Współrzędna pionowa środka. */
                  ssh_natural a,                               /**< Długość PÓŁOSI 'a' (poziomej). */
                  ssh_natural b,                               /**< Długość PÓŁOSI 'b' (pionowej). */
                  ssh_color c                                  /**< Indeks koloru. */
                  );

/** \brief Wypełnienie łuku kołowego o promieniu `r` kolorem domyślnym. */
void fill_arc_d(ssh_coordinate x,                              /**< Współrzędna pozioma wirtualnego środka. */
                ssh_coordinate y,                              /**< Współrzędna pionowa wirtualnego środka. */
                ssh_natural r,                                 /**< Promień okręgu. */
                ssh_radian start,                              /**< Kąt startowy w radianach. */
                ssh_radian stop,                               /**< Kąt końcowy w radianach. */
                ssh_bool pie                                   /**< Określa, czy wypełniać jako wycinek tortu. */
                );

/** \brief Wypełnienie łuku kołowego o promieniu `r` kolorem indeksowanym 'c'. */
void fill_arc(ssh_coordinate x,                                /**< Współrzędna pozioma wirtualnego środka. */
              ssh_coordinate y,                                /**< Współrzędna pionowa wirtualnego środka. */
              ssh_natural r,                                   /**< Promień okręgu. */
              ssh_radian start,                                /**< Kąt startowy w radianach. */
              ssh_radian stop,                                 /**< Kąt końcowy w radianach. */
              ssh_bool pie,                                    /**< Określa, czy wypełniać jako wycinek tortu. */
              ssh_color c                                      /**< Indeks koloru. */
              );

/** \brief Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem domyślnym. */
void fill_earc_d(ssh_coordinate x,                             /**< Współrzędna pozioma wirtualnego środka. */
                 ssh_coordinate y,                             /**< Współrzędna pionowa wirtualnego środka. */
                 ssh_natural a,                                /**< Długość wirtualnej PÓŁOSI 'a' (poziomej). */
                 ssh_natural b,                                /**< Długość wirtualnej PÓŁOSI 'b' (pionowej). */
                 ssh_radian start,                             /**< Kąt startowy w radianach. */
                 ssh_radian stop,                              /**< Kąt końcowy w radianach. */
                 ssh_bool pie                                  /**< Określa, czy wypełniać jako wycinek tortu. */
                 );

/** \brief Wypełnienie łuku eliptycznego o półosiach 'a' i 'b' kolorem indeksowany 'c'. */
void fill_earc(ssh_coordinate x,                               /**< Współrzędna pozioma wirtualnego środka. */
               ssh_coordinate y,                               /**< Współrzędna pionowa wirtualnego środka. */
               ssh_natural a,                                  /**< Długość wirtualnej PÓŁOSI 'a' (poziomej). */
               ssh_natural b,                                  /**< Długość wirtualnej PÓŁOSI 'b' (pionowej). */
               ssh_radian start,                               /**< Kąt startowy w radianach. */
               ssh_radian stop,                                /**< Kąt końcowy w radianach. */
               ssh_bool pie,                                   /**< Określa, czy wypełniać jako wycinek tortu. */
               ssh_color c                                     /**< Indeks koloru. */
               );


/* WIELOKĄTY 
   ========= */

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem domyślnym. */
void fill_rect_d(ssh_coordinate x1,                                  /**< Współrzędna pozioma narożnika startu. */
                 ssh_coordinate y1,                                  /**< Współrzędna pionowa narożnika startu. */
                 ssh_coordinate x2,                                  /**< Współrzędna pozioma narożnika końca. */
                 ssh_coordinate y2                                   /**< Współrzędna pionowa narożnika końca. */
                 );

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem indeksowanym 'c'. */
void fill_rect(ssh_coordinate x1,                                    /**< Współrzędna pozioma narożnika startu. */
               ssh_coordinate y1,                                    /**< Współrzędna pionowa narożnika startu. */
               ssh_coordinate x2,                                    /**< Współrzędna pozioma narożnika końca. */
               ssh_coordinate y2,                                    /**< Współrzędna pionowa narożnika końca. */
               ssh_color c                                           /**< Indeks koloru */
               );

/** \brief Wypełnienie prostokąta rozciągniętego między rogami x1y1 a x2y2 kolorem RGB. */
void fill_rect_rgb(ssh_coordinate x1,                                /**< Współrzędna pozioma narożnika startu */
                   ssh_coordinate y1,                                /**< Współrzędna pionowa narożnika startu */
                   ssh_coordinate x2,                                /**< Współrzędna pozioma narożnika końca */
                   ssh_coordinate y2,                                /**< Współrzędna pionowa narożnika końca */
                   ssh_intensity r,ssh_intensity g,ssh_intensity b   /**< Kolor wypełnienia określony składowymi. */
                   );

/** \brief Wypełnia wielokąt przesunięty o "vx","vy" kolorem domyślnym. */
void fill_poly_d(ssh_coordinate vx,                                   /**< Pozioma składowa wektora przesunięcia. */
                 ssh_coordinate vy,                                   /**< Pionowa składowa wektora przesunięcia. */
                 const ssh_point points[],                            /**< Tablica wierzchołków wielokąta. */
                 ssh_length n_of_points                               /**< Długość tablicy. */
                 );

/** \brief Wypełnia wielokąt przesunięty o "vx","vy" kolorem indeksowanym 'c'. */
void fill_poly(ssh_coordinate vx,                                    /**< Pozioma składowa wektora przesunięcia. */
               ssh_coordinate vy,                                    /**< Pionowa składowa wektora przesunięcia. */
               const ssh_point points[],                             /**< Tablica wierzchołków wielokąta. */
               ssh_length n_of_points,                               /**< Długość tablic. */
               ssh_color c                                           /**< Indeks koloru. */
               );



/* POBIERANIE ZNAKÓW Z KLAWIATURY i ZDARZEŃ OKIENNYCH (w tym z MENU) 
   ================================================================= */

/// @name POBIERANIE ZNAKÓW I ZDARZEŃ OKNA
/// @{

/** \brief Nieblokująca funkcja sprawdzająca, czy jest coś do wzięcia z wejścia.
 * @returns 1, gdy tak jest. W takiej sytuacji należy wykonać `get_char`. */
ssh_mode  input_ready();

/** \brief Blokująca funkcja odczytywania znaków sterowania i zdarzeń.
  * \return Indeks znaku z klawiatury, znak specjalny lub kod pozycji menu.
  * Niektóre znaki mają specjalne znaczenie:
  *  * '\r': Wymagane odrysowanie co najmniej fragmentu ekranu. Można użyć `repaint_area` lub odrysować całość.
  *  * '\\b': Jest zdarzenie myszy do przetworzenia. Trzeba użyć `get_mouse_event` i odpowiednio zareagować.
  *  *  EOF: Zamknięto okno graficzne. Trzeba zakończyć program.
  *  *  NNN: Duża liczba reprezentującą komendę z menu (zazwyczaj powyżej 1024).
  *  * '\0': neutralny komunikat. Zazwyczaj oznacza zdarzenie, które biblioteka sama przetworzyła. Należy zignorować. */
ssh_msg   get_char();

ssh_stat  set_char(ssh_msg ch); /**< \brief Odesłanie znaku na wejście. \return Zwraca 0, jeśli nie ma miejsca.
                                * \details Gwarantowane jest tylko odesłanie jednego znaku! */

/** \brief Funkcja odczytująca ostatnie zdarzenie myszy. \return 0, jeśli dane nie są dostępne. */
ssh_stat  get_mouse_event(ssh_coordinate* x_pos,         /**< [out] Adres, na który wpisze poziome położenie kursora. */
                          ssh_coordinate* y_pos,         /**< [out] Adres, na który wpisze pionowe położenie kursora. */
                          ssh_coordinate* click          /**< [out] Adres, na który wpisze informacje o kliku lub 0.  */
                          );

/** \brief Funkcja podaje obszar, który ma być odnowiony na żądanie '/r'.
 * \return  Zwraca 0 jak poprawnie (TODO CHECK?)
 *          Jeśli zwraca -1 to brak danych lub brak implementacji. Należy odrysować całość.
 *          Jeśli zwraca -2 to znaczy, że dane już były odczytane. Prawdopodobnie należy zignorować. */
ssh_stat  repaint_area(ssh_coordinate* x,          /**< [out] Adres, na który wpisze poziomą współrzędną rogu obszaru. */
                       ssh_coordinate* y,          /**< [out] Adres, na który wpisze pionową współrzędną rogu obszaru. */
                       ssh_natural* width,         /**< [out] Adres, na który wpisze szerokość obszaru. */
                       ssh_natural* height         /**< [out] Adres, na który wpisze wysokość obszaru. */
                       );
/// @}

/* OBSŁUGA MENU KONTEKSTOWEGO */
/* ========================== */

/// @name TWORZENIE MENU KONTEKSTOWEGO
/// @{

/** \brief Struktura do definiowania prostego menu. */
typedef struct ssh_menu_item_definition {
    const char* item_text;  /**< Tekst linii menu. Może być też etykieta różniąca się tym, że wartość jest 0. */
    long long   item_value; /**< Wartość przekazywana poprzez funkcję `get_char`. Dla etykiet 0. */
} ssh_menu_item_definition;

/** \brief Struktura do przekazywania absolutnego położenia kliknięcia i innych danych do uruchomienia menu kontekstowego. */
typedef struct ssh_basic_win_place_context {
    unsigned long long ScrIdentifier; /**< Dane identyfikacji systemu wyświetlania. Np. Display handle w X11 */
    unsigned long long WinIdentifier; /**< Dane identyfikacji wywołującego okna. Np. Window handle w X11 */
    unsigned X; /**< Bezwzględne położenie `x` kursora w układzie wyświetlacza albo -1, gdy nie można obliczyć. */
    unsigned Y; /**< Bezwzględne położenie `y` kursora w układzie wyświetlacza albo -1, gdy nie można obliczyć. */
} ssh_basic_win_place_context;

/** \brief Funkcja uruchamiająca kontekstowe menu po kliknięciu prawym klawiszem myszy.
 * \details Wywoływana z biblioteki, z pętli zdarzeń. Użytkownik biblioteki może zaproponować swoją wersję, a wersja
 *          domyślna znajduje się w odpowiednim katalogu źródłowym biblioteki, np. "X11/wb_context_menu_expected_rofi.c"
 * @param x - współrzędna pozioma kursora myszy.
 * @param y - współrzędna pionowa kursora myszy.
 * @param other_data - wskaźnik do rekordu danych użytkownika zawierającego co najmniej uchwyt Display i uchwyt okna.
 * @return 0 gdy menu nic nie zwróciło albo oczekujemy, że wynik wróci później jako message.
 *        -1 gdy funkcja zaniechała obsługi i kliknięcie ma być przekazane normalnej obsłudze w aplikacji (przez `\b`).
 *         Każda wartość dodatnia jest traktowana jako komunikat do zwrócenia przez funkcję `get_char`.
 *         Inna wartość ujemna powoduje wyświetlenie informacji o błędzie, ze sprawdzeniem wartości zmiennej `errno`.
 * @details Funkcja może być blokująca lub nieblokująca (np. odpalać osobny wątek). Podstawową implementację dostarcza
 *          biblioteka SYMSHELL, ale zdefiniowanie własnej przez użytkownika biblioteki blokuje linkowanie wersji domyślnej.
 */
extern long long ssh_context_menu_expected(unsigned x, unsigned y, struct ssh_basic_win_place_context* other_data);

/** \brief Domyślna definicja menu kontekstowego. W X11 dostarczana z biblioteki, ale można ją podmienić na poziomie linkowania.*/
extern ssh_menu_item_definition  context_menu_default[];

/** \brief Liczba itemów w domyślnym menu kontekstowym. Musi towarzyszyć `context_menu_default`. */
extern unsigned context_menu_default_size;

/** \brief Zmienna określająca poziom debugging-u funkcji menu. */
extern int 			ssh_menu_trace/*=0*/;

/// @}

#ifdef __cplusplus
} //extern C
#endif

#ifdef __cplusplus
static_assert( sizeof(uchar8b)==1 , "Type `uchar8b` has more than 1 byte" ); //???

/// @name FUNKCJE INLINE DOSTĘPNE SĄ TYLKO Z POZIOMU C++ !!!\n
///@{

//TODO namespace SYMSHELL ???

/// \brief Budowanie wartości RGB ze składowych \return ssh_rgb
inline ssh_rgb RGB( ssh_intensity r,                                       /**< Składowa red. */
                    ssh_intensity g,                                       /**< Składowa green. */
                    ssh_intensity b                                        /**< Składowa blue. */
                    )
{
    ssh_rgb po;
    po.r=(uchar8b)(r & 0xff);
    po.g=(uchar8b)(g & 0xff);
    po.b=(uchar8b)(b & 0xff);
    return po;
}

/// \brief Temporary implementation set_background RGB. TODO REAL IMPLEMENTATION!
inline void set_background(ssh_intensity r,ssh_intensity g,ssh_intensity b)
{   // TODO!
    set_background(r);
}

/// \brief Konwerter na referencje, żeby nie trzeba było używać adresów.
inline ssh_stat  get_mouse_event(int& x_pos,int& y_pos,int& click)
{
    return get_mouse_event(&x_pos,&y_pos,&click);
}

/// \brief Konwerter na referencje, żeby nie trzeba było używać adresów.
inline ssh_stat  repaint_area(ssh_coordinate& x, ssh_coordinate& y,ssh_natural& width, ssh_natural& height)
{
    return repaint_area(&x,&y,&width,&height);
}

/// \warning FUNKCJE PRZESTARZAŁE TAKŻE DOSTĘPNE TYLKO DLA KOMPILATORA C++
inline ssh_mode   get_buffering() { return 	buffered(); }        /**< Zwraca 1, jeśli buforowane. */
inline ssh_mode   get_fixed() { return fixed(); }                /**< Czy okno ma zafiksowana wielkość. */
inline ssh_color  get_background(void) { return background(); }  /**< Aktualny kolor tła... */

///@}

#endif

/// @}

/* ****************************************************************** */
/*                     SYMSHELLLIGHT 2026                             */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*             W O J C I E C H   B O R K O W S K I                    */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI   */
/*     GITHUB: https://github.com/borkowsk                            */
/*                                                                    */
/*                                 (Don't change or remove this note) */
/* ****************************************************************** */
#endif /* SYMSHELL_H_INCLUDED_ */


