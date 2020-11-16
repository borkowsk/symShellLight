/* SYMSHELL.H - SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++    */
/************************************************************************/
/* designed by W.Borkowski http://www.iss.uw.edu.pl/~borkowsk           */
/* Last changed: 22.06.2007                                			    */

#ifndef _SYMSHELL_H_
#define _SYMSHELL_H_
#ifdef __cplusplus
extern "C" {
#endif

/* DEFINICJE */
#define SSH_SOLID_TEXT 0
#define SSH_TRANSPARENT_TEXT 1
#define SSH_SOLID_PUT	1
#define SSH_XOR_PUT		2
#define SSH_LINE_SOLID  1
#define SSH_LINE_DOTTED 2
#define SSH_LINE_DASHED 3

/* TYPY */
typedef struct ssh_point {int x,y;} ssh_point;
typedef unsigned ssh_color; 

/* OTWIERANIE i ZAMYKANIE TRYBU (OKNA) GRAFICZNEGO */
/* Operacje konfiguracyjne o dzia³aniu gwarantowanym przed inicjacja */
void shell_setup(const char* title,int iargc,const char* iargv[]);/* Przekazanie parametrow wywolania */
void buffering_setup(int Yes);			/* Przelaczanie buforowanie okna - moze nie dzialac po inicjacji*/
void fix_size(int Yes);					/* Czy symulowaæ niezmiennosc rozmiarow okna */
void set_background(ssh_color c);		/* Ustala index koloru do czyszczenia - moze nie dzialac po inicjacji*/
void set_rgb(ssh_color color,int r,int g,int b);/* Zmienia definicje koloru w palecie kolorow. Indeksy 0..255 */
void set_gray(ssh_color shade,int intensity);   /* Zmiania definicje odcienia szarosci w palecie szarosci. Indeksy 256..511 */

int  init_plot(int a,int b,int ca, int cb); /* inicjacja grafiki/semigrafiki */
void close_plot(void); 					/* zamkniecie grafiki/semigrafiki */
										/* Automatycznie instalowana w atexit - stad durne (void) */
										/* zeby uniknac warningu          */

/* OPERACJE DOTYCZACE CALEGO OKNA GRAFICZNEGO */
void	delay_ms(unsigned ms);					/* Wymuszenie oczekiwania przez pewn¹ liczbê ms */
void	flush_plot(void);						/* Ostateczne uzgodnienie zawartosci ekranu z zawartoscia pamieci */
void	clear_screen(void);						/* Czysci ekran lub ekran wirtualny */
int		dump_screen(const char* Filename);	/* Zapisuje zawartosc ekranu do pliku graficznego w naturalnym formacie platformy: BMP, XBM itp */

/* Operacje przestawiania wlasnosci pracy okna graficznego */
int		mouse_activity(int yes);		/* Ustala czy mysz ma byc obslugiwana. Zwraca poprzedni stan flagi */
int		print_transparently(int yes);	/* Wlacza drukowanie tekstu bez zamazywania t³a. Zwraca stan poprzedni */
int		line_width(int width);		/* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni */
int		line_style(int Style);          /* Ustala styl rysowania lini: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
int		put_style(int Style);           /* Ustala stosunek nowego rysowania do starej zawartosci ekranu: SSH_SOLID_PUT,SSH_XOR_PUT */
void	set_pen(ssh_color c,int size,int style);			/* Ustala aktualny kolor linii za pomoca typu ssh_color */
void	set_brush(ssh_color c);			/* Ustala aktualny kolor wypelnien za pomoca typu ssh_color */
void	set_pen_rgb(int r,int g,int b,int size,int style); /* Ustala aktualny kolor linii za pomoca skladowych RGB */
void	set_brush_rgb(int r,int g,int b);/* Ustala aktualny kolor wypelnien za pomoca skladowych RGB */

/* ODCZYTYWYWANIE AKTUALNYCH USTAWIEN OKNA GRAFICZNEGO*/
int  buffered(void);				        /* Zwraca 1 jesli buforowane */
int  fixed(void);					        /* Czy okno ma zafiksowana wielkosc */
ssh_color background(void);				    /* Aktualny kolor tla... */
int get_line_width(void);					/* Aktualna grubosc linii */
ssh_color get_pen(void);					/* Aktualny kolor linii jako ssh_color */
ssh_color get_brush(void);					/* Aktualny kolor wypelnien jako ssh_color */
int  screen_height(void);  					/* Aktualne rozmiary okna po przeliczeniach z init_plot*/
int  screen_width(void);   					/*  ...i ewentualnych zmianach uczynionych "recznie" przez operatora */
int  char_height(char znak);			/* Aktualne rozmiary znaku  */
int  char_width(char znak);				/* ...potrzebne do pozycjonowania tekstu */
int  string_height(const char* str);	/* Aktualne rozmiary lancucha */
int  string_width(const char* str);		/* ...potrzebne do jego pozycjonowania */

/* WYPROWADZANIE TEKSTU */
#ifndef print
#define print printbw
#else
#error "print" already defined
#endif

void printbw(int x,int y,const char* format,...);/* Drukuje w kolorach domyslnych*/
void printc(int x,int y,
			ssh_color fore,ssh_color back,
					   const char* format,...);/* Drukuje w kolorach uzytkownika*/

/* PUNKTOWANIE  */
void plot(int x,int y,
				ssh_color c);		/* Wyswietlenie punktu na ekranie */
void plot_rgb(int x,int y,
				int r,int g,int b);
void fill_flood(int x,int y,
				ssh_color fill,ssh_color border);/* Wypelnia powodziowo lub algorytmem siania */
void fill_flood_rgb(int x,int y,
				int rf,int gf,int bf,int rb,int gb,int bb);
/* RYSOWANIE  */
void line_d(int x1,int y1,int x2,int y2);/* Wyswietlenie lini w kolorze domyslnym */
void line(int x1,int y1,int x2,int y2,
				ssh_color c);			/* Wyswietlenie lini w kolorze c */

void circle_d(int x,int y,int r);		/* Wyswietlenie okregu w kolorze domyslnym */
void circle(int x,int y,int r,
				ssh_color c);			/* Wyswietlenie okregu w kolorze c */

void fill_circle_d(int x,int y,int r);	/* Wyswietlenie kola w kolorach domyslnych*/
void fill_circle(int x,int y,int r,
				ssh_color c);			/* Wyswietlenie kola w kolorze c */

void fill_rect_d(int x1,int y1,int x2,int y2);/* Wyswietla prostokat w kolorach domyslnych*/
void fill_rect(int x1,int y1,int x2,int y2,
				ssh_color c);			/* Wyswietla prostokat w kolorze c */

void fill_poly_d(int vx,int vy,
				const ssh_point points[],int number);/* Wypelnia wielokat przesuniety o vx,vy w kolorach domyslnych */
void fill_poly(int vx,int vy,
				const ssh_point points[],int number,
				ssh_color c);		/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */

/* POBIERANIE ZNAKOW Z KLAWIATURY i ZDAZEN OKIENNYCH (w tym z MENU) */
int  input_ready(void); 	/* Funkcja sprawdzajaca czy jest cos do wziecia z wejscia */
int  get_char(void);    	/* Funkcja odczytywania znakow sterowania */
int  set_char(int);		/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
						/* Pewne jest tylko odeslanie jednego znaku. */

int  get_mouse_event(int* xpos,int* ypos,int* click);/* Funkcja odczytujaca ostatnie zdazenie myszy */
int  repaint_area(int* x,int* y,int* width,int* height);/* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
						/* Jesli zwraca -1 to brak danych lub brak implementacji ! Odrysowac calosc. */
						/* Jesli zwraca -2 to znaczy ze dane juz zostaly odczytane. Nalezy zignorowac. */

/* DLA close_plot() */
extern int WB_error_enter_before_clean/* =0 */; /* Czy zamykac od razu czy dac szanse na przejzenie zawartosci */


#ifdef __cplusplus
/* FUNKCJE PRZESTARZALE DOSTEPNE TYLKO DLA KOMPILATORA C++ */
inline int  get_buffering(void){return 	buffered();}    // Zwraca 1 jesli buforowane
inline int  get_fixed(void){return fixed();}		    // Czy okno ma zafiksowana wielkosc
inline unsigned get_background(void){return background();}	// Aktualny kolor tla...

};
#endif
#endif

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                               (Don't change or remove this note) */
/********************************************************************/



